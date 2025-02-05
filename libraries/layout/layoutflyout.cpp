#include "layoutflyout.h"
#include "layoutcalcs.h"

LayoutFlyout::LayoutFlyout(Private privateTag, LayoutContext *context, LayoutPtr buttonContent, LayoutPtr flyoutContent)
    : LayoutButton{privateTag, context, buttonContent, ButtonType::checkbox}, content{flyoutContent}
{
    setParentsOfChildren();
}

void LayoutFlyout::showFlyout(const PropertyBag& parentProps, bool show)
{
    if (show) {
        auto bound = content->getBound(parentProps);
        int menuWidth = std::max(bound.xBound.minSize, 20);
        int menuHeight = std::max(bound.yBound.minSize, 20);
        RectI contentRect = {{pos.x, pos.y}, menuWidth, menuHeight};
        contentRect = positionOverlay(context->getWindowRect(), thisRect(), isHorizontal ? thisRect().bottomCenter() : thisRect().rightCenter(), contentRect, OverlayStyle::menu);
        content->resize(parentProps, contentRect);
        setOverlay(content);
    }
    else  {
        closeOverlay();
    }
}

void LayoutFlyout::onButtonPress(const PropertyBag& parentProps, bool buttonValue)
{
    showFlyout(parentProps, buttonValue);
}
