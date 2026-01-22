
#include "layoutdraghandle.h"
#include "rectdraw.h"

using namespace mssm;

LayoutDragHandle::LayoutDragHandle(Private privateTag, LayoutContext *context, std::function<void(DragState, Vec2d)> dragCallback)
    : LayoutBase{context}, dragCallback{dragCallback}
{
}

LayoutBase::EvtRes LayoutDragHandle::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    const int dragThreshold = 10;
    
    switch (evt.action) {
    case MouseEvt::Action::exitOverlayParent:
        break;
    case MouseEvt::Action::none:
    case MouseEvt::Action::scroll:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::move:
        hovering = true;
        break;
    case MouseEvt::Action::drag:
        hovering = true;
        if (evt.dragMax > dragThreshold) {
            dragCallback(DragState::dragging, evt.dragDelta);
        }
        break;
    case MouseEvt::Action::press:
        hovering = true;
        dragCallback(DragState::start, evt.dragDelta);
        grabMouse();
        break;
    case MouseEvt::Action::release:
        if (!evt.insideElement) {
            hovering = false;
        }
        if (hasDragFocus()) {
            dragCallback(DragState::end, evt.dragDelta);
            releaseMouse();
        }
        break;
    case MouseEvt::Action::exit:
        if (!hasDragFocus()) {
            hovering = false;
        }
        break;
    }
    
    return EvtRes::propagate;
}

void LayoutDragHandle::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    drawRect(g, thisRect(), WHITE, BLUE);
}

SizeBound2d LayoutDragHandle::getBound(const PropertyBag& parentProps)
{
    return bound;
}

void LayoutDragHandle::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
}

void LayoutDragHandle::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}
