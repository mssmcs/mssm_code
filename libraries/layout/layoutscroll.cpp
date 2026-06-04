
#include "layoutscroll.h"

LayoutScroll::LayoutScroll(Private privateTag, LayoutContext *context, LayoutPtr child)
    : LayoutBase{context}, child{child}
{
    hScroll = LayoutScrollBar::make(context, true);
    vScroll = LayoutScrollBar::make(context, false);
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
    case MouseEvt::Action::exitOverlayParent:
        break;
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
    RectI viewport{{left(), top()},
                   std::max(0, width - vScrollWidth),
                   std::max(0, height - hScrollHeight)};
    pushClip(g, viewport, false);
    child->draw(parentProps, g);
    popClip(g);
    popClip(g);
}

SizeBound2d LayoutScroll::getBound(const PropertyBag& parentProps)
{
    return {SizeBound(0), SizeBound(0)};
}

void LayoutScroll::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
    
    xScroll = static_cast<int>(hScroll->scrollOffset);
    yScroll = static_cast<int>(vScroll->scrollOffset);

    auto childBound = child->getBound(parentProps);

    contentRect.width = childBound.xBound.minSize;
    contentRect.height = childBound.yBound.minSize;

    int viewW = rect.width;
    int viewH = rect.height;

    extraY = std::max(0, contentRect.height - viewH);
    extraX = std::max(0, contentRect.width - viewW);

    if (extraY > 0) {
        viewH -= barSize;
    }
    if (extraX > 0) {
        viewW -= barSize;
    }
    if (extraY > 0) {
        int extraX2 = std::max(0, contentRect.width - viewW);
        if (extraX2 > 0) {
            extraX = extraX2;
            if (extraX > 0) {
                viewH -= barSize;
            }
        }
    }

    if (extraX > 0) {
        extraX += barSize;
        xScroll = std::max(0, std::min(xScroll, extraX));
    } else {
        extraX = 0;
        xScroll = 0;
    }

    if (extraY > 0) {
        extraY += barSize;
        yScroll = std::max(0, std::min(yScroll, extraY));
    } else {
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
    
    hScroll->proportion = static_cast<double>(rect.width - vScroll->width) / contentRect.width;
    hScroll->scrollMax = extraX;
    hScroll->scrollOffset = xScroll;

    vScroll->proportion = static_cast<double>(rect.height - hScroll->height) / contentRect.height;
    vScroll->scrollMax = extraY;
    vScroll->scrollOffset = yScroll;
    
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
