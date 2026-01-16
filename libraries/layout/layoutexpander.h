#ifndef LAYOUTEXPANDER_H
#define LAYOUTEXPANDER_H

#include "layoutcore.h"
#include "layoutwidgets.h"

class LayoutExpander : public LayoutBase
{
    LayoutPtr header;
    LayoutPtr content;
    bool expanded{false};
    bool isHorizontal{true};
public:
    LayoutExpander(Private privateTag, LayoutContext *context, LayoutPtr header, LayoutPtr content);
    static std::shared_ptr<LayoutExpander> make(LayoutContext* context, LayoutPtr header, LayoutPtr content) { return std::make_shared<LayoutExpander>(Private{}, context, header, content); }
    std::string getTypeStr() const override { return "Expander"; }
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    virtual void onClickHeader();
    // void showFlyout(bool show);

//    EvtProp onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    void foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
};


#endif // LAYOUTEXPANDER_H
