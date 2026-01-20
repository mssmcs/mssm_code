
#include "layoutadapter.h"

LayoutAdapter::LayoutAdapter(Private privateTag, LayoutContext *context, LayoutPtr child)
    : LayoutBase(context)
    , child(child)
{
    setParentsOfChildren();
}

void LayoutAdapter::draw(const PropertyBag& parentProps, mssm::Canvas2d &g)
{
    child->draw(parentProps, g);
}

SizeBound2d LayoutAdapter::getBound(const PropertyBag& parentProps)
{
    return child->getBound(parentProps);
}

void LayoutAdapter::resize(const PropertyBag& parentProps, const RectI& rect)
{
    child->resize(parentProps, rect);
    setRect(rect);
}

void LayoutAdapter::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context,
                                     bool includeOverlay,
                                     bool includeCollapsed)
{
    if (child) {
        f(child.get());
    }
}


LayoutAdapterPadded::LayoutAdapterPadded(Private privateTag,
                                         LayoutContext *context,
                                         LayoutPtr child,
                                         Padding padding)
    : LayoutAdapter(privateTag, context, child), padding{padding}
{

}

SizeBound2d LayoutAdapterPadded::getBound(const PropertyBag& parentProps)
{
    auto bound = child->getBound(parentProps);
    grow(bound, padding);
    return bound;
}

void LayoutAdapterPadded::resize(const PropertyBag& parentProps, const RectI& rect)
{
    child->resize(parentProps, shrunk(rect, padding));
    setRect(rect);
}

LayoutAdapterClickable::LayoutAdapterClickable(Private privateTag,
                                               LayoutContext *context,
                                               LayoutPtr child,
                                               Padding padding,
                                               std::function<void()> clickCallback)
    : LayoutAdapterPadded{privateTag, context, child, padding}, clickCallback{clickCallback}
{

}

LayoutBase::EvtRes LayoutAdapterClickable::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::enter:
    case MouseEvt::Action::exit:
    case MouseEvt::Action::move:
    case MouseEvt::Action::drag:
    case MouseEvt::Action::release:
    case MouseEvt::Action::scroll:
        break;
    case MouseEvt::Action::press:
        clickCallback();
        break;
    case MouseEvt::Action::exitOverlayParent:
        break;
    }

    return EvtRes::consumed;
}
