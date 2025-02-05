#include "layoutmanager.h"

LayoutManager::LayoutManager(LayoutContext *context, LayoutPtr layout)
    : context{context}
    , layout{layout}
{
    layout->updateLayer(0, 0);
    layout->setOuterMargins(5);
 //   gjh::CoreWindow &g{*context->getWindow()};

    lastMousePos = context->mousePos();
}

LayoutManager::LayoutManager(LayoutContext *context, LayoutHelper::Builder builder)
    : LayoutManager(context, builder(context))
{}

void LayoutManager::propagateEvents(const PropertyBag &parentProps)
{
    //gjh::CoreWindow &g{*context->getWindow()};

    MouseEvt evt;
    KeyEvt keyEvt;

    RectI screenRect = context->getWindowRect();
    if (screenRect.width == 0 || screenRect.height == 0) {
        return;
    }

    bool moved{false};

    if (!lastMousePos.exactlyEquals(context->mousePos())) {
        lastMousePos = context->mousePos();
        moved = true;
    }

  //    RectI screenRect{{0, 0}, g.width(), g.height()};

    bool sent{false};

    for (auto &e : context->events()) {
        switch (e.evtType) {
        case gjh::EvtType::MousePress:
            evt.action = MouseEvt::Action::press;
            evt.pos = {e.x, e.y};
            evt.mods = e.mods;
            evt.button = e.mouseButton();
            propagateMouse(parentProps, screenRect, evt);
            sent = true;
            break;
        case gjh::EvtType::MouseRelease:
            evt.action = MouseEvt::Action::release;
            evt.pos = {e.x, e.y};
            evt.mods = e.mods;
            evt.button = e.mouseButton();
            propagateMouse(parentProps, screenRect, evt);
            sent = true;
            break;
        case gjh::EvtType::MouseMove:
            if (e.arg
                & 0x01) { // TODO: hack:  arg = BITMASK OF BUTTONS!  1 = 0  2 = 1  4 = 2,  x and y = mouse pos
                evt.action = MouseEvt::Action::drag;
                evt.pos = {e.x, e.y};
                evt.mods = e.mods;
                evt.button = gjh::MouseButton::Left;
                evt.dragDelta = evt.pos - context->mouseDragStart(gjh::MouseButton::Left);
                evt.dragMax = context->maxDragDistance(gjh::MouseButton::Left);
                propagateMouse(parentProps, screenRect, evt);
                sent = true;
            }
            break;
        case gjh::EvtType::MouseWheel:
            evt.action = MouseEvt::Action::scroll;
            evt.pos = {e.x, e.y};
            evt.mods = e.mods;
            evt.dragDelta = {0.0, e.arg};
            evt.button = gjh::MouseButton::None;
            propagateMouse(parentProps, screenRect, evt);
            break;
        case gjh::EvtType::KeyPress:
            keyEvt.action = KeyEvt::Action::press;
            keyEvt.key = e.key();
            keyEvt.mods = e.mods;
            propagateKey(parentProps, screenRect, keyEvt);
            break;
        case gjh::EvtType::KeyRepeat:
            keyEvt.action = KeyEvt::Action::repeat;
            keyEvt.key = e.key();
            keyEvt.mods = e.mods;
            propagateKey(parentProps, screenRect, keyEvt);
            break;
        case gjh::EvtType::KeyRelease:
            keyEvt.action = KeyEvt::Action::release;
            keyEvt.key = e.key();
            keyEvt.mods = e.mods;
            propagateKey(parentProps, screenRect, keyEvt);
            break;
        case gjh::EvtType::MusicEvent:
            break;
        case gjh::EvtType::WindowResize:
            break;
        }
    }

    if (!sent && moved) {
        evt.action = MouseEvt::Action::move;
        evt.pos = context->mousePos();
        evt.button = gjh::MouseButton::Left;
        propagateMouse(parentProps, screenRect, evt);
    }
}

