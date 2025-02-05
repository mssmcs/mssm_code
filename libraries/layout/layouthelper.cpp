#include "layouthelper.h"

LayoutHelper::HStack::operator LayoutHelper::Builder() const
{
    auto tmp = children;
    ConfigFunc config = this->config;
    return [tmp, config](LayoutContext* context) {
        std::vector<LayoutPtr> kids;
        for (auto& c : tmp) {
            kids.push_back(c(context));
        }
        auto widget = LayoutStacked::make(context, true, Justify::begin, CrossJustify::stretch, kids);
        if (config) config(widget.get());
        return widget;
    };
}

LayoutHelper::VStack::operator LayoutHelper::Builder() const
{
    auto tmp = children;
    ConfigFunc config = this->config;
    return [tmp, config](LayoutContext* context) {
        std::vector<LayoutPtr> kids;
        for (auto& c : tmp) {
            kids.push_back(c(context));
        }
        auto widget = LayoutStacked::make(context, false, Justify::begin, CrossJustify::stretch, kids);
        if (config) config(widget.get());
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
    ConfigFunc config = this->config;
    return [tmp, config](LayoutContext* context) {
        std::vector<LayoutGridRowPtr> kids;
        for (auto& c : tmp) {
            kids.push_back(c(context));
        }
        auto widget = LayoutGrid::make(context, kids);
        if (config) config(widget.get());
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

HTabs::operator Builder() const
{
    auto tmp = children;
    ConfigFunc config = this->config;

    return [tmp, config](LayoutContext* context) {
        std::vector<LayoutPtr> kids;
        for (auto& c : tmp) {
            kids.push_back(c(context));
        }
        auto widget = LayoutTabs::make(context, true, kids);
        if (config) config(widget.get());
        return widget;
    };
}

VTabs::operator Builder() const
{
    auto tmp = children;
    ConfigFunc config = this->config;

    return [tmp, config](LayoutContext* context) {
        std::vector<LayoutPtr> kids;
        for (auto& c : tmp) {
            kids.push_back(c(context));
        }
        auto widget = LayoutTabs::make(context, false, kids);
        if (config) config(widget.get());
        return widget;
    };
}

HSplit::operator Builder() const
{
    Builder left = this->left;
    Builder right = this->right;
    ConfigFunc config = this->config;
    return [left, right, config](LayoutContext *context) {
        auto widget = LayoutSplitter::make(context, true, left(context), right(context));
        if (config) config(widget.get());
        return widget;
    };
}

VSplit::operator Builder() const
{
    Builder top = this->top;
    Builder bottom = this->bottom;
    ConfigFunc config = this->config;
    return [top, bottom, config](LayoutContext *context) {
        auto widget = LayoutSplitter::make(context, false, top(context), bottom(context));
        if (config) config(widget.get());
        return widget;
    };
}

Button::operator Builder() const
{
    std::string txt = label;
    ConfigFunc config = this->config;
    return [txt, config](LayoutContext *context) {
        auto widget = LayoutButton::make(context, LayoutLabel::make(context, txt));
        if (config) config(widget.get());
        return widget;
    };
}

Slider::operator Builder() const
{
    double isHorizontal = this->isHorizontal;
    double minValue = this->minValue;
    double maxValue = this->maxValue;
    double value = this->value;
    ConfigFunc config = this->config;
    return [isHorizontal, minValue, maxValue, value, config](LayoutContext *context) {
        auto widget = LayoutSlider::make(context, isHorizontal, minValue, maxValue, value);
        if (config) config(widget.get());
        return widget;
    };
}

Scroll::operator Builder() const
{
    Builder child = this->child;
    ConfigFunc config = this->config;
    return [child, config](LayoutContext *context) {
        auto widget = LayoutScroll::make(context, child(context));
        if (config) config(widget.get());
        return widget;
    };
}

Panel::operator Builder() const
{
    mssm::Color color = this->color;
    ConfigFunc config = this->config;
    return [color, config](LayoutContext *context) {
        auto widget = LayoutColor::make(context, color);
        if (config) config(widget.get());
        return widget;
    };
}

HMenu::operator Builder() const
{
    auto tmp = children;
    ConfigFunc config = this->config;

    return [tmp, config](LayoutContext* context) {
        std::vector<LayoutPtr> kids;
        for (auto& c : tmp) {
            kids.push_back(c(context));
        }
        auto widget = LayoutMenu::make(context, true, kids);
        if (config) config(widget.get());
        return widget;
    };
}

VMenu::operator Builder() const {
    auto tmp = children;
    ConfigFunc config = this->config;

    return [tmp, config](LayoutContext* context) {
        std::vector<LayoutPtr> kids;
        for (auto& c : tmp) {
            kids.push_back(c(context));
        }
        auto widget = LayoutMenu::make(context, false, kids);
        if (config) config(widget.get());
        return widget;
    };
}

Flyout::operator Builder() const
{
    auto tmp = children;
    ConfigFunc config = this->config;

    return [tmp, config](LayoutContext* context) {
        auto widget = LayoutFlyout::make(context, tmp[0](context), tmp[1](context));
        if (config) config(widget.get());
        return widget;
    };
}

Expander::operator Builder() const
{
    auto tmp = children;
    ConfigFunc config = this->config;

    return [tmp, config](LayoutContext* context) {
        auto widget = LayoutExpander::make(context, tmp[0](context), tmp[1](context));
        if (config) config(widget.get());
        return widget;
    };
}

}
