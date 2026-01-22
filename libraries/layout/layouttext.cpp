
#include "layouttext.h"

using namespace mssm;

LayoutText::LayoutText(Private privateTag, LayoutContext *context, std::string text)
    : LayoutBase{context}, editBox{*context, context->defaultFont(), text}
{
    setParentsOfChildren();
}

LayoutBase::EvtRes LayoutText::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    switch (evt.action) {
    case MouseEvt::Action::exitOverlayParent:
        break;
    case MouseEvt::Action::none:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::exit:
        break;
    case MouseEvt::Action::move:
        break;
    case MouseEvt::Action::drag:
        if (hasDragFocus()) {
            editBox.onDrag(evt.pos);
        }
        break;
    case MouseEvt::Action::press:
        if (editBox.onClick(evt.pos)) {
            grabKeyboard();
            grabMouse();
        }
        break;
    case MouseEvt::Action::release:
        if (hasDragFocus()) {
            releaseMouse();
        }
        break;
    case MouseEvt::Action::scroll:
        break;
    }
    
    return EvtRes::propagate;
}

LayoutBase::EvtRes LayoutText::onKey(const PropertyBag &parentProps, const KeyEvt &key)
{
    switch (key.action) {
    case KeyEvt::Action::press:
    case KeyEvt::Action::repeat:
        if (hasKeyFocus()) {
            if (std::isprint(key.key)) {
                editBox.addChar(key.key, key.hasShift());
                return EvtRes::consumed;
            }
            else {
                switch (key.key) {
                case Key::Left:
                    editBox.onLeft(key.hasShift());
                    break;
                case Key::Right:
                    editBox.onRight(key.hasShift());
                    break;
                case Key::Delete:
                    editBox.onDelete();
                    break;
                case Key::Backspace:
                    editBox.onBackspace();
                    break;
                }
            }
        }
        break;
    case KeyEvt::Action::release:
        break;
    }
    return EvtRes::propagate;
}

void LayoutText::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    editBox.draw(g, hasKeyFocus());
}

SizeBound2d LayoutText::getBound(const PropertyBag& parentProps)
{
    return {SizeBound(editBox.boxHeight()*2), SizeBound(editBox.boxHeight(), editBox.boxHeight())};
}

void LayoutText::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
    auto bound = getBound(parentProps);
    RectI contentRect;
    contentRect.pos = rect.pos;
    contentRect.width = bound.xBound.constrain(rect.width);
    contentRect.height = bound.yBound.constrain(rect.height);
    editBox.setRect({{contentRect.pos.x, contentRect.pos.y}, contentRect.width, contentRect.height});
}

void LayoutText::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}
