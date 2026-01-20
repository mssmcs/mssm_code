#ifndef LAYOUTSLIDER_H
#define LAYOUTSLIDER_H

#include "layoutcore.h"

class LayoutSlider : public LayoutBase {
    RectI handle;
    bool isHorizontal{false};
    bool hovering = false;
public:
    double proportion{0.2}; // proportion of the length of the overall slider taken up by handle
    int minHandleSize{20};  // lower bound on handle size (pixels)
    int handleSize{20};     // size in pixels of the handle
    // int sliderLength{}   // width of control for now
    
    // next three values describe the current and range of possible values
    double value{0};
    double minValue{0};
    double maxValue{100};
    
    double wheelStep{0.1};  // fraction of range to move on mouse wheel
    double clickStep{0.1};  // fraction of range to move on click
private:
    // used during drag operation
    double dragStartValue{};
public:
    LayoutSlider(Private privateTag, LayoutContext* context, bool isHorizontal, double minValue, double maxValue, double value);
    static std::shared_ptr<LayoutSlider> make(LayoutContext* context, bool isHorizontal, double minValue, double maxValue, double value) { return std::make_shared<LayoutSlider>(Private{}, context, isHorizontal, minValue, maxValue, value); }
    std::string getTypeStr() const override { return "Slider"; }
    EvtRes onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    void stepValue(int count);
    void applyWheel(int count);
    constexpr double posMax() const { return isHorizontal ? (thisRect().width-handleSize) : (thisRect().height-handleSize); }
    constexpr double tFromValue(double value) const { return std::clamp((value - minValue)/(maxValue - minValue), 0., 1.); }
    constexpr double tFromPos(double pos) const { return std::clamp(pos/posMax(), 0., 1.); }
    constexpr double posFromT(double t) const { return std::clamp(posMax()*t, 0., posMax()); }
    constexpr double posFromValue(double value) const { return posFromT(tFromValue(value)); }
    constexpr double valueFromT(double t) const { return minValue + (maxValue-minValue)*t; }
    constexpr double valueFromPos(double pos) const { return valueFromT(tFromPos(pos)); }
    constexpr RectI calcHandleRect(int handlePos) const {
        if (isHorizontal) {
            return {pos.x+handlePos, pos.y, handleSize, height};
        }
        return {pos.x, pos.y+handlePos, width, handleSize};
    }
};

#endif // LAYOUTSLIDER_H
