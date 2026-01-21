#include "layouthelper.h"

LayoutHelper::HStack::operator LayoutHelper::Builder() const
{
    auto tmp = children;
    auto config = this->config;
    return [tmp, config](LayoutContext* context) {
        std::vector<LayoutPtr> kids;
        for (auto& c : tmp) {
            kids.push_back(c(context));
        }
        auto widget = LayoutStacked::make(context, true, Justify::begin, CrossJustify::stretch, kids);
        config.applyTo(widget);
        return widget;
    };
}

LayoutHelper::VStack::operator LayoutHelper::Builder() const
{
    auto tmp = children;
    auto config = this->config;
    return [tmp, config](LayoutContext* context) {
        std::vector<LayoutPtr> kids;
        for (auto& c : tmp) {
            kids.push_back(c(context));
        }
        auto widget = LayoutStacked::make(context, false, Justify::begin, CrossJustify::stretch, kids);
        config.applyTo(widget);
        return widget;
    };
}

LayoutHelper::GridRowWrapper::operator LayoutHelper::GridRowBuilder() const
{
    auto tmp = children;  
    return [tmp](LayoutContext* context) {
        std::vector<LayoutPtr> kids;
        for (auto& c : tmp) {
            kids.push_back(c(context));
        }
        return LayoutGridRow::make(context, kids);
    };
}

LayoutHelper::Grid::operator LayoutHelper::Builder() const
{
    auto tmp = children;
    auto config = this->config;
    return [tmp, config](LayoutContext* context) {
        std::vector<LayoutGridRowPtr> kids;
        for (auto& c : tmp) {
            kids.push_back(c(context));
        }
        auto widget = LayoutGrid::make(context, kids);
        config.applyTo(widget);
        return widget;
    };
}

namespace LayoutHelper {
Builder operator|(Wrapper aw, Wrapper bw) {
    Builder a = aw;
    Builder b = bw;

    return [a, b](LayoutContext* context) {
        return LayoutSplitter::make(context, true, a(context), b(context));
    };
}

Builder operator/(Wrapper aw, Wrapper bw) {
    Builder a = aw;
    Builder b = bw;
    return [a, b](LayoutContext* context) {
        return LayoutSplitter::make(context, false, a(context), b(context));
    };
}

Tabs::operator Builder() const
{
    auto tmp = children;
    auto config = this->config;
    bool isHorizontal = this->isHorizontal;

    return [isHorizontal, tmp, config](LayoutContext* context) {
        std::vector<LayoutTabs::Tab> kids;
        for (auto& c : tmp) {
            kids.push_back({c.label, c.content(context)});
        }
        auto widget = LayoutTabs::make(context, isHorizontal, kids);
        config.applyTo(widget);
        return widget;
    };
}


HSplit::operator Builder() const
{
    Builder left = this->left;
    Builder right = this->right;
    auto config = this->config;
    return [left, right, config](LayoutContext *context) {
        auto widget = LayoutSplitter::make(context, true, left(context), right(context));
        config.applyTo(widget);
        return widget;
    };
}

VSplit::operator Builder() const
{
    Builder top = this->top;
    Builder bottom = this->bottom;
    auto config = this->config;
    return [top, bottom, config](LayoutContext *context) {
        auto widget = LayoutSplitter::make(context, false, top(context), bottom(context));
        config.applyTo(widget);
        return widget;
    };
}

Button::operator Builder() const
{
    std::string txt = label;
    auto config = this->config;
    return [txt, config](LayoutContext *context) {
        auto widget = LayoutButton::make(context, LayoutLabel::make(context, txt));
        config.applyTo(widget);
        return widget;
    };
}

Slider::operator Builder() const
{
    double isHorizontal = this->isHorizontal;
    double minValue = this->minValue;
    double maxValue = this->maxValue;
    double value = this->value;
    auto config = this->config;
    return [isHorizontal, minValue, maxValue, value, config](LayoutContext *context) {
        auto widget = LayoutSlider::make(context, isHorizontal, minValue, maxValue, value);
        config.applyTo(widget);
        return widget;
    };
}

Scroll::operator Builder() const
{
    Builder child = this->child;
    auto config = this->config;
    return [child, config](LayoutContext *context) {
        auto widget = LayoutScroll::make(context, child(context));
        config.applyTo(widget);
        return widget;
    };
}

Panel::operator Builder() const
{
    mssm::Color color = this->color;
    auto bound = this->bound;
    auto config = this->config;
    return [color, config, bound](LayoutContext *context) {
        auto widget = LayoutColor::make(context, color, bound);
        config.applyTo(widget);
        return widget;
    };
}

ImagePanel::operator Builder() const
{
    mssm::Image image = this->image;
    auto config = this->config;
    return [image, config](LayoutContext *context) {
        auto widget = LayoutImage::make(context, image);
        config.applyTo(widget);
        return widget;
    };
}

Menu::operator Builder() const
{
    auto tmp = children;
    auto config = this->config;
    bool isHorizontal = this->isHorizontal;

    return [isHorizontal, tmp, config](LayoutContext* context) {
        std::vector<LayoutMenu::Item> kids;
        for (auto& c : tmp) {
            kids.push_back({c.label, c.content(context)});
        }
        auto widget = LayoutMenu::make(context, isHorizontal, kids);
        config.applyTo(widget);
        return widget;
    };
}

Flyout::operator Builder() const
{
    auto tmp = children;
    auto config = this->config;

    return [tmp, config](LayoutContext* context) {
        auto widget = LayoutFlyout::make(context, tmp[0](context), tmp[1](context));
        config.applyTo(widget);
        return widget;
    };
}

Expander::operator Builder() const
{
    auto tmp = children;
    auto config = this->config;

    return [tmp, config](LayoutContext* context) {
        auto widget = LayoutExpander::make(context, tmp[0](context), tmp[1](context));
        config.applyTo(widget);
        return widget;
    };
}

}
