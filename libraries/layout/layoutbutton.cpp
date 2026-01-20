
#include "layoutbutton.h"
#include "rectdraw.h"

using namespace mssm;

LayoutButton::LayoutButton(Private privateTag,
                           LayoutContext *context,
                           LayoutPtr child,
                           LayoutButton::ButtonType buttonType,
                           ButtonCallback callback, int buttonId)
    : LayoutButtonBase{privateTag, context, buttonType},
    child{child},
    callback{callback},
    buttonId{buttonId}
{
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
    
    drawRect(g, buttonRect, RED, TRANSPARENT);
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
