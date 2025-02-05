#ifndef RECTDRAW_H
#define RECTDRAW_H

#include "canvas2d.h"
#include "layoutcore.h"
#include "recti.h"
#include "vecUtil.h"

#include <numbers>

template<is2dVector V>
void drawRect(mssm::Canvas2d &g, const RectBase<V> &rect, mssm::Color border, mssm::Color fill);

template<is2dVector V>
void drawInsetRect(
    mssm::Canvas2d &g, const RectBase<V> &rect, int inset, mssm::Color border, mssm::Color fill);

template<is2dVector V>
void drawRoundedRect(mssm::Canvas2d &g,
                     const RectBase<V> &rect,
                     int inset,
                     int radius,
                     mssm::Color border,
                     mssm::Color fill);


template<is2dVector V>
void drawRect(mssm::Canvas2d& g, const RectBase<V>& rect, mssm::Color border, mssm::Color fill)
{
    g.rect(cast<Vec2d>(rect.pos), rect.width, rect.height, border, fill);
}

template<is2dVector V>
void drawInsetRect(mssm::Canvas2d& g, const RectBase<V>& rect, int inset, mssm::Color border, mssm::Color fill)
{
    auto insetRect = rect.makeInset(inset);
    g.rect(cast<Vec2d>(insetRect.pos), insetRect.width, insetRect.height, border, fill);
}

constexpr Vec2d ptOnCircle(Vec2d center, double radius, double angle);
void appendArcPts(std::vector<Vec2d>& pts, Vec2d center, double radius, double startAngle, double endAngle, int numPts);
std::vector<Vec2d> arcPts(Vec2d center, double radius, double startAngle, double endAngle, int numPts);

template<is2dVector V>
void drawRoundedRect(mssm::Canvas2d& g, const RectBase<V>& rect, int inset, int radius, mssm::Color border, mssm::Color fill)
{
    using namespace std::numbers;

    int minDim = std::max(0, std::min(rect.width-2*inset, rect.height-2*inset));
    if (minDim == 0) {
        if (rect.width <= 2*inset) {
            if (rect.height <= 2*inset) {
                // just a point!
                return;
            }
            g.lineV(rect.topCenter(inset), rect.bottomCenter(inset), border.a > 0 ? border : fill);
            return;
        }
        g.lineV(rect.leftCenter(inset), rect.rightCenter(inset), border.a > 0 ? border : fill);
        return;
    }

    if (minDim < 2*radius) {
        radius = minDim/2;
    }

    if (radius == 0) {
        g.rectV(rect.upperLeft(), rect.width, rect.height, border, fill);
        return;
    }
    else if (rect.height-2*inset < 2*radius) {
        g.lineV(rect.leftCenter(inset+radius), rect.rightCenter(inset+radius), border.a > 0 ? border : fill);
        return;
    }

    int ptsPerCorner;

    if (radius < 12) {
        if (radius < 8) {
            if (radius < 4) {
                ptsPerCorner = 3;
            }
            else {
                ptsPerCorner = 5;
            }
        }
        else {
            ptsPerCorner = 7;
        }
    }
    else {
        ptsPerCorner = radius/2;
        if (ptsPerCorner % 2 == 0) {
            ptsPerCorner++;
        }
    }

    std::vector<Vec2d> pts;
    pts.reserve(ptsPerCorner*4);
    auto ul = cast<Vec2d>(rect.upperLeft(inset+radius));
    auto ll = cast<Vec2d>(rect.lowerLeft(inset+radius));
    auto lr = cast<Vec2d>(rect.lowerRight(inset+radius));
    auto ur = cast<Vec2d>(rect.upperRight(inset+radius));
    appendArcPts(pts, ur, radius, 0, pi/2, ptsPerCorner);
    appendArcPts(pts, ul, radius, pi/2, pi, ptsPerCorner);
    appendArcPts(pts, ll, radius, pi, 3*pi/2, ptsPerCorner);
    appendArcPts(pts, lr, radius, 3*pi/2, 2*pi, ptsPerCorner);
    g.polygon(pts, border, fill);
}

template<is2dVector V>
void drawRoundedRect(mssm::Canvas2d& g, const RectBase<V>& rect, const Margins& margins, int radius, mssm::Color border, mssm::Color fill)
{
    using namespace std::numbers;

    int ptsPerCorner = 9;
    std::vector<Vec2d> pts;
    pts.reserve(ptsPerCorner*4);
    auto ul = cast<Vec2d>(rect.upperLeft(radius))+Vec2d{margins.left, margins.top};
    auto ll = cast<Vec2d>(rect.lowerLeft(radius))+Vec2d{margins.left, -margins.bottom};
    auto lr = cast<Vec2d>(rect.lowerRight(radius))+Vec2d{-margins.right, -margins.bottom};
    auto ur = cast<Vec2d>(rect.upperRight(radius))+Vec2d{-margins.right, margins.top};
    appendArcPts(pts, ur, radius, 0, pi/2, ptsPerCorner);
    appendArcPts(pts, ul, radius, pi/2, pi, ptsPerCorner);
    appendArcPts(pts, ll, radius, pi, 3*pi/2, ptsPerCorner);
    appendArcPts(pts, lr, radius, 3*pi/2, 2*pi, ptsPerCorner);
    g.polygon(pts, border, fill);
}

#endif // RECTDRAW_H
