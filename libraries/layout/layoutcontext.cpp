#include "layoutcontext.h"
#include "layoutcore.h"
//#include "vulkdevice.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

std::string MouseEvt::actionName() const {
    switch (action) {
    case Action::none: return "none";
    case Action::enter: return "enter";
    case Action::exit: return "exit";
    case Action::move: return "move";
    case Action::drag: return "drag";
    case Action::press: return "press";
    case Action::release: return "release";
    case Action::scroll: return "scroll";
    default: return "unknown";
    }
}

[[noreturn]] void LayoutContext::failValidation(const char* stage, const std::string& message) const
{
#ifndef NDEBUG
    std::stringstream ss;
    ss << "LayoutContext validation failed";
    if (stage) {
        ss << " [" << stage << "]";
    }
    ss << ": " << message << "\n" << buildDebugStateSummary();
    throw std::logic_error(ss.str());
#else
    (void)stage;
    (void)message;
    throw std::logic_error("LayoutContext validation failed");
#endif
}

std::string LayoutContext::buildDebugStateSummary() const
{
#ifndef NDEBUG
    std::stringstream ss;
    ss << "eventDispatchDepth=" << eventDispatchDepth << "\n";
    ss << "overlays(" << overlays.size() << "):";
    if (overlays.empty()) {
        ss << " <none>\n";
    }
    else {
        ss << "\n";
        for (int i = 0; i < static_cast<int>(overlays.size()); ++i) {
            const auto& ov = overlays[i];
            ss << "  [" << i << "] ";
            if (!ov) {
                ss << "<null>\n";
                continue;
            }
            ss << ov->getTypeStr()
               << " name='" << ov->getName() << "'"
               << " layer=" << ov->getLayer()
               << " depth=" << ov->getDepth()
               << " collapsed=" << (ov->getCollapsed() ? "true" : "false")
               << "\n";
        }
    }

    ss << "hoverChain(" << hoverChain.size() << "):";
    if (hoverChain.empty()) {
        ss << " <none>\n";
    }
    else {
        ss << "\n";
        for (int i = 0; i < static_cast<int>(hoverChain.size()); ++i) {
            const auto& h = hoverChain[i];
            ss << "  [" << i << "] ";
            if (!h.element) {
                ss << "<null>\n";
                continue;
            }
            ss << h.element->getTypeStr()
               << " name='" << h.element->getName() << "'"
               << " layer=" << h.element->getLayer()
               << " depth=" << h.element->getDepth()
               << " hoverTime=" << h.hoverTime
               << " collapsed=" << (h.element->getCollapsed() ? "true" : "false")
               << "\n";
        }
    }
    return ss.str();
#else
    return {};
#endif
}

void LayoutContext::validateHoverChain(const char* stage) const
{
#ifndef NDEBUG
    if (hoverChain.empty()) {
        return;
    }

    const auto& first = hoverChain[0].element;
    if (!first) {
        failValidation(stage, "hoverChain[0] is null");
    }
    if (first->getDepth() != 0) {
        failValidation(stage, "hover chain must start at local root (depth 0)");
    }
    if (first->getCollapsed()) {
        failValidation(stage, "collapsed element cannot be in hover chain");
    }

    int expectedLayer = first->getLayer();
    for (int i = 0; i < static_cast<int>(hoverChain.size()); ++i) {
        const auto& element = hoverChain[i].element;
        if (!element) {
            failValidation(stage, "hover chain contains null element at index " + std::to_string(i));
        }
        if (element->getDepth() != i) {
            failValidation(stage, "hover depth/index mismatch at index " + std::to_string(i));
        }
        if (element->getLayer() != expectedLayer) {
            failValidation(stage, "hover layer mismatch at index " + std::to_string(i));
        }
        if (element->getCollapsed()) {
            failValidation(stage, "collapsed hover element at index " + std::to_string(i));
        }

        if (i > 0) {
            auto parent = element->getParentPtr();
            if (!parent) {
                failValidation(stage, "hover chain element missing parent at index " + std::to_string(i));
            }
            if (parent.get() != hoverChain[i - 1].element.get()) {
                failValidation(stage, "hover chain parent linkage broken at index " + std::to_string(i));
            }
        }
    }
#else
    (void)stage;
#endif
}

