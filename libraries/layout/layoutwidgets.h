#ifndef LAYOUTWIDGETS_H
#define LAYOUTWIDGETS_H

#include "layoutcore.h"
#include "texteditbox.h"
#include "image.h"

class LayoutLeaf : public LayoutBase {
public:
    LayoutLeaf(LayoutContext* context) : LayoutBase(context) {}
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
};

class LayoutToggle : public LayoutLeaf {
public:
    enum class ToggleStyle {
        expander,
        checkbox,
        radio
    };
protected:
    ToggleStyle style;
public:
    LayoutToggle(Private privateTag, LayoutContext* context, ToggleStyle style);
    static std::shared_ptr<LayoutToggle> make(LayoutContext* context, ToggleStyle style) { return std::make_shared<LayoutToggle>(Private{}, context, style); }
    std::string getTypeStr() const override { return "Toggle"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
};

class LayoutColor : public LayoutWithContent {
    mssm::Color color;
    SizeBound2d bound;
public:
    LayoutColor(Private privateTag, LayoutContext* context, mssm::Color color, SizeBound2d bound = {200,200,50,50});
    virtual ~LayoutColor();
    static std::shared_ptr<LayoutColor> make(LayoutContext* context, mssm::Color color, SizeBound2d bound) { return std::make_shared<LayoutColor>(Private{}, context, color, bound); }
    static std::shared_ptr<LayoutColor> make(LayoutContext* context, mssm::Color color) { return std::make_shared<LayoutColor>(Private{}, context, color); }
    std::string getTypeStr() const override { return "Color"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    EvtProp onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
};


class LayoutImage : public LayoutBase {
    mssm::Image image;
    SizeBound2d bound;
public:
    LayoutImage(Private privateTag, LayoutContext* context, mssm::Image img);
    virtual ~LayoutImage();
    void updateImage(mssm::Image img);
    static std::shared_ptr<LayoutImage> make(LayoutContext* context, mssm::Image img) { return std::make_shared<LayoutImage>(Private{}, context, img); }
    std::string getTypeStr() const override { return "Image"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    EvtProp onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
};

class LayoutLabel : public LayoutWithContent {
    std::string text;
    FontInfo sizeAndFace;
    HAlign hAlign;
    VAlign vAlign;
    Padding padding{4};
public:
    mssm::Color borderColor{mssm::TRANS};
    mssm::Color backgroundColor{mssm::TRANS};
    mssm::Color textColor{mssm::WHITE};
public:
    LayoutLabel(Private privateTag, LayoutContext* context, std::string text, const FontInfo& sizeAndFace);
    virtual ~LayoutLabel();
    static std::shared_ptr<LayoutLabel> make(LayoutContext* context, std::string text, int fontSize = 20) { return std::make_shared<LayoutLabel>(Private{}, context, text, fontSize); }
    std::string getTypeStr() const override { return "Label"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
};

class LayoutButton;
using LayoutButtonPtr = std::shared_ptr<LayoutButton>;



class ButtonSet {
public:
    std::vector<LayoutButtonPtr> buttons;
    bool isRadio{false};
public:
    ButtonSet(bool isRadio) : isRadio{isRadio} {}
    virtual void onButtonPress(const PropertyBag &parentProps, LayoutButtonPtr button, int buttonIndex, bool checked);
    void operator()(const PropertyBag &parentProps, LayoutButtonPtr button, int buttonIndex, bool checked) { onButtonPress(parentProps, button, buttonIndex, checked); }
};

class HoverTrack {
    enum class DrawMode {
        normal,
        hover,
        pressing,
    };
    DrawMode mode;
    void onMouse(LayoutBase *element, const MouseEvt &evt, bool captureOnPress);
public:
    operator DrawMode() const { return mode; }
};


class LayoutAdapter : public LayoutBase {
protected:
    LayoutPtr child;
public:
    LayoutAdapter(Private privateTag, LayoutContext *context, LayoutPtr child);
    std::string getTypeStr() const override { return "Adapter"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase *)> f,
                      ForeachContext context,
                      bool includeOverlay,
                      bool includeCollapsed) override;
};

class LayoutModalHolder : public LayoutBase {
protected:
    LayoutPtr child;
    LayoutPtr modal;
public:
    LayoutModalHolder(Private privateTag, LayoutContext *context, LayoutPtr child, LayoutPtr modal);
    static std::shared_ptr<LayoutModalHolder> make(LayoutContext* context, LayoutPtr child, LayoutPtr modal) { return std::make_shared<LayoutModalHolder>(Private{}, context, child, modal); }
    std::string getTypeStr() const override { return "ModalHolder"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase *)> f,
                      ForeachContext context,
                      bool includeOverlay,
                      bool includeCollapsed) override;
    void setModal(LayoutPtr modal);
    void clearModal();
};

class LayoutAdapterPadded : public LayoutAdapter {
protected:
    Padding padding;
public:
    LayoutAdapterPadded(Private privateTag, LayoutContext *context, LayoutPtr child, Padding padding);
    static std::shared_ptr<LayoutAdapterPadded> make(LayoutContext* context, LayoutPtr child, Padding padding) { return std::make_shared<LayoutAdapterPadded>(Private{}, context, child, padding); }
    std::string getTypeStr() const override { return "AdapterPadded"; }
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
};

class LayoutAdapterClickable : public LayoutAdapterPadded {
protected:
    HoverTrack hoverTrack;
    std::function<void()> clickCallback;
public:
    LayoutAdapterClickable(Private privateTag, LayoutContext *context, LayoutPtr child, Padding padding, std::function<void()> clickCallback);
    static std::shared_ptr<LayoutAdapterClickable> make(LayoutContext* context, LayoutPtr child, Padding padding, std::function<void()> clickCallback) { return std::make_shared<LayoutAdapterClickable>(Private{}, context, child, padding, clickCallback); }
    std::string getTypeStr() const override { return "AdapterClickable"; }
    EvtProp onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
};

// intended for use as overlay, just consumes mouse/keyboard events
// so they don't reach parent
class LayoutModalFrame : public LayoutAdapter {
public:
    LayoutModalFrame(Private privateTag, LayoutContext *context, LayoutPtr child);
    static std::shared_ptr<LayoutModalFrame> make(LayoutContext* context, LayoutPtr child) { return std::make_shared<LayoutModalFrame>(Private{}, context, child); }
    std::string getTypeStr() const override { return "LayoutModalFrame"; }

