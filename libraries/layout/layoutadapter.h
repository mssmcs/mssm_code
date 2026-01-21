#ifndef LAYOUTADAPTER_H
#define LAYOUTADAPTER_H

#include "layoutcore.h"

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
    EvtRes onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
};


#endif // LAYOUTADAPTER_H
