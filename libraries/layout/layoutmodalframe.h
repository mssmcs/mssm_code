#ifndef LAYOUTMODALFRAME_H
#define LAYOUTMODALFRAME_H

#include "layoutadapter.h"

// intended for use as overlay, just consumes mouse/keyboard events
// so they don't reach parent
class LayoutModalFrame : public LayoutAdapter {
public:
    LayoutModalFrame(Private privateTag, LayoutContext *context, LayoutPtr child, int width, int height);
    static std::shared_ptr<LayoutModalFrame> make(LayoutContext* context, LayoutPtr child, int width, int height) { return std::make_shared<LayoutModalFrame>(Private{}, context, child, width, height); }
    std::string getTypeStr() const override { return "LayoutModalFrame"; }
    
    // LayoutBase interface
public:
    EvtRes onMouse(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    EvtRes onKey(const PropertyBag &parentProps, const KeyEvt &evt) override;
    EvtRes onMouseDeferred(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    EvtRes onKeyDeferred(const PropertyBag &parentProps, const KeyEvt &evt) override;
    
    SizeBound2d getBound(const PropertyBag &parentProps) override;
    void resize(const PropertyBag &parentProps, const RectI &rect) override;
};

#endif // LAYOUTMODALFRAME_H
