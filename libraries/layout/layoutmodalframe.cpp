
#include "layoutmodalframe.h"

LayoutModalFrame::LayoutModalFrame(Private privateTag, LayoutContext *context, LayoutPtr child, int width, int height)
    : LayoutAdapter(privateTag, context, child)
{
    setRect(RectI{{0,0},width,height});
}

LayoutBase::EvtRes LayoutModalFrame::onMouseDeferred(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    return EvtRes::consumed;
}

LayoutBase::EvtRes LayoutModalFrame::onKeyDeferred(const PropertyBag &parentProps, const KeyEvt &evt)
{
    return EvtRes::consumed;
}

SizeBound2d LayoutModalFrame::getBound(const PropertyBag &parentProps)
{
    return SizeBound2d(width, height, width, height);
}

void LayoutModalFrame::resize(const PropertyBag &parentProps, const RectI &rect)
{
    RectI newRect{{0, 0}, width, height};
    setRect(newRect.centered(rect));
    child->resize(parentProps, thisRect());
}

LayoutBase::EvtRes LayoutModalFrame::onMouse(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    (void)parentProps;
    (void)reason;
    (void)evt;
    return EvtRes::propagate;
}

LayoutBase::EvtRes LayoutModalFrame::onKey(const PropertyBag &parentProps, const KeyEvt &evt)
{
    (void)parentProps;
    (void)evt;
    return EvtRes::propagate;
}