void LayoutContext::validateOverlayStack(const char* stage) const
{
#ifndef NDEBUG
    if (eventDispatchDepth < 0) {
        failValidation(stage, "eventDispatchDepth underflow");
    }

    std::unordered_set<const LayoutBase*> seen;
    for (int i = 0; i < static_cast<int>(overlays.size()); ++i) {
        const auto& overlay = overlays[i];
        if (!overlay) {
            failValidation(stage, "overlay pointer is null at index " + std::to_string(i));
        }
        if (!overlay->isLocalRoot()) {
            failValidation(stage, "overlay must be local root at index " + std::to_string(i));
        }
        if (overlay->getLayer() != i + 1) {
            failValidation(stage, "overlay layer mismatch at index " + std::to_string(i));
        }
        auto [_, inserted] = seen.insert(overlay.get());
        if (!inserted) {
            failValidation(stage, "duplicate overlay pointer in stack at index " + std::to_string(i));
        }
    }
#else
    (void)stage;
#endif
}


void LayoutContext::pushOverlayNow(LayoutPtr overlay)
{
    validateOverlayStack("prePushOverlayNow");
    overlay->updateLayer(overlays.size()+1, 0);
    overlays.push_back(overlay);
    this->setNeedsResize();
    validateOverlayStack("postPushOverlayNow");
}

void LayoutContext::removeOverlayNow(LayoutPtr overlay)
{
    validateOverlayStack("preRemoveOverlayNow");
    validateHoverChain("preRemoveOverlayNow");
    //hoverChain.onRemove(overlay);

    if (hoverChain.size() > 0 && hoverChain[0].element == overlay) {
        // for (int i = hoverChain.size()-1; i >= 0; i--) {
        //     sendExit(hoverChain[i], {0,0});
        // }
        hoverChain.clear();
    }

    int removedIdx = -1;

    for (int i = 0; i < overlays.size(); i++) {
        if (overlays[i] == overlay) {
            overlays.erase(overlays.begin() + i);
            removedIdx = i;
            break;
        }
    }

    for (int i = removedIdx; i < overlays.size(); i++) {
        overlays[i]->updateLayer(i+1, 0);
    }
    validateOverlayStack("postRemoveOverlayNow");
    validateHoverChain("postRemoveOverlayNow");
}

void LayoutContext::pushOverlay(LayoutPtr overlay)
{
    if (eventDispatchDepth > 0) {
        pendingOverlayMutations.push_back({OverlayMutationType::push, overlay});
        setNeedsResize();
        return;
    }
    pushOverlayNow(overlay);
}

void LayoutContext::removeOverlay(LayoutPtr overlay)
{
    if (eventDispatchDepth > 0) {
        pendingOverlayMutations.push_back({OverlayMutationType::remove, overlay});
        setNeedsResize();
        return;
    }
    removeOverlayNow(overlay);
}

void LayoutContext::beginEventDispatch()
{
    eventDispatchDepth++;
    validateOverlayStack("beginEventDispatch");
}

void LayoutContext::endEventDispatch()
{
    validateOverlayStack("preEndEventDispatch");
    if (eventDispatchDepth == 0) {
        return;
    }
    eventDispatchDepth--;
    if (eventDispatchDepth == 0) {
        applyPendingOverlayMutations();
    }
    validateOverlayStack("postEndEventDispatch");
}

void LayoutContext::applyPendingOverlayMutations()
{
    validateOverlayStack("preApplyPendingOverlayMutations");
    validateHoverChain("preApplyPendingOverlayMutations");
    if (pendingOverlayMutations.empty()) {
        return;
    }
    auto mutations = std::move(pendingOverlayMutations);
    pendingOverlayMutations.clear();
    for (const auto& mutation : mutations) {
        if (!mutation.overlay) {
            continue;
        }
        switch (mutation.type) {
        case OverlayMutationType::push:
            pushOverlayNow(mutation.overlay);
            break;
        case OverlayMutationType::remove:
            removeOverlayNow(mutation.overlay);
            break;
        }
    }
    validateOverlayStack("postApplyPendingOverlayMutations");
    validateHoverChain("postApplyPendingOverlayMutations");
}


