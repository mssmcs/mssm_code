#include "layoutcore.h"
#include "layoutcalcs.h"
#include <sstream>


std::string to_string(RectRegion region)
{
    switch (region) {
    case RectRegion::ulCorner: return "ulCorner";
    case RectRegion::urCorner: return "urCorner";
    case RectRegion::lrCorner: return "lrCorner";
    case RectRegion::llCorner: return "llCorner";
    case RectRegion::leftBorder: return "leftBorder";
    case RectRegion::topBorder: return "topBorder";
    case RectRegion::rightBorder: return "rightBorder";
    case RectRegion::bottomBorder: return "bottomBorder";
    case RectRegion::content: return "content";
    case RectRegion::outside: return "outside";
        break;
    }
    return "unknown";
}

void LayoutBase::setParent(LayoutBase *newParent)
{
    parent = newParent;
    if (parent) {
        depth = parent->depth+1;
    }
}

void LayoutBase::updateLayer(int newLayer, int newDepth)
{
    layer = newLayer;
    depth = newDepth;
    foreachChild([newLayer, newDepth](LayoutBase* child) {
        child->updateLayer(newLayer, newDepth+1);
    }, ForeachContext::other, false, true);
}

void LayoutBase::onSetKeyFocus()
{
    isKeyFocusElement = true;

    LayoutBase* curr = parent;
    while (curr) {
        curr->containsKeyFocusElement = true;
        curr = curr->parent;
    }
}

void LayoutBase::onClearKeyFocus()
{
    isKeyFocusElement = false;

    LayoutBase* curr = parent;
    while (curr) {
        curr->containsKeyFocusElement = false;
        curr = curr->parent;
    }
}

void LayoutBase::onSetDragFocus()
{
    isDragFocusElement = true;    
    foreachAncestor([](LayoutBase* ancestor) {
        ancestor->containsDragFocusElement = true;
    });
}

void LayoutBase::onClearDragFocus()
{
    isDragFocusElement = false;
    foreachAncestor([](LayoutBase* ancestor) {
        ancestor->containsDragFocusElement = false;
    });
}

double LayoutBase::distance(Vec2d p)
{
    double xDist{0};
    double yDist{0};

    if (p.x < pos.x) {
        xDist = pos.x - p.x;
    }
    else if (p.x > pos.x + width) {
        xDist = p.x - (pos.x + width);
    }

    if (p.y < pos.y) {
        yDist = pos.y - p.y;
    }
    else if (p.y > pos.y + height) {
        yDist = p.y - (pos.y + height);
    }

    if (xDist == 0 && yDist == 0) {
        return 0;
    }

    if (xDist == 0) {
        return yDist;
    }

    if (yDist == 0) {
        return xDist;
    }

    return std::max(xDist, yDist);
}

std::shared_ptr<LayoutBase> LayoutBase::getLocalRoot()
{
    LayoutBase* curr = this;
    while (curr->depth > 0) {
        curr = curr->parent;
    }
    return curr->getBasePtr();
}

