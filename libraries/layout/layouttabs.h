#ifndef LAYOUTTABS_H
#define LAYOUTTABS_H

#include "layoutcore.h"
#include "layoutwidgets.h"

class LayoutTabFrame : public LayoutBase {
    LayoutPtr child;
    int frameInset{3};
    RectI contentRect;
public:
    LayoutTabFrame(Private privateTag, LayoutContext *context, const LayoutPtr &child);
    static std::shared_ptr<LayoutTabFrame> make(LayoutContext* context, LayoutPtr child) { return std::make_shared<LayoutTabFrame>(Private{}, context, child); }
    std::string getTypeStr() const override { return "TabFrame"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void (LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
};

class LayoutTabs : public LayoutBase
{
    class TabButtonSet : public ButtonSet {
        LayoutTabs* host;
    public:
        TabButtonSet(LayoutTabs* host) : ButtonSet{true}, host{host} {}
        void onButtonPress(const PropertyBag& parentProps, LayoutButtonPtr button, int buttonIndex, bool checked) override;
    };

    LayoutPtr tabBar;
    TabButtonSet buttonSet;
    std::vector<LayoutPtr> tabs;
    bool isHorizontal{true};
    int activeTab{0};
    int headerSize{30};
    Margins margins;
public:
    LayoutTabs(Private privateTag, LayoutContext* context, bool isHorizontal, std::vector<LayoutPtr> children);
    static std::shared_ptr<LayoutTabs> make(LayoutContext* context, bool isHorizontal, std::vector<LayoutPtr> children) { return std::make_shared<LayoutTabs>(Private{}, context, isHorizontal, children); }
    std::string getTypeStr() const override { return "Tabs"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void setOuterMargins(int left, int right, int top, int bottom) override;
    void foreachChildImpl(std::function<void (LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    void selectTab(int tabIndex);
};



#endif // LAYOUTTABS_H
