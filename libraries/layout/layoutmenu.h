#ifndef LAYOUTMENU_H
#define LAYOUTMENU_H

#include "layoutcore.h"
#include "layoutwidgets.h"

// class LayoutMenuItem : public LayoutBase {
//     std::shared_ptr<LayoutLabel> label;
// public:
//     LayoutMenuItem(Private privateTag, LayoutContext* context);
//     static std::shared_ptr<LayoutMenu> make(LayoutContext* context) { return std::make_shared<LayoutMenuItem>(Private{}, context); }
//     SizeBound2d getBound(const PropertyBag& parentProps) override;
//     void resize(const PropertyBag& parentProps, const RectI& rect) override;
//     void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
//     void foreachChild(std::function<void (LayoutBase *)> f) override;
// };

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
    void foreachChild(std::function<void (LayoutBase *)> f, bool includeOverlay, bool includeCollapsed) override;
    void openMenu(const PropertyBag &parentProps, LayoutButtonPtr button, int buttonIdx);
};


#endif // LAYOUTMENU_H
