#include "sizebound.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>

SizeBound constraintIntersection(const SizeBound& a, const SizeBound& b) {
    return { std::max(a.minSize, b.minSize), std::min(a.maxSize, b.maxSize) };
}

// SizeBound mergeA(const SizeBound& a, const SizeBound& b) {
//     return constraintIntersection(a, b);
// }

SizeBound constraintIntersectionSafe(const SizeBound& a, const SizeBound& b) {
    int newMin = std::max(a.minSize, b.minSize);
    int newMax = std::min(a.maxSize, b.maxSize);

    // If constraints are incompatible, use minimum as both min and max
    if (newMax < newMin) {
        newMax = newMin;
    }

    return { newMin, newMax };
}

// SizeBound mergeB(const SizeBound& a, const SizeBound& b) {
//     return constraintIntersectionSafe(a, b);
// }

SizeBound operator+ (const SizeBound& a, const SizeBound& b) {
    return {
        SizeBound::addWithUpperLimit(a.minSize, b.minSize),
        SizeBound::addWithUpperLimit(a.maxSize, b.maxSize)
    };
}

SizeBound2d vStack(const SizeBound2d &a, const SizeBound2d &b) {
    return { constraintIntersectionSafe(a.xBound, b.xBound), a.yBound + b.yBound };
}

SizeBound2d hStack(const SizeBound2d &a, const SizeBound2d &b) {
    return { a.xBound + b.xBound, constraintIntersectionSafe(a.yBound, b.yBound) };
}

bool limit(const SizeBound &bound1, const SizeBound &bound2, int &value, int span) {
    // Check if constraints can't be satisfied
    if (spanTooSmall(bound1, bound2, span)) {
        value = bound1.minSize;
        return false;
    }

    if (spanTooLarge(bound1, bound2, span)) {
        value = bound1.maxSize;
        return false;
    }

    // Calculate min and max bounds for value based on both constraints

    // span-value must be >= than bound2.minSize
    // therefore value must be <= span - bound2.minSize

    // span-value must be <= than bound2.maxSize
    // therefore value must be >= span - bound2.maxSize

    // Combining with bound1's constraints:
    // value >= max(span - bound2.maxSize, bound1.minSize)
    // value <= min(span - bound2.minSize, bound1.maxSize)

    int lowestValue = std::max(span - bound2.maxSize, bound1.minSize);
    int highestValue = std::min(span - bound2.minSize, bound1.maxSize);

    if (lowestValue > highestValue) {
        return false;
    }

    // Constrain value to the calculated bounds
    value = std::clamp(value, lowestValue, highestValue);

    return true;
}

SizeBound constraintIntersection(const std::vector<SizeBound> &bounds) {
    if (bounds.empty()) {
        return {}; // Return default SizeBound if vector is empty
    }

    SizeBound result = bounds[0];
    for (size_t i = 1; i < bounds.size(); ++i) {
        result = constraintIntersection(result, bounds[i]);
    }
    return result;
}

// SizeBound mergeA(const std::vector<SizeBound> &bounds) {
//     return constraintIntersection(bounds);
// }

SizeBound constraintIntersectionSafe(const std::vector<SizeBound> &bounds) {
    if (bounds.empty()) {
        return {}; // Return default SizeBound if vector is empty
    }

    SizeBound result = bounds[0];
    for (size_t i = 1; i < bounds.size(); ++i) {
        result = constraintIntersectionSafe(result, bounds[i]);
    }
    return result;
}

// SizeBound mergeB(const std::vector<SizeBound> &bounds) {
//     return constraintIntersectionSafe(bounds);
// }

SizeBound sum(const std::vector<SizeBound> &bounds) {
    SizeBound result{0, 0};
    for (const auto &bound : bounds) {
        result = result + bound;
    }
    return result;
}

std::vector<int> distributeSizes(const std::vector<SizeBound> &bounds, int span, bool stretchAll, int growIdx) {
    std::vector<int> sizes(bounds.size());

    // Remaining space to distribute
    int toDistribute = span;

    // Number of elements that haven't reached their maximum
    int nonFilled = bounds.size();

    // First give each child its minimum size
    for (size_t i = 0; i < sizes.size(); i++) {
        sizes[i] = bounds[i].minSize;
        toDistribute -= sizes[i];

        // If this element is already at its maximum, reduce nonFilled count
        if (sizes[i] == bounds[i].maxSize) {
            nonFilled--;
        }
    }

    // If we don't have enough space for the minimums, return what we have
    if (toDistribute < 0) {
        std::cout << "Not enough space for minimum sizes" << std::endl;
        return sizes;
    }

    // If we have a specific element to prioritize for growth
    if (growIdx >= 0 && growIdx < static_cast<int>(bounds.size()) &&
        toDistribute > 0 && bounds[growIdx].maxSize > sizes[growIdx]) {

        int potentialGrow = bounds[growIdx].maxSize - sizes[growIdx];
        int grow = std::min(potentialGrow, toDistribute);
        sizes[growIdx] += grow;
        toDistribute -= grow;

        // If this element reached its maximum, reduce nonFilled count
        if (sizes[growIdx] == bounds[growIdx].maxSize) {
            nonFilled--;
        }
    }

    // Optionally distribute remaining space to all elements
    if (stretchAll) {
        // Continue distributing space while we have space and elements that can grow
        while (toDistribute > 0 && nonFilled > 0) {
            // Calculate amount to distribute per element this round
            int amountPerChild = std::max(1, toDistribute / nonFilled);

            // Distribute to each element that has room to grow
            for (size_t i = 0; i < bounds.size(); i++) {
                // Skip elements that are already at max
                if (sizes[i] == bounds[i].maxSize) {
                    continue;
                }

                // Add space to this element
                int amountToAdd = std::min(amountPerChild, bounds[i].maxSize - sizes[i]);
                sizes[i] += amountToAdd;
                toDistribute -= amountToAdd;

                // If this element reached its maximum, reduce nonFilled count
                if (sizes[i] == bounds[i].maxSize) {
                    nonFilled--;
                }

                // Exit early if we've distributed all space
                if (toDistribute <= 0) {
                    break;
                }
            }
        }
    }

    return sizes;
}

SizeBound2d constraintIntersectionSafe(const SizeBound2d &a, const SizeBound2d &b) {
    return {
        constraintIntersectionSafe(a.xBound, b.xBound),
        constraintIntersectionSafe(a.yBound, b.yBound)
    };
}

SizeBound2d mergeB(const SizeBound2d &a, const SizeBound2d &b) {
    return constraintIntersectionSafe(a, b);
}
