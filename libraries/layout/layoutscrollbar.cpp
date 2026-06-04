
#include "layoutscrollbar.h"
#include "rectdraw.h"

using namespace mssm;

LayoutScrollBar::LayoutScrollBar(Private privateTag, LayoutContext *context, bool isHorizontal)
    : LayoutBase{context}, isHorizontal{isHorizontal}
{
    setParentsOfChildren();
}

LayoutBase::EvtRes LayoutScrollBar::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    const int dragThreshold = 10;

    switch (evt.action) {
    case MouseEvt::Action::exitOverlayParent:
        break;
    case MouseEvt::Action::none:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::scroll:
        applyWheel(static_cast<int>(evt.dragDelta.y));
        context->setNeedsResize();
        return EvtRes::consumed;
    case MouseEvt::Action::move:
        hovering = true;
        break;
    case MouseEvt::Action::drag:
        hovering = true;
        if (evt.dragMax > dragThreshold) {
            int dragDistPixels = isHorizontal ? static_cast<int>(evt.dragDelta.x) : static_cast<int>(evt.dragDelta.y);
            auto origPos = posFromOffset(dragStartOffset);
            auto newPos = origPos + dragDistPixels;
            scrollOffset = offsetFromPos(newPos);
            context->setNeedsResize();
        }
        break;
    case MouseEvt::Action::press:
        hovering = true;
        dragStartOffset = scrollOffset;
        grabMouse();
        break;
    case MouseEvt::Action::release:
        if (!evt.insideElement) {
            hovering = false;
        }
        if (hasDragFocus()) {
            releaseMouse();
        }
        break;
    case MouseEvt::Action::exit:
        if (!hasDragFocus()) {
            hovering = false;
        }
        break;
    }

    return EvtRes::propagate;
}

void LayoutScrollBar::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    handleSize = std::max(minHandleSize, static_cast<int>((isHorizontal ? width : height) * proportion));
    int handlePos = posFromOffset(scrollOffset);
    RectI handleRect = calcHandleRect(handlePos);

    const auto medGrey = rgb(35, 35, 35);
    const auto darkGrey = rgb(10, 10, 10);
    const auto ltGrey = rgb(84, 84, 84);

    drawRect(g, *this, TRANS, hovering ? medGrey : darkGrey);
    handleRect.shrink(2);
    drawRect(g, handleRect, TRANS, hovering ? ltGrey : medGrey);
}

SizeBound2d LayoutScrollBar::getBound(const PropertyBag& parentProps)
{
    return {SizeBound{0}, SizeBound{0}};
}

void LayoutScrollBar::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
}

void LayoutScrollBar::foreachChildImpl(std::function<void(LayoutBase *)> f,
                                       ForeachContext context,
                                       bool includeOverlay,
                                       bool includeCollapsed)
{
}

void LayoutScrollBar::applyWheel(int count)
{
    double t = tFromOffset(scrollOffset);
    t -= count * wheelStep;
    t = std::clamp(t, 0., 1.);
    scrollOffset = offsetFromT(t);
    context->setNeedsResize();
}
