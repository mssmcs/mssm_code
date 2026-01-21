#include "layouttabs.h"
#include "layoutcontainers.h"
#include "rectdraw.h"

LayoutTabs::LayoutTabs(Private privateTag,
                       LayoutContext *context,
                       bool isHorizontal,
                       std::vector<Tab> children)
    : LayoutBase{context}
    , buttonSet{true}
    , isHorizontal{isHorizontal}
{
    int tabIdx{0};

    buttonSet.setCallback([this](int buttonIdx, bool checked) { selectTab(buttonIdx); } );

    std::vector<LayoutPtr> buttons;

    for (auto &c : children) {
        auto frame = LayoutTabFrame::make(context, c.content);
        tabs.push_back(frame);
        frame->setParent(this);

        auto button = LayoutButton::make(context,
                                         LayoutLabel::make(context, c.label),
                                         LayoutButton::ButtonType::radio,
                                         &buttonSet
                                         );

        //button->setCallback(buttonSet.createCallback(button));

        button->style = isHorizontal ? LayoutButton::DrawStyle::tabTop : LayoutButton::DrawStyle::tabLeft;

        if (tabIdx == 0) {
            button->setChecked(true);
        }

        buttons.push_back(button);

        tabIdx++;
    }

    tabBar = LayoutStacked::make(context,
                                 isHorizontal,
                                 Justify::begin,
                                 CrossJustify::stretch,
                                 buttons);

    tabBar->setInnerMargins(0,0);

    for (int i = 0; i < tabs.size(); i++) {
        tabs[i]->setCollapsed(i != activeTab);
    }

    setParentsOfChildren();
}

void LayoutTabs::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    tabBar->draw(parentProps, g);
    tabs[activeTab]->draw(parentProps, g);
}

void LayoutTabs::resize(const PropertyBag& parentProps, const RectI& rect)
{
    headerSize = isHorizontal ? tabBar->getBound(parentProps).yBound.minSize
                              : tabBar->getBound(parentProps).xBound.minSize;
    auto [header, content] = isHorizontal ? ::splitTop(rect, headerSize)
                                          : ::splitLeft(rect, headerSize);
    tabBar->resize(parentProps, header);
    tabs[activeTab]->resize(parentProps, content);
    setRect(rect);
}

SizeBound2d LayoutTabs::getBound(const PropertyBag& parentProps)
{
    headerSize = isHorizontal ? tabBar->getBound(parentProps).yBound.minSize
                              : tabBar->getBound(parentProps).xBound.minSize;
    SizeBound2d childBound = tabs[0]->getBound(parentProps);
    for (int i = 1; i < tabs.size(); i++) {
        childBound = constraintIntersectionSafe(childBound, tabs[i]->getBound(parentProps));
    }
    if (isHorizontal) {
        //childBound.yBound = constraintIntersectionSafe(childBound.yBound, tabBar->getBound(parentProps).yBound);
        childBound.grow(0, headerSize);
    } else {
        //childBound.xBound = constraintIntersectionSafe(childBound.xBound, tabBar->getBound(parentProps).xBound);
        childBound.grow(headerSize, 0);
    }
    grow(childBound, margins, isHorizontal ? 0 : 1, isHorizontal ? 1 : 0);
    return childBound;
}

void LayoutTabs::setOuterMargins(int left, int right, int top, int bottom)
{
    margins.left = left;
    margins.right = right;
    margins.top = top;
    margins.bottom = bottom;
}

void LayoutTabs::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    f(tabBar.get());
    if (includeCollapsed) {
        for (auto &t : tabs) {
            f(t.get());
        }
    }
    else {
        f(tabs[activeTab].get());
    }
}

void LayoutTabs::selectTab(int tabIndex)
{
    if (activeTab == tabIndex) {
        return;
    }
    tabs[activeTab]->closeOverlayRecursive();
    tabs[activeTab]->setCollapsed(true);
    activeTab = tabIndex;
    tabs[activeTab]->setCollapsed(false);
    context->setNeedsResize();
}



LayoutTabFrame::LayoutTabFrame(Private privateTag, LayoutContext *context, const LayoutPtr &child) : LayoutBase(context),
    child(child)
{
    setParentsOfChildren();
}

void LayoutTabFrame::draw(const PropertyBag& parentProps, mssm::Canvas2d &g)
{
    drawRect(g, thisRect(), mssm::TRANS, mssm::rgb(101, 101, 101));
    pushClip(g, contentRect, false);
    child->draw(parentProps, g);
    popClip(g);
}

SizeBound2d LayoutTabFrame::getBound(const PropertyBag& parentProps)
{
    auto bound = child->getBound(parentProps);
    bound.grow(frameInset,frameInset);
    return bound;
}

void LayoutTabFrame::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
    contentRect = rect.makeInset(frameInset);
    child->resize(parentProps, contentRect);
}

void LayoutTabFrame::foreachChildImpl(std::function<void (LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    if (child) {
        f(child.get());
    }
}


