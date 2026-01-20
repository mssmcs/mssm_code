#ifndef LAYOUTTOOLTIPANCHOR_H
#define LAYOUTTOOLTIPANCHOR_H

#include "layoutadapter.h"

class LayoutTooltipAnchor : public LayoutAdapter {
protected:
    Vec2d offset; // relative offset within owner
public:
    LayoutTooltipAnchor(Private privateTag, LayoutContext *context, LayoutPtr child, Vec2d offset);
    static std::shared_ptr<LayoutTooltipAnchor> make(LayoutContext* context, LayoutPtr child, Vec2d offset) { return std::make_shared<LayoutTooltipAnchor>(Private{}, context, child, offset); }
    std::string getTypeStr() const override { return "LayoutTooltipAnchor"; }
    
    SizeBound2d getBound(const PropertyBag &parentProps) override;
    void resize(const PropertyBag &parentProps, const RectI &rect) override;
};

#endif // LAYOUTTOOLTIPANCHOR_H