void LayoutContext::updateHoverChain(const PropertyBag& parentProps, LayoutPtr hoverElement, bool isInside, Vec2d pos)
{
    validateHoverChain("preUpdateHoverChain");
    // Hover-chain invariants:
    // - Chain entries are ordered root -> leaf for one local root/layer.
    // - Index in chain corresponds to element depth for the active local root.
    // - Crossing layer boundaries truncates and re-roots the chain.
    if (!hoverChain.empty()) {
        // is hoverElement in a higher overlay than the hoverChain?

        int elementLayer = hoverElement->getLayer(); // getOverlayLevel(hoverRoot);
        int chainLayer =  hoverChain[0].element->getLayer();

        if (elementLayer < chainLayer) {
            // ignore?
            validateHoverChain("postUpdateHoverChainIgnoreLowerLayer");
            return;
        }

        if (elementLayer > chainLayer) {
            if (isInside) {
                if (!hoverElement->isLocalRoot()) {
                    throw std::logic_error("Hover element not local root");
                }
                truncateHoverChain(parentProps, 0, pos);
                if (!hoverElement->getCollapsed()) {
                    hoverChain.push_back({hoverElement, 0.0});
                    sendEnter(parentProps, hoverElement, pos);
                }
            }
            else {
                // ignore?
            }
            validateHoverChain("postUpdateHoverChainLayerTransition");
            return;
        }
    }

    int depth = hoverElement->getDepth();

    if (hoverChain.size() < depth && isInside) {
        if (hoverChain.size() == 0) {
            // example of when this happens: button click inside overlay
            // is seen by button "underneath" the overlay, and the
            // button handler removes or replaces overlay
            // which triggers clearing hover chain, but event is still
            // propagating through the non-overlay tree
            validateHoverChain("postUpdateHoverChainIgnoreCleared");
            return;
        }
        throw std::logic_error("hoverChain.size() < depth");
    }

    // Note: hoverChain.size() < depth is ok as long as isInside is false

    if (isInside) {
        if (hoverChain.size() < depth) {
            failValidation("updateHoverChainInside", "hover chain shorter than depth");
        }

        if (hoverChain.size() == depth) {
            hoverChain.push_back({hoverElement, 0.0});
            sendEnter(parentProps, hoverElement, pos);
            validateHoverChain("postUpdateHoverChainAppend");
            return;
        }
        else if (hoverChain[depth].element == hoverElement) {
            // we were inside, and we're still inside, nothing to do
            validateHoverChain("postUpdateHoverChainNoop");
            return;
        }
        else {
            truncateHoverChain(parentProps, depth, pos);
            hoverChain.push_back({hoverElement, 0.0});
            sendEnter(parentProps, hoverElement, pos);
            validateHoverChain("postUpdateHoverChainTruncateAndAppend");
            return;
        }
    }
    else {
        // not inside
        if (depth < hoverChain.size() && hoverChain[depth].element == hoverElement) {
            // we _were_ inside, so truncate
            truncateHoverChain(parentProps, depth, pos);
            validateHoverChain("postUpdateHoverChainTruncate");
            return;
        }
    }
    validateHoverChain("postUpdateHoverChainNoChange");
}

void LayoutContext::iterateHoverChain(std::function<void (LayoutBase *)> f)
{
    for (auto& e : hoverChain) {
        f(e.element.get());
    }
}

void LayoutContext::iterateHoverChain(std::function<void (LayoutBase *, double)> f)
{
    for (auto& e : hoverChain) {
        f(e.element.get(), e.hoverTime);
    }
}

void LayoutContext::updateHoverTimes(double elapsedTimeS)
{
    for (auto& e : hoverChain) {
        e.hoverTime += elapsedTimeS;
    }
}

double LayoutContext::getHoverTime(const LayoutBase *element) const
{
    for (auto& e : hoverChain) {
        if (e.element.get() == element) {
            return e.hoverTime;
        }
    }
    return 0.0;
}

void LayoutContext::truncateHoverChain(const PropertyBag& parentProps, int depth, Vec2d pos)
{
    validateHoverChain("preTruncateHoverChain");
    while (hoverChain.size() > depth) {
        sendExit(parentProps, hoverChain.back().element, pos);
        hoverChain.pop_back();
    }
    validateHoverChain("postTruncateHoverChain");
}

void LayoutContext::resizeOverlays(const PropertyBag &parentProps, const RectI &rect)
{
    for (int i = 0; i < overlays.size(); i++) {
        overlays[i]->resize(parentProps, rect);
        overlays[i]->updateLayer(i+1, 0);
    }
}

void LayoutContext::sendEnter(const PropertyBag& parentProps, LayoutPtr element, Vec2d pos)
{
    MouseEvt evt;
    evt.action = MouseEvt::Action::enter;
    evt.button = mssm::MouseButton::None;
    evt.pos = pos;
    MouseEventReason reason = MouseEventReason::within;
    element->onMouse(parentProps, reason, evt);
}

void LayoutContext::sendExit(const PropertyBag &parentProps, LayoutPtr element, Vec2d pos)
{
    if (element->isShowingTooltip()) {
        element->closeOverlay();
    }

    MouseEvt evt;
    evt.button = mssm::MouseButton::None;
    evt.pos = pos;

    if (element->hasOverlay()) {
        evt.action = MouseEvt::Action::exitOverlayParent;
        element->getOverlay()->onMouse(parentProps, MouseEventReason::exitOverlay, evt);
    }

    evt.action = MouseEvt::Action::exit;

    element->onMouse(parentProps, MouseEventReason::exit, evt);
}

void HoverChain::onRemove(LayoutPtr ptr)
{
    for (int i = 0; i < chain.size(); i++) {
        if (chain[i] == ptr) {
            chain.resize(i);
            return;
        }
    }
}
