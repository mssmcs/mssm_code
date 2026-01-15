#include "layoutmanager.h"

LayoutManager::LayoutManager(LayoutContext *context, LayoutPtr layout)
    : context{context}
    , layout{layout}
{
    layout->updateLayer(0, 0);
    layout->setOuterMargins(5);
 //   mssm::CoreWindow &g{*context->getWindow()};

    lastMousePos = context->mousePos();
}

LayoutManager::LayoutManager(LayoutContext *context, LayoutHelper::Builder builder)
    : LayoutManager(context, builder(context))
{}

void LayoutManager::propagateEvents(const PropertyBag &parentProps)
{
    //mssm::CoreWindow &g{*context->getWindow()};

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
        case mssm::EvtType::MousePress:
            evt.action = MouseEvt::Action::press;
            evt.pos = {e.x, e.y};
            evt.mods = e.mods;
            evt.button = e.mouseButton();
            propagateMouse(parentProps, screenRect, evt);
            sent = true;
            break;
        case mssm::EvtType::MouseRelease:
            evt.action = MouseEvt::Action::release;
            evt.pos = {e.x, e.y};
            evt.mods = e.mods;
            evt.button = e.mouseButton();
            propagateMouse(parentProps, screenRect, evt);
            sent = true;
            break;
        case mssm::EvtType::MouseMove:
            if (e.arg
                & 0x01) { // TODO: hack:  arg = BITMASK OF BUTTONS!  1 = 0  2 = 1  4 = 2,  x and y = mouse pos
                evt.action = MouseEvt::Action::drag;
                evt.pos = {e.x, e.y};
                evt.mods = e.mods;
                evt.button = mssm::MouseButton::Left;
                evt.dragDelta = evt.pos - context->mouseDragStart(mssm::MouseButton::Left);
                evt.dragMax = context->maxDragDistance(mssm::MouseButton::Left);
                propagateMouse(parentProps, screenRect, evt);
                sent = true;
            }
            break;
        case mssm::EvtType::MouseWheel:
            evt.action = MouseEvt::Action::scroll;
            evt.pos = {e.x, e.y};
            evt.mods = e.mods;
            evt.dragDelta = {0.0, e.arg};
            evt.button = mssm::MouseButton::None;
            propagateMouse(parentProps, screenRect, evt);
            break;
        case mssm::EvtType::KeyPress:
            keyEvt.action = KeyEvt::Action::press;
            keyEvt.key = e.key();
            keyEvt.mods = e.mods;
            propagateKey(parentProps, screenRect, keyEvt);
            break;
        case mssm::EvtType::KeyRepeat:
            keyEvt.action = KeyEvt::Action::repeat;
            keyEvt.key = e.key();
            keyEvt.mods = e.mods;
            propagateKey(parentProps, screenRect, keyEvt);
            break;
        case mssm::EvtType::KeyRelease:
            keyEvt.action = KeyEvt::Action::release;
            keyEvt.key = e.key();
            keyEvt.mods = e.mods;
            propagateKey(parentProps, screenRect, keyEvt);
            break;
        case mssm::EvtType::MusicEvent:
            break;
        case mssm::EvtType::WindowResize:
            break;
        }
    }

    if (!sent && moved) {
        evt.action = MouseEvt::Action::move;
        evt.pos = context->mousePos();
        evt.button = mssm::MouseButton::Left;
        propagateMouse(parentProps, screenRect, evt);
    }
}

void LayoutManager::draw(mssm::CoreWindow& window, mssm::Canvas2d &g)
{

    PropertyBag parentProps;

    RectI screenRect{{0, 0}, g.width(), g.height()};

    context->updateWindowRect(screenRect);

    // TODO: is this just debugging?
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
        LayoutBase::ForeachContext::drawing, false, true);

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
            layout->updateLayer(0, 0);
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
            mssm::CoreWindowCursor::arrow); // reset layout cursor so that it auto defaults to arrow

        if (layout->hasDragFocus()) {
            g.rect({0, 0}, g.width() - 1, g.height() - 1, mssm::RED);
        }
    }

    int yPos = 25;

    layout->traversePreOrder(
        [&](LayoutBase *element) {
            std::string txt = element->getTypeStr() + " " + element->getName();
            g.text({10*element->getDepth(), yPos}, 20, txt);
            yPos += 20;
        },
        LayoutBase::ForeachContext::drawing, false, true);
}

LayoutPtr LayoutManager::findByName(std::string name)
{
    return layout->findByName(name);
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
    if (evt.key == mssm::Key::Tab) {
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
