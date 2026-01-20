#ifndef LAYOUTTOGGLE_H
#define LAYOUTTOGGLE_H

#include "layoutcore.h"

class LayoutToggle : public LayoutBase {
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
    void resize(const PropertyBag &parentProps, const RectI &rect) override;
    void foreachChildImpl(std::function<void(LayoutBase *)> f,
                                        ForeachContext context,
                                        bool includeOverlay,
                                        bool includeCollapsed) override;
};

#endif // LAYOUTTOGGLE_H
