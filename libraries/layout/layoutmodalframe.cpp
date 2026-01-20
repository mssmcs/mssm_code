
#include "layoutmodalframe.h"

LayoutModalFrame::LayoutModalFrame(Private privateTag, LayoutContext *context, LayoutPtr child, int width, int height)
    : LayoutAdapter(privateTag, context, child)
{
    setRect(RectI{{0,0},width,height});
}

LayoutBase::EvtRes LayoutModalFrame::onMouseDeferred(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    return EvtRes::consumed;
}

LayoutBase::EvtRes LayoutModalFrame::onKeyDeferred(const PropertyBag &parentProps, const KeyEvt &evt)
{
    return EvtRes::consumed;
}

SizeBound2d LayoutModalFrame::getBound(const PropertyBag &parentProps)
{
    return SizeBound2d(width, height, width, height);
}

void LayoutModalFrame::resize(const PropertyBag &parentProps, const RectI &rect)
{
    RectI newRect{{0, 0}, width, height};
    setRect(newRect.centered(rect));
    child->resize(parentProps, thisRect());
}

LayoutBase::EvtRes LayoutModalFrame::onMouse(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    switch (evt.action) {
    case MouseEvt::Action::none:
        std::cout << "LayoutModalFrame MouseAction none" << std::endl;
        break;
    case MouseEvt::Action::enter:
        std::cout << "LayoutModalFrame MouseAction enter" << std::endl;
        break;
    case MouseEvt::Action::exit:
        std::cout << "LayoutModalFrame MouseAction exit" << std::endl;
        break;
    case MouseEvt::Action::move:
        std::cout << "LayoutModalFrame MouseAction move" << std::endl;
        break;
    case MouseEvt::Action::drag:
        std::cout << "LayoutModalFrame MouseAction drag" << std::endl;
        break;
    case MouseEvt::Action::release:
        std::cout << "LayoutModalFrame MouseAction release" << std::endl;
        break;
    case MouseEvt::Action::scroll:
        std::cout << "LayoutModalFrame MouseAction scroll" << std::endl;
        break;
    case MouseEvt::Action::press:
        std::cout << "LayoutModalFrame MouseAction press" << std::endl;
        break;
    case MouseEvt::Action::exitOverlayParent:
        std::cout << "LayoutModalFrame MouseAction press" << std::endl;
        break;
    }
    
    return EvtRes::propagate;
}

LayoutBase::EvtRes LayoutModalFrame::onKey(const PropertyBag &parentProps, const KeyEvt &evt)
{
    switch (evt.action) {
    case KeyEvt::Action::press:
        std::cout << "LayoutModalFrame Key Press: " << evt.key << std::endl;
        break;
    case KeyEvt::Action::repeat:
        std::cout << "LayoutModalFrame Key Repeat: " << evt.key << std::endl;
        break;
    case KeyEvt::Action::release:
        std::cout << "LayoutModalFrame Key Release: " << evt.key << std::endl;
        break;
        
    }
    
    return EvtRes::propagate;
}
