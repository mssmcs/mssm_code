
#include "layoutscroll.h"

LayoutScroll::LayoutScroll(Private privateTag, LayoutContext *context, LayoutPtr child)
    : LayoutBase{context}, child{child}
{
    hScroll = LayoutSlider::make(context, true, 0, 0, 0);
    vScroll = LayoutSlider::make(context, false, 0, 0, 0);
    setParentsOfChildren();
}

LayoutBase::EvtRes LayoutScroll::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{/*
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::move:
    case MouseEvt::Action::drag:
    case MouseEvt::Action::press:
    case MouseEvt::Action::release:
    case MouseEvt::Action::exit:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::scroll:
        return EvtProp::defer;
        // if (!vScroll->within(evt.pos) && !hScroll->within(evt.pos)) {
        //     vScroll->applyWheel(evt.dragDelta.y);
        //     context->setNeedsResize();
        // }
        break;
    }*/
    
    return EvtRes::propagate;
}

LayoutBase::EvtRes LayoutScroll::onMouseDeferred(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::move:
    case MouseEvt::Action::drag:
    case MouseEvt::Action::press:
    case MouseEvt::Action::release:
    case MouseEvt::Action::exit:
    case MouseEvt::Action::enter:
        
        break;
    case MouseEvt::Action::scroll:
        if (!vScroll->within(evt.pos) && !hScroll->within(evt.pos)) {
            vScroll->applyWheel(evt.dragDelta.y);
            context->setNeedsResize();
            std::cout << "scrolling" << std::endl;
            return EvtRes::consumed;
        }
        break;
    }
    
    return EvtRes::propagate;
}

void LayoutScroll::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    pushClip(g, thisRect(), false);
    
    if (extraX) {
        hScroll->draw(parentProps, g);
    }
    
    if (extraY) {
        vScroll->draw(parentProps, g);
    }
    
    constexpr const int scrollGutter = 2;
    int vScrollWidth = vScroll->width ? vScroll->width + scrollGutter : 0;
    int hScrollHeight = hScroll->height ? hScroll->height + scrollGutter : 0;
    g.pushClip(left(), top(), std::max(0,width-vScrollWidth), std::max(0,height-hScrollHeight), false);
    child->draw(parentProps, g);
    popClip(g);
}

SizeBound2d LayoutScroll::getBound(const PropertyBag& parentProps)
{
    return {SizeBound(0), SizeBound(0)};
}

void LayoutScroll::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
    
    xScroll = hScroll->value;
    yScroll = vScroll->value;
    contentRect.pos = rect.pos-Vec2i32{xScroll, yScroll};
    
    auto childBound = child->getBound(parentProps);
    
    contentRect.width = childBound.xBound.minSize;
    contentRect.height = childBound.yBound.minSize;
    
    // compute what doesn't fit into rect
    extraX = contentRect.width - rect.width;
    extraY = contentRect.height - rect.height;
    
    if (extraX > 0) {
        extraX += barSize;
        xScroll = std::max(0, std::min(xScroll, extraX));
    }
    else {
        extraX = 0;
        xScroll = 0;
    }
    
    if (extraY > 0) {
        extraY += barSize;
        yScroll = std::max(0, std::min(yScroll, extraY));
    }
    else {
        extraY = 0;
        yScroll = 0;
    }
    
    if (extraX) {
        auto bar = bottomSubrect(barSize, 2);
        if (extraY) {
            bar.width -= barSize;
        }
        hScroll->resize(parentProps, bar);
    }
    else {
        hScroll->height = 0;
    }
    
    if (extraY) {
        auto bar = rightSubrect(barSize, 2);
        if (extraX) {
            bar.height -= barSize;
        }
        vScroll->resize(parentProps, bar);
    }
    else {
        vScroll->width = 0;
    }
    
    hScroll->proportion = (double(rect.width-vScroll->width)/contentRect.width);
    hScroll->minValue = 0;
    hScroll->maxValue = extraX;
    hScroll->value = xScroll;
    
    vScroll->proportion = (double(rect.height-hScroll->height)/contentRect.height);
    vScroll->minValue = 0;
    vScroll->maxValue = extraY;
    vScroll->value = yScroll;
    
    contentRect.pos = rect.pos-Vec2i32{xScroll, yScroll};
    
    child->resize(parentProps, contentRect);
}

void LayoutScroll::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    if (hScroll) {
        f(hScroll.get());
    }
    if (vScroll) {
        f(vScroll.get());
    }
    if (child) {
        f(child.get());
    }
}
