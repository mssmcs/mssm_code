#ifndef LAYOUTDRAGHANDLE_H
#define LAYOUTDRAGHANDLE_H

#include "layoutcore.h"

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
    EvtRes onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
};

#endif // LAYOUTDRAGHANDLE_H
