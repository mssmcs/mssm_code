#ifndef LAYOUTSCROLL_H
#define LAYOUTSCROLL_H

#include "layoutslider.h"

class LayoutScroll : public LayoutBase {
    LayoutPtr child;
    std::shared_ptr<LayoutSlider> vScroll;
    std::shared_ptr<LayoutSlider> hScroll;
    int extraX{0};
    int extraY{0};
    int xScroll{0};
    int yScroll{0};
    int barSize{10};
    RectI contentRect;
public:
    LayoutScroll(Private privateTag, LayoutContext* context, LayoutPtr child);
    static std::shared_ptr<LayoutScroll> make(LayoutContext* context, LayoutPtr child) { return std::make_shared<LayoutScroll>(Private{}, context, child); }
    std::string getTypeStr() const override { return "Scroll"; }
    EvtRes onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    EvtRes onMouseDeferred(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void (LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
};

#endif // LAYOUTSCROLL_H
