#ifndef LAYOUTCALCS_H
#define LAYOUTCALCS_H

#include "recti.h"
#include "sizebound.h"

enum class OverlayStyle {
    tooltip,
    menu
};

constexpr SizeBound2d boundMax(const RectI &maxRect)
{
    return {{0, maxRect.width}, {0, maxRect.height}};
}

RectI positionOverlay(const RectI& container, const RectI& initiator, Vec2i32 pointer, const RectI &overlayRect, OverlayStyle hint);


#endif // LAYOUTCALCS_H