void LayoutBase::foreachChild(std::function<void (LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    foreachChildImpl(f, context, includeOverlay, includeCollapsed);

    if (includeOverlay && overlayElement) {
        f(overlayElement.get());
    }
}

LayoutPtr LayoutBase::findByName(std::string nm)
{
    if (nm == name) {
        return shared_from_this();
    }

    LayoutPtr ptr{};

    this->foreachChild([&ptr,nm](LayoutBase* item) {
        LayoutPtr p = item->findByName(nm);
        if (p) {
            ptr = p;
        }
    }, ForeachContext::other, true, true);

    return ptr;
}



void LayoutBase::traversePreOrder(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    f(this);
    foreachChild([&f, context, includeOverlay, includeCollapsed](LayoutBase *child) { child->traversePreOrder(f, context, includeOverlay, includeCollapsed); }, context, includeOverlay, includeCollapsed);
}

void LayoutBase::traversePostorder(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    foreachChild([&f, context, includeOverlay, includeCollapsed](LayoutBase *child) { child->traversePostorder(f, context, includeOverlay, includeCollapsed); }, context, includeOverlay, includeCollapsed);
    f(this);
}

int LayoutBase::nextId{1};

void LayoutBase::addToolTip(const PropertyBag& parentProps, Vec2d pos, LayoutPtr tip)
{
    showingTooltip = true;
    setOverlay(tip);
}

std::string LayoutBase::trail() const
{
    std::stringstream ss;
    if (depth > 0) {
        ss << parent->trail() << ":" << id << "(" << typeStr << ") ";
    }
    else {
        ss << "Layer " << layer << ": " << id << "(" << typeStr << ") ";
    }
    return ss.str();
}

void LayoutBase::setCollapsed(bool collapsed)
{
    isCollapsed = collapsed;
    foreachChild([collapsed](LayoutBase* child) {
        child->setCollapsed(collapsed);
    }, ForeachContext::other, true, false);
}

void LayoutBase::setParentsOfChildren()
{
    foreachChild([this](LayoutBase* child) {
        child->setParent(this);
    }, ForeachContext::other, false, true);
}

void LayoutBase::setOuterMargins(int borders)
{
    setOuterMargins(borders, borders);
}

void LayoutBase::setOuterMargins(int leftRight, int topBottom)
{
    setOuterMargins(leftRight, leftRight, topBottom, topBottom);
}

void LayoutBase::setOuterMargins(int left, int right, int top, int bottom)
{
    std::cout << "setOuterMargins not supported on this element" << std::endl;
   // throw std::logic_error("setOuterMargins not supported on this element");
}

void LayoutBase::setInnerMargins(int hBetween, int vBetween)
{
    std::cout << "setInnerMargins not supported on this element" << std::endl;
  // throw std::logic_error("setInnerMargins not supported on this element");
}

void LayoutBase::setInnerMargins(int between)
{
    setInnerMargins(between, between);
}

void LayoutBase::grabKeyboard()
{
    context->setKeyboardFocus(shared_from_this());
}

void LayoutBase::releaseKeyboard()
{
    context->setKeyboardFocus({});
}

void LayoutBase::grabMouse()
{
    std::cout << "Grabbing Mouse: " << getTypeStr() << " " << getName() << std::endl;
    context->setDragFocus(shared_from_this());
}

void LayoutBase::releaseMouse()
{
    std::cout << "Releasing Mouse: " << getTypeStr() << " " << getName() << std::endl;
    context->setDragFocus({});
}

void LayoutBase::localReleaseMouseAndKeyboard()  // on subtree
{
    if (isDragFocusElement || containsDragFocusElement) {
        releaseMouse();
    }

    if (isKeyFocusElement || containsKeyFocusElement) {
        releaseKeyboard();
    }
}

void LayoutBase::pushClip(mssm::Canvas2d &g, const RectI &rect, bool replace)
{
    g.pushClip(rect.pos.x, rect.pos.y, rect.width, rect.height, replace);
}

void LayoutBase::popClip(mssm::Canvas2d &g)
{
    g.popClip();
}

LayoutBase::EvtRes LayoutBase::propagateMouse(const PropertyBag& parentProps, EvtProp prop, const RectI &clip, MouseEvt &evt)
{
    // if (evt.action == MouseEvt::Action::press)
    // {
    //     std::cout << "Press: " << getTypeStr() << " " << getName() << std::endl;
    // }

    // if (evt.action == MouseEvt::Action::release)
    // {
    //     std::cout << "Release: " << getTypeStr() << " " << getName() << std::endl;
    // }

    // TODO: hack, just for debugging so we can see type text in debugger more easily
    if (typeStr.empty()) {
        typeStr = getTypeStr();
    }

    if (isCollapsed) {
        return EvtRes::propagate;
    }

    auto newClip = ::intersect(clip, thisRect());

    bool isInside = newClip.within(evt.pos); // cache local since recursive calls may update evt.insideElement

    evt.insideElement = isInside;

    context->updateHoverChain(parentProps, shared_from_this(), isInside, evt.pos);

    MouseEventReason reason{};

    if (prop == EvtProp::hover) {
        if (isInside) {
            reason = MouseEventReason::hoverOnly;
        }
        else {
            return EvtRes::hover;
        }
    }
    else if (isAnyDragFocus()) {
        if (isDragFocusElement) {
            reason = MouseEventReason::isGrab;
        }
        else if (containsDragFocusElement) {
            reason = MouseEventReason::containsGrab;
        }
        else if (isInside) {
            reason = MouseEventReason::hoverOnly;
        }
        else {
            // someone else has drag focus
            // don't pass down to children
            return EvtRes::propagate;
        }
    }
    else if (isInside) {
        // mouse is within this element
        reason = MouseEventReason::within;
    }
    else {
        // mouse pointer not in this element
        // don't pass down to children
        return EvtRes::propagate;
    }

    // if we got here, mouse is either:
    //      within the element
    //      grabbed by the element
    //      grabbed by a child of the element

    // if reason == hoverOnly, we only are tracking enter/exit of the mouse

    EvtRes res = EvtRes::propagate;

    if (reason != MouseEventReason::hoverOnly) {
        res = onMouse(parentProps, reason, evt);
    }

    switch (res) {
    case LayoutBase::EvtRes::consumed:
    case LayoutBase::EvtRes::hover:
        prop = EvtProp::hover;
        break;
    case LayoutBase::EvtRes::propagate:
        break;

    }

    foreachChild([&evt, &newClip, &prop, &parentProps](LayoutBase* child) {
        EvtRes res = child->propagateMouse(parentProps, prop, newClip, evt);
        if (res != EvtRes::propagate) {
            prop = EvtProp::hover;
        }
    }, ForeachContext::events, false, false);

    if (reason != MouseEventReason::hoverOnly) {
        res = onMouseDeferred(parentProps, reason, evt);
    }

    return res;
}

LayoutBase::EvtRes LayoutBase::propagateKey(const PropertyBag& parentProps, EvtProp prop, const RectI &clip, KeyEvt &evt)
{
    auto newClip = ::intersect(clip, thisRect());

    bool isInside = newClip.within(evt.pos); // cache local since recursive calls may update evt.insideElement

    evt.insideElement = isInside;

    bool callOnKey{true};

    if (isAnyKeyboardFocus()) {
        if (isKeyFocusElement) {
            callOnKey = true;
        }
        else if (containsKeyFocusElement) {
            callOnKey = false;
        }
        else {
            return EvtRes::propagate;
        }
    }
    else {
        callOnKey = true;
    }

    EvtRes res{EvtRes::propagate};

    if (callOnKey) {
        res = onKey(parentProps, evt);
    }

    if (res == EvtRes::consumed) {
        return res;
    }

    bool consumed = false;

    foreachChild([&consumed, &evt, &newClip, &parentProps, prop](LayoutBase* child) {
        if (!consumed) {
            auto childClip = ::intersect(newClip, child->thisRect());
            EvtRes res = child->propagateKey(parentProps, prop, childClip, evt);
            if (res == EvtRes::consumed) {
                consumed = true;
            }
        }
    }, ForeachContext::events, false, false);

    if (callOnKey) {
        res = onKeyDeferred(parentProps, evt);
    }

    return res;
}

LayoutBase::EvtRes LayoutBase::onKey(const PropertyBag& parentProps, const KeyEvt &evt)
{
    return EvtRes::propagate;
}

LayoutBase::EvtRes LayoutBase::onKeyDeferred(const PropertyBag& parentProps, const KeyEvt &evt)
{
    return EvtRes::propagate;
}

LayoutBase::EvtRes LayoutBase::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    //context->debugMouse(thisRect(), reason, evt);
    return EvtRes::propagate;
}

