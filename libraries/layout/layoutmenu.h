#ifndef LAYOUTMENU_H
#define LAYOUTMENU_H

#include "layoutcore.h"
#include "layoutadapter.h"
#include "layoutbutton.h"

class LayoutMenuItem : public LayoutAdapter {
protected:
    bool isHorizontal{true};
    LayoutButton* button;
public:
    LayoutMenuItem(Private privateTag, LayoutContext *context, LayoutPtr child, LayoutButton* button, bool isHorizontal);
    static std::shared_ptr<LayoutMenuItem> make(LayoutContext* context, LayoutPtr child, LayoutButton* button, bool isHorizontal) { return std::make_shared<LayoutMenuItem>(Private{}, context, child, button, isHorizontal); }
    std::string getTypeStr() const override { return "LayoutMenuItem"; }

    SizeBound2d getBound(const PropertyBag &parentProps) override;
    void resize(const PropertyBag &parentProps, const RectI &rect) override;
};

class LayoutMenu : public LayoutBase
{
    class MenuButtonSet : public ButtonSet {
        LayoutMenu* host;
    public:
        MenuButtonSet(LayoutMenu* host) : ButtonSet{true}, host{host} {}
        void onButtonPress(const PropertyBag &parentProps, LayoutButtonPtr button, int buttonIndex, bool checked) override;
    };

    LayoutPtr tabBar;
    MenuButtonSet buttonSet;
    std::vector<LayoutPtr> menus;  // OVERLAYS NOT CHILDREN
    int openedMenuIdx{-1};
    bool isHorizontal{true};
    Margins margins;
public:
    LayoutMenu(Private privateTag, LayoutContext* context, bool isHorizontal, std::vector<LayoutPtr> children);
    static std::shared_ptr<LayoutMenu> make(LayoutContext* context, bool isHorizontal, std::vector<LayoutPtr> children) { return std::make_shared<LayoutMenu>(Private{}, context, isHorizontal, children); }
    std::string getTypeStr() const override { return "Menu"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void setOuterMargins(int left, int right, int top, int bottom) override;
    void foreachChildImpl(std::function<void (LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    void openMenu(const PropertyBag &parentProps, LayoutButtonPtr button, int buttonIdx);
};


#endif // LAYOUTMENU_H
