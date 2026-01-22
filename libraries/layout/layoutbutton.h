#ifndef LAYOUTBUTTON_H
#define LAYOUTBUTTON_H

#include "layoutcore.h"

class LayoutButton;
using LayoutButtonPtr = std::shared_ptr<LayoutButton>;
class LayoutButtonBase;

class ButtonSet2 {
private:
    class ButtonInfo {
    public:
        LayoutButtonBase* button;
        double autoClickTime;  // if non zero, hovering over the button
                              // for this time will "click" it, assuming
                              // someone calls void checkClickOnHover(const PropertyBag &parentProps, double hoverTime);
    };
    std::vector<ButtonInfo> buttons;
    bool isRadio;
    std::function<void(int buttonId, bool checked)> onPress{};
public:
    ButtonSet2(bool isRadio, std::function<void(int buttonId, bool checked)> onPress = {});
    void setCallback(std::function<void(int buttonId, bool checked)> onPress);
    LayoutButtonBase *add(LayoutButtonBase* button, double autoClickTime = 0.0);
    void remove(LayoutButtonBase* button);
    std::function<void(const PropertyBag& parentProps, LayoutButtonPtr button, int buttonId, bool checked)> createCallback();
    virtual void onButtonPress(const PropertyBag& parentProps, LayoutButtonBase* button, bool pressValue);
    void checkClickOnHover(const PropertyBag &parentProps);
protected:
    virtual void onButtonPress(const PropertyBag& parentProps, LayoutButtonBase* button, int buttonIndex, bool pressValue);
};

class LayoutButtonBase : public LayoutBase {
public:
    enum class ButtonType {
        normal,
        checkbox,
        radio
    };
    typedef HoverTrack::DrawMode DrawMode;
protected:
    HoverTrack hoverTrack;
    ButtonType type{ButtonType::normal};
    DrawMode mode{DrawMode::normal};
    bool checked{false};  // only valid if checkbox or radio
    ButtonSet2* buttonSet2;
public:
    LayoutButtonBase(Private privateTag, LayoutContext* context, ButtonType buttonType = ButtonType::normal, ButtonSet2* buttonSet = nullptr, double autoPressHoverTime = 0.0);
    virtual ~LayoutButtonBase();
    std::string getTypeStr() const override { return "ButtonBase"; }
    virtual void onButtonPress(const PropertyBag& parentProps, bool pressValue);
    virtual void setChecked(bool checked);
    bool isChecked() const { return checked; }
    EvtRes onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
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
    LayoutButton(Private privateTag, LayoutContext* context, LayoutPtr child, ButtonType buttonType, ButtonSet2* buttonSet, double autoPressHoverTime = 0.0);

    static LayoutButtonPtr make(LayoutContext* context, LayoutPtr child, int buttonId = 0, ButtonType buttonType = ButtonType::normal, ButtonCallback callback = {})
    { return std::make_shared<LayoutButton>(Private{}, context, child, buttonType, callback, buttonId); }

    static LayoutButtonPtr make(LayoutContext* context, LayoutPtr child, int buttonId, ButtonType buttonType, SimpleButtonCallback callback)
    { return std::make_shared<LayoutButton>(Private{}, context, child, buttonType, [callback](const PropertyBag& parentProps, LayoutButtonPtr button, int buttonId, bool checked) {
            callback(buttonId, checked);
        }, buttonId); }

    static LayoutButtonPtr make(LayoutContext* context, LayoutPtr child, ButtonType buttonType, ButtonSet2* buttonSet, double autoPressHoverTime = 0.0)
    {
        return std::make_shared<LayoutButton>(Private{}, context, child, buttonType, buttonSet, autoPressHoverTime);
    }

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

// class ButtonSet {
// public:
//     std::vector<LayoutButtonPtr> buttons;
//     bool isRadio{false};
// public:
//     ButtonSet(bool isRadio) : isRadio{isRadio} {}
//     virtual void onButtonPress(const PropertyBag &parentProps, LayoutButtonPtr button, int buttonIndex, bool checked);
//     void operator()(const PropertyBag &parentProps, LayoutButtonPtr button, int buttonIndex, bool checked) { onButtonPress(parentProps, button, buttonIndex, checked); }
// };

#endif // LAYOUTBUTTON_H
