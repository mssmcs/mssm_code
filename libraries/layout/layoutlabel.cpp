
#include "layoutlabel.h"
#include "rectdraw.h"

using namespace mssm;

LayoutLabel::LayoutLabel(Private privateTag, LayoutContext *context, std::string text, const FontInfo &sizeAndFace)
    : LayoutBase{context}, text{text}, sizeAndFace{sizeAndFace}
{
    hAlign = HAlign::center;
    vAlign = VAlign::center;
    
    setParentsOfChildren();
}

LayoutLabel::~LayoutLabel()
{
    std::cout << "Deleting label: " << text << std::endl;
}

void LayoutLabel::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    Vec2d tPos = textPos(thisRect(), hAlign, vAlign);
    if (borderColor.a > 0 || backgroundColor.a > 0) {
        drawRect(g, thisRect(), borderColor, backgroundColor);
    }
    g.text(tPos, sizeAndFace, text, textColor, hAlign, vAlign);
}

SizeBound2d LayoutLabel::getBound(const PropertyBag& parentProps)
{
    TextExtents extents;
    context->textExtents(sizeAndFace, text, extents);
    SizeBound2d bound{SizeBound(extents.textWidth), SizeBound(extents.fontHeight, extents.fontHeight)};
    grow(bound, padding);
    return bound;
    
    //    return {SizeBound(extents.textWidth+padding.left+padding.right), SizeBound(extents.fontHeight + , extents.fontHeight + 4)};
}

void LayoutLabel::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
}

void LayoutLabel::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}