    // EvtProp onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    // EvtProp onMouseDeferred(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    // EvtProp onKey(const PropertyBag &parentProps, const KeyEvt &evt) override;
    // EvtProp onKeyDeferred(const PropertyBag &parentProps, const KeyEvt &evt) override;
};

class LayoutButtonBase : public LayoutBase {
public:
    enum class ButtonType {
        normal,
        checkbox,
        radio
    };
    enum class DrawMode {
        normal,
        hover,
        pressing,
    };
protected:
    ButtonType type{ButtonType::normal};
    DrawMode mode{DrawMode::normal};
    bool checked{false};  // only valid if checkbox or radio
public:
    LayoutButtonBase(Private privateTag, LayoutContext* context, ButtonType buttonType = ButtonType::normal);
    std::string getTypeStr() const override { return "ButtonBase"; }
    virtual void onButtonPress(const PropertyBag& parentProps, bool pressValue);
    virtual void setChecked(bool checked);
    bool isChecked() const { return checked; }
    EvtProp onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
};


class LayoutButton : public LayoutButtonBase {
public:
    enum class DrawStyle {
        button,
        tabTop,
        tabBottom,
        tabLeft,
        tabRight,
        menu
    };

    using ButtonCallback = std::function<void(const PropertyBag& parentProps, LayoutButtonPtr button, int buttonId, bool checked)>;
    using SimpleButtonCallback = std::function<void(int buttonId, bool checked)>;
protected:
    ButtonCallback callback;
    int buttonId{0};
    LayoutPtr child;
public:
    DrawStyle style{DrawStyle::button};
    int roundRadius{6};
public:
    LayoutButton(Private privateTag, LayoutContext* context, LayoutPtr child, ButtonType buttonType = ButtonType::normal, ButtonCallback callback = {}, int buttonId = 0);
    static LayoutButtonPtr make(LayoutContext* context, LayoutPtr child, int buttonId = 0, ButtonType buttonType = ButtonType::normal, ButtonCallback callback = {})
    { return std::make_shared<LayoutButton>(Private{}, context, child, buttonType, callback, buttonId); }
    static LayoutButtonPtr make(LayoutContext* context, LayoutPtr child, int buttonId, ButtonType buttonType, SimpleButtonCallback callback)
    { return std::make_shared<LayoutButton>(Private{}, context, child, buttonType, [callback](const PropertyBag& parentProps, LayoutButtonPtr button, int buttonId, bool checked) {
            callback(buttonId, checked);
        }, buttonId); }
    std::string getTypeStr() const override { return "Button"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    void onButtonPress(const PropertyBag& parentProps, bool pressValue) override;
    //virtual void setChecked(bool checked);
    void drawCommon(const PropertyBag& parentProps, mssm::Canvas2d& g, mssm::Color color);
    void drawMomentary(const PropertyBag& parentProps, mssm::Canvas2d &g, DrawMode mode);
    void drawCheckbox(const PropertyBag& parentProps, mssm::Canvas2d &g, DrawMode mode);
    void drawRadio(const PropertyBag& parentProps, mssm::Canvas2d &g, DrawMode mode);

    void setCallback(ButtonCallback callback) { this->callback = callback; }
    void setCallback(SimpleButtonCallback callback) {
        setCallback([callback](const PropertyBag& parentProps, LayoutButtonPtr button, int buttonId, bool checked) {
            callback(buttonId, checked);
        });
    }
    void setId(int id) { buttonId = id; }
    int getId() const { return buttonId; }
    //EvtProp onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
};


class LayoutText : public LayoutWithContent {
    TextEditBox editBox;
public:
    LayoutText(Private privateTag, LayoutContext* context, std::string text);
    static std::shared_ptr<LayoutText> make(LayoutContext* context, std::string text) { return std::make_shared<LayoutText>(Private{}, context, text); }
    std::string getTypeStr() const override { return "TextEdit"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    EvtProp onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    EvtProp onKey(const PropertyBag& parentProps, const KeyEvt &key) override;
};

class LayoutDragHandle : public LayoutBase {
public:
    enum class DragState {
        start,
        dragging,
        end
    };
    using DragCallback = std::function<void(DragState, Vec2d)>;
protected:
    bool hovering = false;
    SizeBound2d bound;
    DragCallback dragCallback;
public:
    LayoutDragHandle(Private privateTag, LayoutContext* context, DragCallback dragCallback);
    static std::shared_ptr<LayoutDragHandle> make(LayoutContext* context, DragCallback dragCallback) { return std::make_shared<LayoutDragHandle>(Private{}, context, dragCallback); }
    std::string getTypeStr() const override { return "DragHandle"; }
    EvtProp onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
};

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
    EvtProp onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
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

class LayoutScroll : public LayoutWithContent {
    LayoutPtr child;
    std::shared_ptr<LayoutSlider> vScroll;
    std::shared_ptr<LayoutSlider> hScroll;
    int extraX{0};
    int extraY{0};
    int xScroll{0};
    int yScroll{0};
    int barSize{10};
public:
    LayoutScroll(Private privateTag, LayoutContext* context, LayoutPtr child);
    static std::shared_ptr<LayoutScroll> make(LayoutContext* context, LayoutPtr child) { return std::make_shared<LayoutScroll>(Private{}, context, child); }
    std::string getTypeStr() const override { return "Scroll"; }
    EvtProp onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    EvtProp onMouseDeferred(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void (LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
};

#endif // LAYOUTWIDGETS_H
