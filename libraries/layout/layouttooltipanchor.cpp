
#include "layouttooltipanchor.h"
#include "layoutcalcs.h"

LayoutTooltipAnchor::LayoutTooltipAnchor(Private privateTag, LayoutContext *context, LayoutPtr child, Vec2d offset)
    : LayoutAdapter(privateTag, context, child), offset{offset}
{
    
}

SizeBound2d LayoutTooltipAnchor::getBound(const PropertyBag &parentProps)
{
    auto bound = child->getBound(parentProps);
    
    return bound;
}

void LayoutTooltipAnchor::resize(const PropertyBag &parentProps, const RectI &rect)
{
    auto bound = getBound(parentProps);
    int tipWidth = std::max(bound.xBound.minSize, 20);
    int tipHeight = std::max(bound.yBound.minSize, 20);
    Vec2i32 p = parent->pos + Vec2i32{offset};
    RectI tipRect = {{p.x, p.y}, tipWidth, tipHeight};
    tipRect = positionOverlay(context->getWindowRect(), thisRect(), p, tipRect, OverlayStyle::tooltip);
    child->resize(parentProps, tipRect);
    setRect(*child);
}
