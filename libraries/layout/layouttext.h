#ifndef LAYOUTTEXT_H
#define LAYOUTTEXT_H

#include "layoutcore.h"
#include "texteditbox.h"

class LayoutText : public LayoutBase {
    TextEditBox editBox;
public:
    LayoutText(Private privateTag, LayoutContext* context, std::string text);
    static std::shared_ptr<LayoutText> make(LayoutContext* context, std::string text) { return std::make_shared<LayoutText>(Private{}, context, text); }
    std::string getTypeStr() const override { return "TextEdit"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    EvtRes onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    EvtRes onKey(const PropertyBag& parentProps, const KeyEvt &key) override;
};

#endif // LAYOUTTEXT_H
