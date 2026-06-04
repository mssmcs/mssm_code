#ifndef LAYOUTSCROLLBAR_H
#define LAYOUTSCROLLBAR_H

#include "layoutcore.h"

class LayoutScrollBar : public LayoutBase {
    bool isHorizontal{false};
    bool hovering = false;
public:
    double proportion{0.2};
    int minHandleSize{20};
    int handleSize{20};

    double scrollOffset{0};
    double scrollMax{0};

    double wheelStep{0.1};
private:
    double dragStartOffset{};
public:
    LayoutScrollBar(Private privateTag, LayoutContext* context, bool isHorizontal);
    static std::shared_ptr<LayoutScrollBar> make(LayoutContext* context, bool isHorizontal)
    {
        return std::make_shared<LayoutScrollBar>(Private{}, context, isHorizontal);
    }
    std::string getTypeStr() const override { return "ScrollBar"; }
    EvtRes onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    void applyWheel(int count);
    constexpr double posMax() const { return isHorizontal ? (thisRect().width - handleSize) : (thisRect().height - handleSize); }
    constexpr double tFromOffset(double offset) const { return scrollMax > 0 ? std::clamp(offset / scrollMax, 0., 1.) : 0.; }
    constexpr double tFromPos(double pos) const { return std::clamp(pos / posMax(), 0., 1.); }
    constexpr double posFromT(double t) const { return std::clamp(posMax() * t, 0., posMax()); }
    constexpr double posFromOffset(double offset) const { return posFromT(tFromOffset(offset)); }
    constexpr double offsetFromT(double t) const { return scrollMax * t; }
    constexpr double offsetFromPos(double pos) const { return offsetFromT(tFromPos(pos)); }
    constexpr RectI calcHandleRect(int handlePos) const
    {
        if (isHorizontal) {
            return {pos.x + handlePos, pos.y, handleSize, height};
        }
        return {pos.x, pos.y + handlePos, width, handleSize};
    }
};

#endif // LAYOUTSCROLLBAR_H
