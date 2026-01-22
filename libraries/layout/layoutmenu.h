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

    EvtRes onMouse(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt) override;
};

class LayoutMenu : public LayoutBase
{
public:
    class Item {
    public:
        std::string label;
        LayoutPtr content;
        std::function<void(std::string label, int idx)> callback{};
    };
protected:
    LayoutPtr tabBar;
    ButtonSet2 buttonSet;
    std::vector<Item> items;  // note, content will have wrapper applied
    int openedMenuIdx{-1};
    bool isHorizontal{true};
    Margins margins;
    bool isSubMenu{false};
public:
    LayoutMenu(Private privateTag, LayoutContext* context, bool isHorizontal, std::vector<Item> children);
    static std::shared_ptr<LayoutMenu> make(LayoutContext* context, bool isHorizontal, std::vector<Item> children) { return std::make_shared<LayoutMenu>(Private{}, context, isHorizontal, children); }
    std::string getTypeStr() const override { return "Menu"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void setOuterMargins(int left, int right, int top, int bottom) override;
    void foreachChildImpl(std::function<void (LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    void openMenu(int buttonIdx);

    void onBroadcast(BroadcastMessage message) override;
    bool onBubbleMessage(BubbleMessage message) override;
};


#endif // LAYOUTMENU_H
