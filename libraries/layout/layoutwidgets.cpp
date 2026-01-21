#include "layoutwidgets.h"
#include "rectdraw.h"
#include "vec3d.h"
#include "layoutcalcs.h"

using namespace mssm;













LayoutColor::LayoutColor(Private privateTag, LayoutContext *context, mssm::Color color, SizeBound2d bound)
    : LayoutBase{context}, color{color}, bound{bound}
{
    setParentsOfChildren();
}

LayoutColor::~LayoutColor()
{
    std::cout << "Deleting Color: " << color.toRealVec3<Vec3d>() << std::endl;
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
    if (evt.action == MouseEvt::Action::press) {

        auto label = LayoutLabel::make(context, "ToolTip Example");

        label->backgroundColor = mssm::BLACK;
        label->borderColor = mssm::GREY;

        Vec2d offset = evt.pos - Vec2d{thisRect().pos};
        LayoutPtr tip = LayoutTooltipAnchor::make(context, label, offset);

        addToolTip(parentProps, evt.pos, tip);

        return LayoutBase::EvtRes::consumed;
    }
    return LayoutBase::EvtRes::propagate;
}



































