LayoutBase::EvtRes LayoutBase::onMouseDeferred(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    return EvtRes::propagate;
}

void LayoutBase::debugDraw(mssm::Canvas2d &g)
{
    // if (this->isDragFocusElement) {
    //     g.ellipse(cast<Vec2d>(center()), 10, 10, mssm::GREEN);
    // }
    // if (this->containsDragFocusElement) {
    //     drawRect(g, thisRect(), mssm::TRANS, mssm::Color{0,255,0,10});
    //     g.ellipse(cast<Vec2d>(center()), 5, 5, mssm::CYAN);
    // }
    foreachChild([&g](LayoutBase* child) {
        child->debugDraw(g);
    }, ForeachContext::drawing, false, false);
}

void LayoutBase::setOverlay(LayoutPtr overlay)
{
    if (overlayElement) {
        if (overlayElement == overlay) {
            return;
        }
        closeOverlay();
    }
    overlayElement = overlay;
    overlay->setParent(this);
    overlay->setCollapsed(false);
    context->pushOverlay(overlay);
}

void LayoutBase::closeOverlay()
{
    if (overlayElement) {
        std::cout << "Closing Overlay" << std::endl;
        showingTooltip = false;
        overlayElement->closeOverlayRecursive();
        context->removeOverlay(overlayElement);
        overlayElement->setParent(nullptr);
        overlayElement->setCollapsed(true);
        overlayElement = {};
    }
}

