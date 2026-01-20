
#include "layoutslider.h"
#include "rectdraw.h"

using namespace mssm;

LayoutSlider::LayoutSlider(Private privateTag,
                           LayoutContext *context,
                           bool isHorizontal,
                           double minValue,
                           double maxValue,
                           double value)
    : LayoutBase{context}, isHorizontal{isHorizontal}, minValue{minValue}, maxValue{maxValue}, value{value}
{
    setParentsOfChildren();
}

LayoutBase::EvtRes LayoutSlider::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    const int dragThreshold = 10;
    
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::scroll:
        applyWheel(evt.dragDelta.y);
        std::cout << "scrolling2" << std::endl;
        
        context->setNeedsResize();
        return EvtRes::consumed;
    case MouseEvt::Action::move:
        hovering = true;
        break;
    case MouseEvt::Action::drag:
        hovering = true;
        if (evt.dragMax > dragThreshold) {
            int dragDistPixels = isHorizontal ? evt.dragDelta.x : evt.dragDelta.y;
            auto origPos = posFromValue(dragStartValue);
            auto newPos = origPos + dragDistPixels;
            value = valueFromPos(newPos);
            context->setNeedsResize();
        }
        break;
    case MouseEvt::Action::press:
        hovering = true;
        dragStartValue = value;
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

void LayoutSlider::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    // do in resize?
    handleSize = std::max(minHandleSize, static_cast<int>((isHorizontal ? width : height)*proportion));
    int handlePos = posFromValue(value);
    RectI handleRect = calcHandleRect(handlePos);
    
    const auto medGrey = rgb(35, 35, 35);
    const auto darkGrey = rgb(10,10,10);
    const auto ltGrey = rgb(84, 84, 84);
    
    drawRect(g, *this, TRANS, hovering ? medGrey : darkGrey);
    handleRect.shrink(2);
    drawRect(g, handleRect, TRANS, hovering ? ltGrey : medGrey);
}

SizeBound2d LayoutSlider::getBound(const PropertyBag& parentProps)
{
    return SizeBound2d{SizeBound{50}, SizeBound{30}};
}

void LayoutSlider::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
}

void LayoutSlider::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}

void LayoutSlider::stepValue(int count)
{
    double t = tFromValue(value);
    t += count * clickStep;
    t = std::clamp(t, 0., 1.);
    value = valueFromT(t);
}

void LayoutSlider::applyWheel(int count)
{
    double t = tFromValue(value);
    t -= count * wheelStep;
    t = std::clamp(t, 0., 1.);
    value = valueFromT(t);
}
