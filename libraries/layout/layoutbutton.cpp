
#include "layoutbutton.h"
#include "rectdraw.h"

using namespace mssm;


LayoutButtonBase::LayoutButtonBase(Private privateTag, LayoutContext *context, ButtonType buttonType, ButtonSet2 *buttonSet, double autoPressHoverTime)
    : LayoutBase{context}, type{buttonType}, buttonSet2{buttonSet}
{
    if (buttonSet) {
        buttonSet->add(this, autoPressHoverTime);
    }
}

LayoutButtonBase::~LayoutButtonBase()
{
    if (buttonSet2) {
        buttonSet2->remove(this);
    }
}

void LayoutButtonBase::onButtonPress(const PropertyBag& parentProps, bool pressValue)
{
    if (buttonSet2) {
        buttonSet2->onButtonPress(parentProps, this, pressValue);
    }
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
    mode = hoverTrack.onMouse(this, evt, true);

    // switch (evt.action) {
    // case MouseEvt::Action::none:
    // case MouseEvt::Action::scroll:
    // case MouseEvt::Action::enter:
    // case MouseEvt::Action::move:
    // case MouseEvt::Action::drag:
    //     break;
    // case MouseEvt::Action::press:
    //     mode = DrawMode::pressing;
    //     grabMouse();
    //     if (type == ButtonType::checkbox) {
    //         checked = !checked;
    //         onButtonPress(parentProps, checked);
    //     }
    //     else if (type == ButtonType::radio) {
    //         checked = true;
    //         onButtonPress(parentProps, checked);
    //     }
    //     break;
    // case MouseEvt::Action::release:
    //     if (evt.insideElement) {
    //         if (hasDragFocus() && type == ButtonType::normal) {
    //             onButtonPress(parentProps, true);
    //         }
    //         mode = DrawMode::hover;
    //     }
    //     else {
    //         mode = DrawMode::normal;
    //     }
    //     if (hasDragFocus()) {
    //         releaseMouse();
    //     }
    //     break;
    // case MouseEvt::Action::exit:
    //     mode = DrawMode::normal;
    //     break;
    // case MouseEvt::Action::exitOverlayParent:
    //     break;
    // }

    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::scroll:
    case MouseEvt::Action::enter:
    case MouseEvt::Action::move:
    case MouseEvt::Action::drag:
    case MouseEvt::Action::exit:
    case MouseEvt::Action::exitOverlayParent:
        break;
    case MouseEvt::Action::press:
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
        }
        break;
    }

    return EvtRes::propagate;
}

// void ButtonSet::onButtonPress(const PropertyBag &parentProps, LayoutButtonPtr button, int buttonIndex, bool checked)
// {
//     if (isRadio) {
//         for (int i = 0; i < buttons.size(); ++i) {
//             if (i != buttonIndex) {
//                 buttons[i]->setChecked(false);
//             }
//         }
//     }
// }



LayoutButton::LayoutButton(Private privateTag,
                           LayoutContext *context,
                           LayoutPtr child,
                           LayoutButton::ButtonType buttonType,
                           ButtonCallback callback,
                           int buttonId)
    : LayoutButtonBase{privateTag, context, buttonType, {}},
    child{child},
    callback{callback},
    buttonId{buttonId}
{
    setParentsOfChildren();
}

LayoutButton::LayoutButton(Private privateTag, LayoutContext *context, LayoutPtr child, ButtonType buttonType, ButtonSet2 *buttonSet, double autoPressHoverTime)
    : LayoutButtonBase{privateTag, context, buttonType, buttonSet, autoPressHoverTime},
    child{child},
    callback{},
    buttonId{0}
{
     setCallback(buttonSet->createCallback());
     setParentsOfChildren();
}

void LayoutButton::onButtonPress(const PropertyBag& parentProps, bool pressValue)
{
    if (callback) {
        callback(parentProps, std::static_pointer_cast<LayoutButton>(shared_from_this()), buttonId, pressValue);
    }
}



void LayoutButton::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    switch (type) {
    case ButtonType::normal:
        drawMomentary(parentProps, g, mode);
        break;
    case ButtonType::checkbox:
        drawCheckbox(parentProps, g, mode);
        break;
    case ButtonType::radio:
        drawRadio(parentProps, g, mode);
        break;
    }
}