void LayoutBase::closeOverlayRecursive()
{
    foreachChild([](LayoutBase *child) {
        child->closeOverlayRecursive();
    }, ForeachContext::other, false, true);
    closeOverlay();
}

void LayoutContext::updateWindowRect(const RectI &wr)
{
    windowRect = wr;
    // TODO: resize handling?
}

void LayoutContext::setKeyboardFocus(LayoutPtr focusElement)
{
    if (keyFocus != focusElement) {
        if (keyFocus) {
            keyFocus->onClearKeyFocus();
        }
        keyFocus = focusElement;
        if (keyFocus) {
            keyFocus->onSetKeyFocus();
        }
    }
}

void LayoutContext::setDragFocus(LayoutPtr focusElement)
{
    if (dragFocus != focusElement) {
        if (dragFocus) {
            dragFocus->onClearDragFocus();
        }
        dragFocus = focusElement;
        if (dragFocus) {
            dragFocus->onSetDragFocus();
        }
    }
}

void grow(SizeBound2d &bound, const Padding &pad)
{
    bound.grow(pad.left + pad.right, pad.top + pad.bottom);
}

void grow(SizeBound2d &bound, const Margins &margins, int hBetweenCount, int vBetweenCount)
{
    bound.grow(margins.hSum(hBetweenCount), margins.vSum(vBetweenCount));
}

Vec2d textPos(const RectI &rect, HAlign hAlign, VAlign vAlign)
{
    int x;
    switch (hAlign) {
    case HAlign::left:
        x = rect.pos.x;
        break;
    case HAlign::center:
        x = rect.pos.x + rect.width / 2;
        break;
    case HAlign::right:
        x = rect.pos.x + rect.width;
        break;
    }

    int y;
    switch (vAlign) {
    case VAlign::top:
        y = rect.pos.y;
        break;
    case VAlign::center:
        y = rect.pos.y + rect.height / 2;
        break;
    case VAlign::bottom:
        y = rect.pos.y + rect.height;
        break;
    case VAlign::baseline:
        y = rect.pos.y + rect.height;
        break;
    }

    return {x, y};
}

RectI grown(const RectI &rect, const Padding &pad)
{
    RectI ret = rect;
    grow(ret, pad);
    return ret;
}

RectI shrunk(const RectI &rect, const Padding &pad)
{
    RectI ret = rect;
    shrink(ret, pad);
    return ret;
}
