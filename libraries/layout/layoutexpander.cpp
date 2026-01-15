#include "layoutexpander.h"
#include "layoutcontainers.h"

LayoutExpander::LayoutExpander(Private privateTag, LayoutContext *context, LayoutPtr headerContent, LayoutPtr content)
    : LayoutBase{context}, content{content}
{
    auto toggle = LayoutToggle::make(context, LayoutToggle::ToggleStyle::expander);
    auto stack = LayoutStacked::make(context, true, Justify::begin, CrossJustify::center, { toggle, headerContent });
    header = LayoutAdapterClickable::make(context, stack, {4}, [this]() { onClickHeader(); });
    setParentsOfChildren();
}

SizeBound2d LayoutExpander::getBound(const PropertyBag& parentProps)
{
    COWPropertyBag childProps{parentProps};
    childProps.setProperty("Checked", expanded);

    if (expanded) {
        auto buttonBound = header->getBound(childProps);
        auto contentBound = content->getBound(childProps);
        auto bound = vStack(buttonBound, contentBound);
        return bound;
    }
    else {
        return header->getBound(childProps);
    }
}

void LayoutExpander::onClickHeader()
{
    expanded = !expanded;
    context->setNeedsResize();
}

void LayoutExpander::resize(const PropertyBag& parentProps, const RectI& rect)
{
    COWPropertyBag childProps{parentProps};
    childProps.setProperty("Checked", expanded);

    if (expanded) {
        auto buttonBound = header->getBound(childProps);
        auto contentRect = rect;
        auto buttonRect = contentRect.splitTop(buttonBound.yBound.minSize);
        header->resize(childProps, buttonRect);
        content->resize(childProps, contentRect);
    }
    else {
        header->resize(childProps, rect);
    }
    setRect(rect);
}

void LayoutExpander::draw(const PropertyBag& parentProps, mssm::Canvas2d &g)
{
    COWPropertyBag childProps{parentProps};
    childProps.setProperty("Checked", expanded);

    if (expanded) {
        header->draw(childProps, g);
        content->draw(childProps, g);
    }
    else {
        header->draw(childProps, g);
    }
}

void LayoutExpander::foreachChild(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    f(header.get());
    if (includeCollapsed || expanded) {
        f(content.get());
    }
    if (includeOverlay && overlayElement) {
        f(overlayElement.get());
    }
}
