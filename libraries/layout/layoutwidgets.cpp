#include "layoutwidgets.h"
#include "rectdraw.h"
#include "layoutcalcs.h"

using namespace mssm;













LayoutColor::LayoutColor(Private privateTag, LayoutContext *context, mssm::Color color, SizeBound2d bound)
    : LayoutBase{context}, color{color}, bound{bound}
{
    setParentsOfChildren();
}

LayoutColor::~LayoutColor()
{
}

void LayoutColor::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    drawRoundedRect(g, *this, 0, 10, light(color), color);
}

SizeBound2d LayoutColor::getBound(const PropertyBag& parentProps)
{
    return bound;
}

void LayoutColor::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
}

void LayoutColor::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}

LayoutBase::EvtRes LayoutColor::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    return LayoutBase::EvtRes::propagate;
}



































