void LayoutButton::drawCommon(const PropertyBag &parentProps, mssm::Canvas2d &g, mssm::Color color)
{
    RectI buttonRect = thisRect();
    
    
    switch (style) {
    case DrawStyle::button:
        if (roundRadius > 0) {
            drawRoundedRect(g, buttonRect, 0, roundRadius, light(color), color);
        }
        else {
            drawRect(g, buttonRect, light(color), color);
        }
        break;
    case DrawStyle::tabTop:
        buttonRect.growDown(roundRadius);
        drawRoundedRect(g, buttonRect, 0, roundRadius, light(color), color);
        break;
    case DrawStyle::tabBottom:
        buttonRect.growUp(roundRadius);
        drawRoundedRect(g, buttonRect, 0, roundRadius, light(color), color);
        break;
    case DrawStyle::tabLeft:
        buttonRect.growRight(roundRadius);
        drawRoundedRect(g, buttonRect, 0, roundRadius, light(color), color);
        break;
    case DrawStyle::tabRight:
        buttonRect.growLeft(roundRadius);
        drawRoundedRect(g, buttonRect, 0, roundRadius, light(color), color);
        break;
    case DrawStyle::menu:
        drawRect(g, *this, light(color), color);
        break;
    }
    
    //drawRect(g, buttonRect, RED, TRANSPARENT);
}

void LayoutButton::drawMomentary(const PropertyBag &parentProps, mssm::Canvas2d &g, DrawMode mode)
{
    mssm::Color color;
    
    switch (mode) {
    case DrawMode::normal:
        color = rgb(40, 40, 40);
        break;
    case DrawMode::hover:
        color = rgb(70, 70, 70);
        break;
    case DrawMode::pressing:
        color = rgb(101, 101, 101);
        break;
    }
    
    drawCommon(parentProps, g, color);
    
    if (child) {
        child->draw(parentProps, g);
    }
}

void LayoutButton::drawCheckbox(const PropertyBag &parentProps, mssm::Canvas2d &g, DrawMode mode)
{
    mssm::Color color;
    
    if (checked) {
        color = rgb(101, 101, 101);
    }
    else {
        switch (mode) {
        case DrawMode::normal:
            color = rgb(40, 40, 40);
            break;
        case DrawMode::hover:
            color = rgb(70, 70, 70);
            break;
        case DrawMode::pressing:
            color = rgb(101, 101, 101);
            break;
        }
    }
    
    drawCommon(parentProps, g, color);
    
    child->draw(parentProps, g);
}

void LayoutButton::drawRadio(const PropertyBag &parentProps, mssm::Canvas2d &g, DrawMode mode)
{
    mssm::Color color;
    
    if (checked) {
        color = rgb(101, 101, 101);
    }
    else {
        switch (mode) {
        case DrawMode::normal:
            color = rgb(40, 40, 40);
            break;
        case DrawMode::hover:
            color = rgb(70, 70, 70);
            break;
        case DrawMode::pressing:
            color = rgb(101, 101, 101);
            break;
        }
    }
    
    drawCommon(parentProps, g, color);
    
    child->draw(parentProps, g);
}

SizeBound2d LayoutButton::getBound(const PropertyBag& parentProps)
{
    auto bound = child->getBound(parentProps);
    bound.grow(roundRadius/2,roundRadius/2);
    return bound;
}

void LayoutButton::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
    if (child) {
        child->resize(parentProps, rect.makeInset(roundRadius/2));
    }
}

void LayoutButton::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    if (child) {
        f(child.get());
    }
}



ButtonSet2::ButtonSet2(bool isRadio, std::function<void (int, bool)> onPress)
    : isRadio{isRadio}, onPress{onPress}
{

}

void ButtonSet2::setCallback(std::function<void (int, bool)> onPress)
{
    this->onPress = onPress;
}

LayoutButtonBase *ButtonSet2::add(LayoutButtonBase *button, double autoClickTime)
{
    buttons.push_back({ button, autoClickTime });
    return buttons.back().button;
}

void ButtonSet2::remove(LayoutButtonBase *button)
{
    buttons.erase(std::remove_if(buttons.begin(), buttons.end(), [button](auto& b) { return button == b.button; }), buttons.end());
}

std::function<void (const PropertyBag &, LayoutButtonPtr, int, bool)> ButtonSet2::createCallback()
{
    return [this] (const PropertyBag &parentProps, LayoutButtonPtr button, int id, bool pressValue) {
        onButtonPress(parentProps, button.get(), pressValue);
    };
}

void ButtonSet2::onButtonPress(const PropertyBag &parentProps, LayoutButtonBase *button, bool pressValue)
{
    for (int i = 0; i < buttons.size(); ++i) {
        if (buttons[i].button == button) {
            onButtonPress(parentProps, button, i, pressValue);
        }
        else {
            // other button
            if (isRadio && pressValue) {
                buttons[i].button->setChecked(false);
            }
        }
    }
}

void ButtonSet2::onButtonPress(const PropertyBag &parentProps, LayoutButtonBase *button, int buttonIndex, bool pressValue)
{
    if (onPress) {
        onPress(buttonIndex, pressValue);
    }
}

void ButtonSet2::checkClickOnHover(const PropertyBag &parentProps)
{
    for (int i = 0; i < buttons.size(); ++i) {
        auto& b = *buttons[i].button;
        double hoverTime = buttons[i].autoClickTime;
        if (hoverTime != 0 && !b.isChecked() && b.getHoverTime() > hoverTime) {
            b.onButtonPress(parentProps, true);
            b.setChecked(true);
        }
    }
}
