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
                       std::vector<Item> children)
    : LayoutBase{context}
    , buttonSet{true}
    , isHorizontal{isHorizontal}
{
    int tabIdx{0};

    std::vector<LayoutPtr> buttons;

    LayoutButton::SimpleButtonCallback onTabClick = [this](int buttonIdx, bool checked) {
        openMenu(buttonIdx);
    };

    buttonSet.setCallback(onTabClick);

    for (auto &c : children) {

        std::string txt = c.label;

        LayoutButtonPtr menuItemButton = LayoutButton::make(context,
                                                            LayoutLabel::make(context, txt),
                                                            LayoutButton::ButtonType::radio,
                                                            &buttonSet, c.content ? 0.2 : 0.0);


      //  menuItemButton->setCallback(buttonSet.createCallback(menuItemButton));

        menuItemButton->style  = LayoutButton::DrawStyle::menu;

        Item item{c.label, {}, c.callback};


        if (c.content) {
            item.content = LayoutMenuItem::make(context, c.content, menuItemButton.get(), isHorizontal);
            item.content->setParent(this);
        }

        items.push_back(item);

        buttons.push_back(menuItemButton);

        tabIdx++;
    }

    tabBar = LayoutStacked::make(context,
                                 isHorizontal,
                                 Justify::begin,
                                 CrossJustify::stretch,
                                 buttons);

    tabBar->setInnerMargins(0,0);

    setParentsOfChildren();

    broadcastRecursive(BroadcastMessage::isUnderMenu, false, true);
    for (int i = 0; i < items.size(); i++) {
        if (items[i].content) {
            items[i].content->broadcastRecursive(BroadcastMessage::isUnderMenu, true, true);
        }
    }
}

void LayoutMenu::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    tabBar->draw(parentProps, g);
    buttonSet.checkClickOnHover(parentProps);
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

void LayoutMenu::openMenu(int buttonIdx)
{
    if (!hasOverlay() || buttonIdx != openedMenuIdx) {

        openedMenuIdx = buttonIdx;

        auto& item = items[buttonIdx];

        // auto bound = menu->getBound(parentProps);
        // int menuWidth = std::max(bound.xBound.minSize, 20);
        // int menuHeight = std::max(bound.yBound.minSize, 20);
        // RectI menuRect = {{pos.x, pos.y}, menuWidth, menuHeight};
        // menuRect = positionOverlay(context->getWindowRect(), button->thisRect(),isHorizontal ? button->thisRect().bottomCenter() : button->thisRect().rightCenter(), menuRect, OverlayStyle::menu);
        // menu->resize(parentProps, menuRect);

        if (item.content) {
            setOverlay(item.content);
        }

        if (item.callback) {
            item.callback(item.label, buttonIdx);
            bubbleMessage(BubbleMessage::closeMenu, true);
        }
    }
}

void LayoutMenu::onBroadcast(BroadcastMessage message)
{
    isSubMenu = true;
}

bool LayoutMenu::onBubbleMessage(BubbleMessage message)
{
    if (message == BubbleMessage::closeMenu) {
        closeOverlay();
    }
    return isSubMenu;
}

// void LayoutMenu::MenuButtonSet::onButtonPress(const PropertyBag& parentProps, LayoutButtonPtr button, int buttonIndex, bool checked)
// {
//     ButtonSet2::onButtonPress(parentProps, button, buttonIndex, checked);
//     host->openMenu(parentProps, button, buttonIndex);
// }


LayoutBase::EvtRes LayoutMenuItem::onMouse(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    switch (evt.action) {
    case MouseEvt::Action::none:
        break;
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::exit:
        break;
    case MouseEvt::Action::move:
        std::cout << "Hmm" << std::endl;
        break;
    case MouseEvt::Action::drag:
        break;
    case MouseEvt::Action::press:
        break;
    case MouseEvt::Action::release:
        break;
    case MouseEvt::Action::scroll:
        break;
    case MouseEvt::Action::exitOverlayParent:
        break;

    }

    return EvtRes::propagate;
}




