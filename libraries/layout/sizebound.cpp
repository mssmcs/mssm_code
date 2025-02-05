#include "sizebound.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>

SizeBound mergeA(const SizeBound& a, const SizeBound& b)
{
    return { std::max(a.minSize, b.minSize), std::min(a.maxSize, b.maxSize) };
}

SizeBound mergeB(const SizeBound& a, const SizeBound& b)
{
    int newMin = std::max(a.minSize, b.minSize);
    int newMax = std::min(a.maxSize, b.maxSize);
    if (newMax < newMin) {
        newMax = newMin;
    }
    return { newMin, newMax };
}

SizeBound operator+ (const SizeBound& a, const SizeBound& b)
{
    return { SizeBound::addWithMax(a.minSize, b.minSize), SizeBound::addWithMax(a.maxSize, b.maxSize) };
}

SizeBound2d vStack(const SizeBound2d &a, const SizeBound2d &b)
{
    return { mergeB(a.xBound, b.xBound), a.yBound + b.yBound };
}

SizeBound2d hStack(const SizeBound2d &a, const SizeBound2d &b)
{
    return { a.xBound + b.xBound, mergeB(a.yBound, b.yBound) };
}


// constrain value using two adjacent bound
// value must conform to bound 1
// (span-value) must conform to bound 2
bool limit(const SizeBound &bound1, const SizeBound &bound2, int &value, int span)
{
    if (spanTooSmall(bound1, bound2, span))
    {
        value = bound1.minSize;
        return false;
    }

    if (spanTooLarge(bound1, bound2, span))
    {
        value = bound1.maxSize;
        return false;
    }

    // span-value must be >= than bound2.minSize
    // 1 therefore value must be <= span - bound2.minSize

    // span-value must be <= than bound2.maxSize
    // 2 therefore value must be >= span - bound2.maxSize

    // 3 value >= bound1.minSize

    // 4 value <= bound1.maxSize

    // combining 2 and 3 we get:
    //    value >= max(span - bound2.maxSize, bound1.minSize)

    // combining 1 and 4 we get:
    //    value <= min(span - bound2.minSize, bound1.maxSize)

    int lowestValue = std::max(span - bound2.maxSize, bound1.minSize);
    int highestValue = std::min(span - bound2.minSize, bound1.maxSize);

    if (lowestValue > highestValue)
    {
        return false;
    }

    if (value < lowestValue)
    {
        value = lowestValue;
    }
    else if (value > highestValue) {
        value = highestValue;
    }

    return true;
}

SizeBound mergeA(const std::vector<SizeBound> &bounds)
{
    SizeBound result;
    for (const auto &bound : bounds)
    {
        result = mergeA(result, bound);
    }
    return result;
}


SizeBound mergeB(const std::vector<SizeBound> &bounds)
{
    SizeBound result;
    for (const auto &bound : bounds)
    {
        result = mergeB(result, bound);
    }
    return result;
}



SizeBound sum(const std::vector<SizeBound> &bounds)
{
    SizeBound result;
    for (const auto &bound : bounds)
    {
        result = result + bound;
    }
    return result;
}

std::vector<int> distributeSizes(const std::vector<SizeBound> &bounds, int span, bool stretchAll, int growIdx)
{
    std::vector<int> sizes(bounds.size());

    int toDistribute = span;

    int nonFilled = sizes.size();  // track remaining

    // first give each child its minimum size
    for (int i = 0; i < sizes.size(); i++) {
        sizes[i] = bounds[i].minSize;
        toDistribute -= sizes[i];
        if (sizes[i] == bounds[i].maxSize) {
            nonFilled--;
        }
    }

    if (toDistribute < 0) {
         std::cout << "not enough space for minimums" << std::endl;
    }

    if (growIdx >= 0 && toDistribute > 0 && bounds[growIdx].maxSize > sizes[growIdx]) {
        int potentialGrow = bounds[growIdx].maxSize - sizes[growIdx];
        int grow = std::min(potentialGrow, toDistribute);
        sizes[growIdx] += grow;
        toDistribute -= grow;
        if (sizes[growIdx] == bounds[growIdx].maxSize) {
            nonFilled--;
        }
    }

    if (stretchAll) {
        // distribute remaining space, respecting maximums
        while (toDistribute > 0 && nonFilled > 0) {
            int amountPerChild = std::max(1, toDistribute / nonFilled);
            for (int i = 0; i < bounds.size(); i++) {
                if (sizes[i] == bounds[i].maxSize) {
                    continue;
                }
                int amountToAdd = std::min(amountPerChild, bounds[i].maxSize - sizes[i]);
                sizes[i] += amountToAdd;
                toDistribute -= amountToAdd;
                if (sizes[i] == bounds[i].maxSize) {
                    nonFilled--;
                }
                if (toDistribute <= 0) {
                    break;
                }
            }
        }

        if (toDistribute > 0) {
            //std::cout << "unfilled space after growing" << std::endl;
        }
    }

    return sizes;
}



void SizeBound::grow(int offset)
{
    minSize += offset;
    maxSize = addWithMax(maxSize, offset);
}

SizeBound2d mergeB(const SizeBound2d &a, const SizeBound2d &b)
{
    return {mergeB(a.xBound, b.xBound), mergeB(a.yBound, b.yBound)};
}
