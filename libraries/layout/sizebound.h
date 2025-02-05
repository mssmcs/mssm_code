#ifndef SIZEBOUND_H
#define SIZEBOUND_H

#include <algorithm>
#include <limits>
#include <vector>

class SizeBound {
public:
    const static int maxV = std::numeric_limits<int>::max();
    int minSize;
    int maxSize;
public:
    SizeBound(const SizeBound& other) = default;
    constexpr SizeBound(int minSize = 0, int maxSize = maxV) : minSize{minSize}, maxSize{maxSize} {}
    constexpr static int addWithMax(int a, int b);
    bool inBound(int value) const { return value >= minSize && value <= maxSize; }
    int constrain(int value) const { return std::max(minSize, std::min(maxSize, value)); }
    void grow(int offset);
};

constexpr bool spanTooSmall(const SizeBound& bound1, const SizeBound& bound2, int span)
{
    return span < bound1.minSize + bound2.minSize;
}

constexpr bool spanTooLarge(const SizeBound& bound1, const SizeBound& bound2, int span)
{
    return span > SizeBound::addWithMax(bound1.maxSize, bound2.maxSize);
}

bool limit(const SizeBound& bound1, const SizeBound& bound2, int& value, int span);

constexpr int SizeBound::addWithMax(int a, int b)
{
    if (a == maxV || b == maxV)
    {
        return maxV;
    }
    return a + b;
}

SizeBound mergeA(const SizeBound& a, const SizeBound& b);
SizeBound mergeA(const std::vector<SizeBound>& bounds);
SizeBound mergeB(const SizeBound& a, const SizeBound& b);
SizeBound mergeB(const std::vector<SizeBound>& bounds);

SizeBound operator+ (const SizeBound& a, const SizeBound& b);

SizeBound sum(const std::vector<SizeBound>& bounds);

class SizeBound2d {
public:
    SizeBound xBound;
    SizeBound yBound;
public:
    SizeBound2d(const SizeBound2d& other) = default;
    constexpr SizeBound2d(int minX = 0, int maxX = SizeBound::maxV,
                int minY = 0, int maxY = SizeBound::maxV)
        : xBound{minX, maxX}, yBound{minY, maxY} { }
    constexpr SizeBound2d(const SizeBound& xBound, const SizeBound& yBound)
        : xBound{xBound}, yBound{yBound} {}
    constexpr static SizeBound2d fixed(int width, int height) { return {{width,width}, {height,height}}; }
    void grow(int xOffset, int yOffset) { xBound.grow(xOffset); yBound.grow(yOffset); }
};

SizeBound2d vStack(const SizeBound2d& a, const SizeBound2d& b);
SizeBound2d hStack(const SizeBound2d& a, const SizeBound2d& b);
SizeBound2d mergeB(const SizeBound2d& a, const SizeBound2d& b);

std::vector<int> distributeSizes(const std::vector<SizeBound>& bounds, int span, bool grow, int growIdx);

#endif // SIZEBOUND_H
