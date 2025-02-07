#include "layoutcontext.h"
#include "layoutcore.h"
//#include "vulkdevice.h"
#include <algorithm>
#include <cassert>

// // Use (void) to silence unused warnings.
#undef assertm
#define assertm(exp, msg) assert((void(msg), exp))

void LayoutContext::pushOverlay(LayoutPtr overlay)
{
    overlay->updateLayer(overlays.size()+1, 0);
    overlays.push_back(overlay);
}

void LayoutContext::removeOverlay(LayoutPtr overlay)
{
    //hoverChain.onRemove(overlay);

    if (hoverChain.size() > 0 && hoverChain[0] == overlay) {
        std::cout << "Erasing hover chain in removeOverlay" << std::endl;
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
}


void LayoutContext::updateHoverChain(const PropertyBag& parentProps, LayoutPtr hoverElement, bool isInside, Vec2d pos)
{
    if (!hoverChain.empty()) {
        // is hoverElement in a higher overlay than the hoverChain?

        int elementLayer = hoverElement->getLayer(); // getOverlayLevel(hoverRoot);
        int chainLayer =  hoverChain[0]->getLayer();

        if (elementLayer < chainLayer) {
            // ignore?
            return;
        }

        if (elementLayer > chainLayer) {
            if (isInside) {
                if (!hoverElement->isLocalRoot()) {
                    throw std::logic_error("Hover element not local root");
                }
                std::cout << "mouse entered an overlay higher than current hoverChain" << std::endl;
                std::cout << "When inside element: " << hoverElement->trail() << std::endl;
                truncateHoverChain(parentProps, 0, pos);
                if (!hoverElement->getCollapsed()) {
                    hoverChain.push_back(hoverElement);
                    sendEnter(parentProps, hoverElement, pos);
                }
            }
            else {
                // ignore?
            }
            return;
        }
    }

    int depth = hoverElement->getDepth();

    if (hoverChain.size() < depth && isInside) {
        throw std::logic_error("hoverChain.size() < depth");
    }

    // Note: hoverChain.size() < depth is ok as long as isInside is false

    if (isInside) {
        assertm(hoverChain.size() >= depth, "hoverChain.size() >= depth");

        if (hoverChain.size() == depth) {
            hoverChain.push_back(hoverElement);
            sendEnter(parentProps, hoverElement, pos);
            return;
        }
        else if (hoverChain[depth] == hoverElement) {
            // we were inside, and we're still inside, nothing to do
            return;
        }
        else {
            std::cout << "This place: " << std::endl;

            truncateHoverChain(parentProps, depth, pos);
            hoverChain.push_back(hoverElement);
            sendEnter(parentProps, hoverElement, pos);
            return;
        }
    }
    else {
        // not inside
        if (depth < hoverChain.size() && hoverChain[depth] == hoverElement) {
            // we _were_ inside, so truncate
            std::cout << "This place2: " << std::endl;
            truncateHoverChain(parentProps, depth, pos);
            return;
        }
    }
}

void LayoutContext::truncateHoverChain(const PropertyBag& parentProps, int depth, Vec2d pos)
{
    while (hoverChain.size() > depth) {
        if (debug) {
            std::cout << "Exiting: " << hoverChain.back()->getTypeStr() << std::endl;
        }
        sendExit(parentProps, hoverChain.back(), pos);
        hoverChain.pop_back();
    }
}

void LayoutContext::sendEnter(const PropertyBag& parentProps, LayoutPtr element, Vec2d pos)
{
    std::cout << "  Entering: " << element->trail() << std::endl;
    MouseEvt evt;
    evt.action = MouseEvt::Action::enter;
    evt.button = mssm::MouseButton::None;
    evt.pos = pos;
    MouseEventReason reason = MouseEventReason::within;
    element->onMouse(parentProps, reason, evt);
}

void LayoutContext::sendExit(const PropertyBag &parentProps, LayoutPtr element, Vec2d pos)
{
    if (element->isOverlayRoot()) {
        std::cout << "  Leaving Overlay" << std::endl;
    }
    else if (element->isLocalRoot()) {
        std::cout << "  Leaving Root" << std::endl;
    }
    else {
        auto pp = element->getParentPtr();
        if (pp) {
            std::cout << "  Leaving: " << pp->trail() << std::endl;
        }
    }

    if (element->isShowingTooltip()) {
        element->closeOverlay();
    }

    MouseEvt evt;
    evt.action = MouseEvt::Action::exit;
    evt.button = mssm::MouseButton::None;
    evt.pos = pos;
    MouseEventReason reason = MouseEventReason::exit;
    element->onMouse(parentProps, reason, evt);
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
