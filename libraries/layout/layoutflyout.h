#ifndef LAYOUTFLYOUT_H
#define LAYOUTFLYOUT_H

#include "layoutcore.h"
#include "layoutwidgets.h"

class LayoutFlyout : public LayoutButton
{
    LayoutPtr content;
public:
    bool isHorizontal{true};
public:
    LayoutFlyout(Private privateTag, LayoutContext *context, LayoutPtr buttonContent, LayoutPtr flyoutContent);
    static std::shared_ptr<LayoutFlyout> make(LayoutContext* context, LayoutPtr buttonContent, LayoutPtr flyoutContent) { return std::make_shared<LayoutFlyout>(Private{}, context, buttonContent, flyoutContent); }
    std::string getTypeStr() const override { return "Flyout"; }
    //void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void showFlyout(const PropertyBag &parentProps, bool show);
    void onButtonPress(const PropertyBag &parentProps, bool buttonValue) override;
};


#endif // LAYOUTFLYOUT_H