void LayoutManager::draw(gjh::CoreWindow& window, mssm::Canvas2d &g)
{

    PropertyBag parentProps;

    RectI screenRect{{0, 0}, g.width(), g.height()};

    context->updateWindowRect(screenRect);

    layout->traversePreOrder(
        [&](LayoutBase *element) {
            if (element->getDepth() > 0) {
                auto pp = element->getParentPtr();
                if (pp) {
                    if (pp->getDepth() != element->getDepth() - 1) {
                        throw std::runtime_error("Parent depth is not correct");
                    }
                } else {
                    throw std::runtime_error("Parent is null");
                }
            }
        },
        false, true);

    propagateEvents(parentProps);

    bool collapsed = g.width() == 0 || g.height() == 0;

    if (g.width() != lastWidth || g.height() != lastHeight) {
        if (g.width() == 0 || g.height() == 0) {
            // minimized?
        } else {
            context->setNeedsResize();
            lastWidth = g.width();
            lastHeight = g.height();
        }
    }

    if (!collapsed) {
        if (context->getNeedsResize()) {
            std::cout << "Resizing" << std::endl;
            layout->resize(parentProps, screenRect);
            context->clearNeedsResize();
        }

        g.resetClip();

        layout->draw(parentProps, g);

        g.resetClip();

        for (LayoutPtr overlay : context->overlays) {
            overlay->draw(parentProps, g);
        }

        layout->debugDraw(g);

        window.setCursor(context->getCursor()); // set cursor to whatever was set by layout

        context->setCursor(
            gjh::CoreWindowCursor::arrow); // reset layout cursor so that it auto defaults to arrow

        if (layout->hasDragFocus()) {
            g.rect({0, 0}, g.width() - 1, g.height() - 1, mssm::RED);
        }
    }
}

LayoutBase::EvtProp LayoutManager::propagateMouse(const PropertyBag &parentProps, const RectI &clip, MouseEvt &evt)
{
    LayoutBase::EvtProp res = LayoutBase::EvtProp::propagate;

    LayoutPtr newHoverElement;

    if (!context->overlays.empty()) {
        for (int i = context->overlays.size() - 1; i >= 0; i--) {
            if (i >= context->overlays.size()) {
                // overlay removed while propagating?
                std::cout
                    << "Overlay removed while propagating mouse (handle this in a more general way!"
                    << std::endl;
                break;
            }
            LayoutPtr tmp = context->overlays[i];
            res = tmp->propagateMouse(parentProps, res, clip, evt);
            if (i >= context->overlays.size() || tmp != context->overlays[i]) {
                // overlay removed while propagating?
                std::cout << "WOverlay removed while propagating mouse (handle this in a more "
                             "general way!"
                          << std::endl;
                break;
            }
            if (res != LayoutBase::EvtProp::propagate) {
                break;
            }
        }
    }

    if (res == LayoutBase::EvtProp::propagate) {
        res = layout->propagateMouse(parentProps, res, clip, evt);
    }

    // updateHoverElement(newHoverElement, cast<Vec2i32>(evt.pos));

    return res;
}

LayoutBase::EvtProp LayoutManager::propagateKey(const PropertyBag &parentProps, const RectI &clip, KeyEvt &evt)
{
    if (evt.key == gjh::Key::Tab) {
        context->setDebug(true);
    }

    LayoutBase::EvtProp res = LayoutBase::EvtProp::propagate;

    if (!context->overlays.empty()) {
        for (int i = context->overlays.size() - 1; i >= 0; i--) {
            res = context->overlays[i]->propagateKey(parentProps, clip, evt);
            if (res != LayoutBase::EvtProp::propagate) {
                break;
            }
        }
    }

    if (res == LayoutBase::EvtProp::propagate) {
        res = layout->propagateKey(parentProps, clip, evt);
    }

    return res;
}

// void LayoutManager::updateHoverElement(LayoutPtr newHoverElement, Vec2i32 pos)
// {
//     if (newHoverElement != lastHoverElement) {

//         std::cout << "Hover Element Changed:" << std::endl;
//         if (lastHoverElement) {
//             std::cout << "  Leaving: " << lastHoverElement->trail() << std::endl;

//             if (lastHoverElement->isShowingTooltip()) {
//                 lastHoverElement->closeOverlay();
//             }

//             MouseEvt evt;
//             evt.action = MouseEvt::Action::exit;
//             evt.button = mssm::MouseButton::None;
//             evt.pos = cast<Vec2d>(pos);
//             MouseEventReason reason = MouseEventReason::exit;
//             lastHoverElement->onMouse(reason, evt);
//         }
//         else {
//             std::cout << "  Last Hover was Null" << std::endl;
//         }
//         if (newHoverElement) {
//             std::cout << "  Entering: " << newHoverElement->trail() << std::endl;
//         }
//         else {
//             std::cout << "  New Hover was Null" << std::endl;
//         }

//         lastHoverElement = newHoverElement;
//     }
// }
