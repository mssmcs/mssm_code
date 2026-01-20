#include "layoutmenu.h"
#include "layoutcontainers.h"
#include "layoutcalcs.h"

LayoutMenuItem::LayoutMenuItem(Private privateTag, LayoutContext *context, LayoutPtr child, LayoutButton *button, bool isHorizontal)
    : LayoutAdapter(privateTag, context, child), button{button}, isHorizontal{isHorizontal}
{
}

SizeBound2d LayoutMenuItem::getBound(const PropertyBag &parentProps)
{
    auto bound = child->getBound(parentProps);
    return bound;
}

void LayoutMenuItem::resize(const PropertyBag &parentProps, const RectI &rect)
{
    auto bound = getBound(parentProps);

    int menuWidth = std::max(bound.xBound.minSize, 20);
    int menuHeight = std::max(bound.yBound.minSize, 20);
    RectI menuRect = {{pos.x, pos.y}, menuWidth, menuHeight};
    menuRect = positionOverlay(context->getWindowRect(), button->thisRect(), isHorizontal ? button->thisRect().bottomCenter() : button->thisRect().rightCenter(), menuRect, OverlayStyle::menu);

    child->resize(parentProps, menuRect);
    setRect(*child);
}

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

        std::string txt = "Menu";

        LayoutButtonPtr menuItemButton = LayoutButton::make(context,
                                                            LayoutLabel::make(context, txt),
                                                            tabIdx, LayoutButton::ButtonType::radio, buttonSet);

        auto menuItem = LayoutMenuItem::make(context, c, menuItemButton.get(), isHorizontal);

        menus.push_back(menuItem);
        menuItem->setParent(this);


        buttonSet.buttons.push_back(menuItemButton);
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

void LayoutMenu::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    // NOTE: don't iterate over menus, they are overlays not children
    f(tabBar.get());
}

void LayoutMenu::openMenu(const PropertyBag& parentProps, LayoutButtonPtr button, int buttonIdx)
{
    openedMenuIdx = buttonIdx;

    auto& menu = menus[buttonIdx];

    // auto bound = menu->getBound(parentProps);
    // int menuWidth = std::max(bound.xBound.minSize, 20);
    // int menuHeight = std::max(bound.yBound.minSize, 20);
    // RectI menuRect = {{pos.x, pos.y}, menuWidth, menuHeight};
    // menuRect = positionOverlay(context->getWindowRect(), button->thisRect(),isHorizontal ? button->thisRect().bottomCenter() : button->thisRect().rightCenter(), menuRect, OverlayStyle::menu);
    // menu->resize(parentProps, menuRect);
    setOverlay(menu);
}

void LayoutMenu::MenuButtonSet::onButtonPress(const PropertyBag& parentProps, LayoutButtonPtr button, int buttonIndex, bool checked)
{
    ButtonSet::onButtonPress(parentProps, button, buttonIndex, checked);
    host->openMenu(parentProps, button, buttonIndex);
}
