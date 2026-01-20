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

LayoutButtonBase::LayoutButtonBase(Private privateTag, LayoutContext *context, ButtonType buttonType)
    : LayoutBase{context}, type{buttonType}
{
}

void LayoutButtonBase::onButtonPress(const PropertyBag& parentProps, bool pressValue)
{

}

void LayoutButtonBase::setChecked(bool newChecked)
{
    if (checked == newChecked) {
        return;
    }
    checked = newChecked;
}

LayoutBase::EvtRes LayoutButtonBase::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::scroll:
        break;
    case MouseEvt::Action::enter:
    case MouseEvt::Action::move:
        mode = DrawMode::hover;
        break;
    case MouseEvt::Action::drag:
        if (evt.insideElement) {
            mode = DrawMode::pressing;
        }
        else {
            mode = DrawMode::normal;
        }
        break;
    case MouseEvt::Action::press:
        mode = DrawMode::pressing;
        grabMouse();
        if (type == ButtonType::checkbox) {
            checked = !checked;
            onButtonPress(parentProps, checked);
        }
        else if (type == ButtonType::radio) {
            checked = true;
            onButtonPress(parentProps, checked);
        }
        break;
    case MouseEvt::Action::release:
        if (evt.insideElement) {
            if (hasDragFocus() && type == ButtonType::normal) {
                onButtonPress(parentProps, true);
            }
            mode = DrawMode::hover;
        }
        else {
            mode = DrawMode::normal;
        }
        if (hasDragFocus()) {
            releaseMouse();
        }
        break;
    case MouseEvt::Action::exit:
        mode = DrawMode::normal;
        break;
    case MouseEvt::Action::exitOverlayParent:
        break;
    }

    return EvtRes::propagate;
}




































































void ButtonSet::onButtonPress(const PropertyBag &parentProps, LayoutButtonPtr button, int buttonIndex, bool checked)
{
    if (isRadio) {
        for (int i = 0; i < buttons.size(); ++i) {
            if (i != buttonIndex) {
                buttons[i]->setChecked(false);
            }
        }
    }
}













void HoverTrack::onMouse(LayoutBase* element, const MouseEvt &evt, bool captureOnPress)
{
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::scroll:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::move:
        mode = DrawMode::hover;
        break;
    case MouseEvt::Action::drag:
        if (evt.insideElement) {
            mode = DrawMode::pressing;
        }
        else {
            mode = DrawMode::normal;
        }
        break;
    case MouseEvt::Action::press:
        mode = DrawMode::pressing;
        if (captureOnPress) {
            element->grabMouse();
        }
        break;
    case MouseEvt::Action::release:
        if (evt.insideElement) {
            // if (type == ButtonType::normal) {
            //     onButtonPress(true);
            // }
            mode = DrawMode::hover;
        }
        else {
            mode = DrawMode::normal;
        }
        if (element->hasDragFocus()) {
            element->releaseMouse();
        }
        break;
    case MouseEvt::Action::exit:
        mode = DrawMode::normal;
        break;
    case MouseEvt::Action::exitOverlayParent:
        break;
    }
}




























































