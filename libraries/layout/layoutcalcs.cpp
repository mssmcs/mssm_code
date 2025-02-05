#include "layoutcalcs.h"

RectI positionTooltip(const RectI &container, const RectI &initiator, Vec2i32 pointer, const RectI &overlayRect)
{
    int y = pointer.y+10;
    int x = pointer.x+10;

    x = std::max(container.left(1), std::min(x, container.right(1)-overlayRect.width));

    if (y+overlayRect.height >= container.bottom()) {
        // can't fit below
        y = pointer.y-10-overlayRect.height;
        if (y <= container.top()) {
            // can't fit above, do the best we can
            y = std::max(container.top(), container.bottom()-overlayRect.height);
        }
    }

    RectI result{{x,y}, overlayRect.width, overlayRect.height};

    result.intersect(container);

    return result;
}

RectI positionMenu(const RectI &container, const RectI &initiator, Vec2i32 pointer, const RectI &overlayRect)
{
    auto region = initiator.region(pointer, 1, 1);

    bool toRight{false};

    switch (region) {
    case RectRegion::leftBorder:
    case RectRegion::topBorder:
        throw std::logic_error("left/top Not implemented yet (positionMenu)");
    case RectRegion::rightBorder:
        toRight = true;
        break;
    case RectRegion::bottomBorder:
        toRight = false;
        break;
    default:
        throw std::logic_error("Not implemented yet (positionMenu)");
    }

    Vec2i32 menuPos;

    if (toRight) {
        menuPos = initiator.upperRight(-1);
        menuPos.y = std::max(container.top(), std::min(menuPos.y, container.bottom()-overlayRect.height));
    }
    else {
        menuPos = initiator.lowerLeft(-1);
        menuPos.x = std::max(container.left(), std::min(menuPos.x, container.right()-overlayRect.width));
    }

    RectI result{menuPos, overlayRect.width, overlayRect.height};

    result.intersect(container);

    return result;
}

RectI positionOverlay(const RectI &container, const RectI &initiator, Vec2i32 pointer, const RectI &overlayRect, OverlayStyle hint)
{
    switch (hint) {
    case OverlayStyle::tooltip:
        return positionTooltip(container, initiator, pointer, overlayRect);
    case OverlayStyle::menu:
        return positionMenu(container, initiator, pointer, overlayRect);
    }

    throw std::logic_error("unsupported style in positionOverlay");
}


