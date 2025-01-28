#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "vecUtil.h"
#include <ranges>


void loopOverAdjacentPairs(const std::ranges::range auto& loop, auto lambda)
{
    auto i = begin(loop);
    auto e = end(loop);
    if (i == e) {
        return;
    }
    auto first = &(*i++);
    auto prev = first;
    while (i != e) {
        auto curr = &(*i++);
        lambda(*prev, *curr);
        prev = curr;
    }
    lambda(*prev, *first);
}

template <typename P, typename C> requires is2dVector<P> && std::ranges::range<C>
bool isCCW(C pts)
{
    double sum = 0;
    loopOverAdjacentPairs(pts, [&sum](auto& a, auto& b) {
        sum += cross(a,b);
    });
    return sum < 0;
}

inline bool rayCrossesSeg(const is2dVector auto& s1, const is2dVector auto& s2, const is2dVector auto& p)
{
    return ((s1.y >= p.y && s2.y < p.y) || (s1.y < p.y && s2.y >= p.y)) &&
           ((((p.y - s1.y)*(s2.x-s1.x)/(s2.y-s1.y)) + s1.x) < p.x);
}

bool pointInPolygon(const is2dVector auto& p, const std::ranges::range auto& poly)
{
    using PT = std::remove_const_t<std::remove_reference_t<decltype(p)>>;
    unsigned int crossCount = 0;
    PT s1;
    PT firstPt;
    bool first = true;
    for (const auto& s2 : poly) {
        if (first) {
            first = false;
            firstPt = s2;
        }
        else if (rayCrossesSeg(s1, s2, p)) {
            crossCount++;
        }
        s1 = s2;
    }

    if (rayCrossesSeg(s1, firstPt, p)) {
        crossCount++;
    }

    return crossCount % 2;
}

auto closestPointToSegment(const is2dOr3dVector auto&  p, const is2dOr3dVector auto& s1, const is2dOr3dVector auto& s2, double& tOut) -> is2dOr3dVector auto
{
    auto s = s2-s1;

    double epsilon1 = 0.0001;

    double denom = dot(s,s);

    if (denom < epsilon1) {
        // s1 and s2 are "really close together", so just snap to one of the ends
        auto v1 = p-s1;
        auto v2 = p-s2;
        double d1 = dot(v1,v1);
        double d2 = dot(v2,v2);
        if (d1 < d2) {
            tOut = 0;
            return s1;
        }
        else {
            tOut = 1;
            return s2;
        }
    }

    tOut = dot(p-s1, s) / denom;

    if (tOut <= 0) {
        tOut = 0;
        return s1;
    }
    else if (tOut >= 1) {
        tOut = 1;
        return s2;
    }

    return s1 + tOut * s;
}

double distanceToSegment(const is2dOr3dVector auto& p, const is2dOr3dVector auto& s1, const is2dOr3dVector auto& s2)
{
    double t;
    auto closest = closestPointToSegment(p, s1, s2, t);
    return magnitude(p-closest);
}

bool isLeftOf(const is2dVector auto& s1, const is2dVector auto&  s2, const is2dVector auto&  p)
{
    return cross(s2 - s1, p - s1) < 0;
}

// Note: assumes left hand coordinate system
bool isLeftTurn(const is2dVector auto&  p1, const is2dVector auto&  p2, const is2dVector auto&  p3)
{
    return cross(p2 - p1, p3 - p2) < 0;
}

// Note: assumes left hand coordinate system
bool leftOfVertex(const is2dVector auto&  prevV, const is2dVector auto&  v, const is2dVector auto&  nextV, const is2dVector auto&  p)
{
    bool lt = isLeftTurn(prevV, v, nextV);
    if (lt) {
        // acute angle
        return isLeftOf(v, nextV, p) && isLeftOf(prevV, v, p);
    } else {
        // obtuse angle
        return !(isLeftOf(nextV, v, p) && isLeftOf(v, prevV, p));
    }
}

// template <typename VOUT>
// constexpr auto cast(const is2dVector auto& v) -> VOUT requires is3dVector<VOUT>
// {
//     return {v.x, v.y, 0};
// }

// template <typename VOUT>
// constexpr auto cast(const is3dVector auto& v) -> VOUT requires is2dVector<VOUT>
// {
//     return {v.x, v.y};
// }

// template <typename VOUT>
// constexpr auto cast(const is3dVector auto& v) -> VOUT requires is3dVector<VOUT>
// {
//     return {v.x, v.y, v.z};
// }

#endif // GEOMETRY_H
