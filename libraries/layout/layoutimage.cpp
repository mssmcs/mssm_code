
#include "layoutimage.h"

LayoutImage::LayoutImage(Private privateTag, LayoutContext *context, mssm::Image img)
    : LayoutBase{context}, image{img}
{
    setName("image");
    //bound.grow(img.width(),image.height());
    setParentsOfChildren();
}

LayoutImage::~LayoutImage()
{
    
}

void LayoutImage::updateImage(mssm::Image img)
{
    image = img;
}

void LayoutImage::draw(const PropertyBag &parentProps, mssm::Canvas2d &g)
{
    Vec2d pos{thisRect().upperLeft()};
    g.image(pos, width, height, image);
}

SizeBound2d LayoutImage::getBound(const PropertyBag &parentProps)
{
    return bound;
}

void LayoutImage::resize(const PropertyBag &parentProps, const RectI &rect)
{
    setRect(rect);
    // contentRect.pos = rect.pos;
    // contentRect.width = bound.xBound.constrain(rect.width);
    // contentRect.height = bound.yBound.constrain(rect.height);
}

void LayoutImage::foreachChildImpl(std::function<void (LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}

LayoutBase::EvtRes LayoutImage::onMouse(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    return LayoutBase::EvtRes::propagate;
}
