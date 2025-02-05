#include "layoutmenu.h"
#include "layoutcontainers.h"
#include "layoutcalcs.h"

LayoutMenu::LayoutMenu(Private privateTag,
                       LayoutContext *context,
                       bool isHorizontal,
                       std::vector<LayoutPtr> children)
    : LayoutBase{context}
    , buttonSet{this}
    , isHorizontal{isHorizontal}
{
    int tabIdx{0};
    for (auto &c : children) {
        menus.push_back(c);
        c->setParent(this);
        std::string txt = "Menu";
        buttonSet.buttons.push_back(LayoutButton::make(context,
                                                       LayoutLabel::make(context, txt),
                                                       LayoutButton::ButtonType::radio,
                                                       buttonSet,
                                                       tabIdx));
        buttonSet.buttons.back()->style = LayoutButton::DrawStyle::menu;
        tabIdx++;
    }
    std::vector<LayoutPtr> buttons;
    for (auto &b : buttonSet.buttons) {
        buttons.push_back(b);
    }
    tabBar = LayoutStacked::make(context,
                                 isHorizontal,
                                 Justify::begin,
                                 CrossJustify::stretch,
                                 buttons);

    tabBar->setInnerMargins(0,0);

    setParentsOfChildren();
}

void LayoutMenu::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    tabBar->draw(parentProps, g);
}

void LayoutMenu::resize(const PropertyBag& parentProps, const RectI& rect)
{
    tabBar->resize(parentProps, rect);
    setRect(rect);
}

SizeBound2d LayoutMenu::getBound(const PropertyBag& parentProps)
{
    return tabBar->getBound(parentProps);
}

void LayoutMenu::setOuterMargins(int left, int right, int top, int bottom)
{
    margins.left = left;
    margins.right = right;
    margins.top = top;
    margins.bottom = bottom;
}

void LayoutMenu::foreachChild(std::function<void(LayoutBase *)> f, bool includeOverlay, bool includeCollapsed)
{
    // NOTE: don't iterate over menus, they are overlays not children
    f(tabBar.get());
    if (includeOverlay && overlayElement) {
        f(overlayElement.get());
    }
}

void LayoutMenu::openMenu(const PropertyBag& parentProps, LayoutButtonPtr button, int buttonIdx)
{
    openedMenuIdx = buttonIdx;

    auto& menu = menus[buttonIdx];

    auto bound = menu->getBound(parentProps);
    int menuWidth = std::max(bound.xBound.minSize, 20);
    int menuHeight = std::max(bound.yBound.minSize, 20);
    RectI menuRect = {{pos.x, pos.y}, menuWidth, menuHeight};
    menuRect = positionOverlay(context->getWindowRect(), button->thisRect(),isHorizontal ? button->thisRect().bottomCenter() : button->thisRect().rightCenter(), menuRect, OverlayStyle::menu);
    menu->resize(parentProps, menuRect);
    setOverlay(menu);
}

void LayoutMenu::MenuButtonSet::onButtonPress(const PropertyBag& parentProps, LayoutButtonPtr button, int buttonIndex, bool checked)
{
    ButtonSet::onButtonPress(parentProps, button, buttonIndex, checked);
    host->openMenu(parentProps, button, buttonIndex);
}
