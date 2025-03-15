#ifndef SIZEBOUND_H
#define SIZEBOUND_H

#include <algorithm>
#include <limits>
#include <optional>
#include <vector>

/**
 * @brief Represents size constraints with minimum and maximum values.
 *
 * SizeBound is used to define size constraints for UI elements,
 * establishing both minimum and maximum size requirements.
 */
class SizeBound {
public:
    /// Maximum possible value for size constraints
    static constexpr int maxV = std::numeric_limits<int>::max();

    /// Minimum required size
    int minSize;

    /// Maximum allowed size
    int maxSize;

public:
    /**
     * @brief Copy constructor
     */
    SizeBound(const SizeBound& other) = default;

    /**
     * @brief Construct a new SizeBound with minimum and maximum constraints
     *
     * @param minSize Minimum required size
     * @param maxSize Maximum allowed size
     */
    constexpr SizeBound(int minSize = 0, int maxSize = maxV) noexcept
        : minSize{minSize}, maxSize{maxSize} {}

    /**
     * @brief Create a fixed size constraint (min = max)
     *
     * @param size Both minimum and maximum size
     * @return Fixed size constraint
     */
    [[nodiscard]] static constexpr SizeBound fixed(int size) noexcept {
        return {size, size};
    }

    /**
     * @brief Create a constraint with only a minimum size
     *
     * @param minSize Minimum required size
     * @return Constraint with specified minimum size
     */
    [[nodiscard]] static constexpr SizeBound atLeast(int minSize) noexcept {
        return {minSize, maxV};
    }

    /**
     * @brief Create a constraint with only a maximum size
     *
     * @param maxSize Maximum allowed size
     * @return Constraint with specified maximum size
     */
    [[nodiscard]] static constexpr SizeBound atMost(int maxSize) noexcept {
        return {0, maxSize};
    }

    /**
     * @brief Add two values safely, respecting the maximum value
     *
     * @param a First value
     * @param b Second value
     * @return Sum of values, or maxV if overflow would occur
     */
    [[nodiscard]] constexpr static int addWithUpperLimit(int a, int b) noexcept;

    /**
     * @brief Check if a value satisfies this constraint
     *
     * @param value Value to check
     * @return true if value is within bounds
     */
    [[nodiscard]] constexpr bool inBound(int value) const noexcept {
        return value >= minSize && value <= maxSize;
    }

    /**
     * @brief Constrain a value to be within bounds
     *
     * @param value Value to constrain
     * @return Constrained value
     */
    [[nodiscard]] constexpr int constrain(int value) const noexcept {
        return std::max(minSize, std::min(maxSize, value));
    }

    /**
     * @brief Increase minimum and maximum by an offset
     *
     * @param offset Amount to increase by
     * @return Reference to this object for chaining
     */
    constexpr SizeBound& grow(int offset) noexcept {
        minSize += offset;
        maxSize = addWithUpperLimit(maxSize, offset);
        return *this;
    }
};

/**
 * @brief Check if a span is too small to satisfy both bounds
 *
 * @param bound1 First size bound
 * @param bound2 Second size bound
 * @param span Total available space
 * @return true if span is insufficient
 */
[[nodiscard]] constexpr bool spanTooSmall(const SizeBound& bound1, const SizeBound& bound2, int span) noexcept {
    return span < bound1.minSize + bound2.minSize;
}

/**
 * @brief Check if a span is too large to satisfy both bounds
 *
 * @param bound1 First size bound
 * @param bound2 Second size bound
 * @param span Total available space
 * @return true if span is too large
 */
[[nodiscard]] constexpr bool spanTooLarge(const SizeBound& bound1, const SizeBound& bound2, int span) noexcept {
    return span > SizeBound::addWithUpperLimit(bound1.maxSize, bound2.maxSize);
}

/**
 * @brief Constrain a value using two adjacent bounds
 *
 * Value must conform to bound1 and (span-value) must conform to bound2
 *
 * @param bound1 First size bound
 * @param bound2 Second size bound
 * @param value Value to constrain (modified in-place)
 * @param span Total available space
 * @return true if constraints can be satisfied
 */
bool limit(const SizeBound& bound1, const SizeBound& bound2, int& value, int span);

/**
 * @brief Create a new constraint that is the intersection of two constraints
 *
 * Takes the maximum of minimums and minimum of maximums
 *
 * @param a First size bound
 * @param b Second size bound
 * @return Intersection of constraints
 */
[[nodiscard]] SizeBound constraintIntersection(const SizeBound& a, const SizeBound& b);

// /**
//  * @brief Legacy alias for constraintIntersection
//  */
// [[nodiscard]] SizeBound mergeA(const SizeBound& a, const SizeBound& b);

/**
 * @brief Create a new constraint that is the intersection of two constraints
 * with a safety check for invalid constraints
 *
 * @param a First size bound
 * @param b Second size bound
 * @return Intersection of constraints with validity check
 */
[[nodiscard]] SizeBound constraintIntersectionSafe(const SizeBound& a, const SizeBound& b);

// /**
//  * @brief Legacy alias for constraintIntersectionSafe
//  */
// [[nodiscard]] SizeBound mergeB(const SizeBound& a, const SizeBound& b);

/**
 * @brief Add two bounds to create a new combined size bound
 *
 * @param a First size bound
 * @param b Second size bound
 * @return Combined size bound
 */
[[nodiscard]] SizeBound operator+ (const SizeBound& a, const SizeBound& b);

/**
 * @brief Find the intersection of multiple bounds
 *
 * @param bounds Collection of bounds to intersect
 * @return Intersection of all bounds
 */
[[nodiscard]] SizeBound constraintIntersection(const std::vector<SizeBound>& bounds);

// /**
//  * @brief Legacy alias for constraintIntersection with vector
//  */
// [[nodiscard]] SizeBound mergeA(const std::vector<SizeBound>& bounds);

/**
 * @brief Find the intersection of multiple bounds with safety check
 *
 * @param bounds Collection of bounds to intersect
 * @return Safe intersection of all bounds
 */
[[nodiscard]] SizeBound constraintIntersectionSafe(const std::vector<SizeBound>& bounds);

// /**
//  * @brief Legacy alias for constraintIntersectionSafe with vector
//  */
// [[nodiscard]] SizeBound mergeB(const std::vector<SizeBound>& bounds);

/**
 * @brief Sum a collection of bounds
 *
 * @param bounds Collection of bounds to sum
 * @return Sum of all bounds
 */
[[nodiscard]] SizeBound sum(const std::vector<SizeBound>& bounds);

/**
 * @brief Represent both horizontal and vertical size constraints
 */
class SizeBound2d {
public:
    /// Horizontal constraints
    SizeBound xBound;

    /// Vertical constraints
    SizeBound yBound;

public:
    /**
     * @brief Copy constructor
     */
    SizeBound2d(const SizeBound2d& other) = default;

    /**
     * @brief Construct with separate min/max for x and y
     *
     * @param minX Minimum width
     * @param maxX Maximum width
     * @param minY Minimum height
     * @param maxY Maximum height
     */
    constexpr SizeBound2d(int minX = 0, int maxX = SizeBound::maxV,
                          int minY = 0, int maxY = SizeBound::maxV) noexcept
        : xBound{minX, maxX}, yBound{minY, maxY} { }

    /**
     * @brief Construct from separate x and y bounds
     *
     * @param xBound Horizontal constraints
     * @param yBound Vertical constraints
     */
    constexpr SizeBound2d(const SizeBound& xBound, const SizeBound& yBound) noexcept
        : xBound{xBound}, yBound{yBound} {}

    /**
     * @brief Create fixed size 2D constraints
     *
     * @param width Fixed width
     * @param height Fixed height
     * @return Fixed size constraints
     */
    [[nodiscard]] constexpr static SizeBound2d fixed(int width, int height) noexcept {
        return {{width,width}, {height,height}};
    }

    /**
     * @brief Grow both dimensions by specified amounts
     *
     * @param xOffset Amount to grow horizontally
     * @param yOffset Amount to grow vertically
     * @return Reference to this object for chaining
     */
    constexpr SizeBound2d& grow(int xOffset, int yOffset) noexcept {
        xBound.grow(xOffset);
        yBound.grow(yOffset);
        return *this;
    }

    /**
     * @brief Grow both dimensions by the same amount
     *
     * @param offset Amount to grow in both directions
     * @return Reference to this object for chaining
     */
    constexpr SizeBound2d& grow(int offset) noexcept {
        return grow(offset, offset);
    }
};

/**
 * @brief Create a vertical stack of bounds
 *
 * Merges x-bounds and adds y-bounds
 *
 * @param a Top bound
 * @param b Bottom bound
 * @return Combined bound
 */
[[nodiscard]] SizeBound2d vStack(const SizeBound2d &a, const SizeBound2d &b);

/**
 * @brief Create a horizontal stack of bounds
 *
 * Adds x-bounds and merges y-bounds
 *
 * @param a Left bound
 * @param b Right bound
 * @return Combined bound
 */
[[nodiscard]] SizeBound2d hStack(const SizeBound2d &a, const SizeBound2d &b);

/**
 * @brief Create a safe intersection of 2D bounds
 *
 * @param a First bound
 * @param b Second bound
 * @return Safe intersection of bounds
 */
[[nodiscard]] SizeBound2d constraintIntersectionSafe(const SizeBound2d &a, const SizeBound2d &b);

/**
 * @brief Legacy alias for constraintIntersectionSafe
 */
[[nodiscard]] SizeBound2d mergeB(const SizeBound2d &a, const SizeBound2d &b);

/**
 * @brief Distribute available space among multiple bounds
 *
 * @param bounds Size constraints for each element
 * @param span Total available space
 * @param stretchAll Whether to stretch all elements if space is available
 * @param growIdx Index of element to prioritize for growth (-1 for none)
 * @return Vector of sizes that satisfy the constraints
 */
[[nodiscard]] std::vector<int> distributeSizes(
    const std::vector<SizeBound>& bounds,
    int span,
    bool stretchAll,
    int growIdx = -1);

// Inline implementation of simple constexpr functions
constexpr int SizeBound::addWithUpperLimit(int a, int b) noexcept {
    if (a == maxV || b == maxV) {
        return maxV;
    }

    // Check for overflow
    if (a > maxV - b) {
        return maxV;
    }

    return a + b;
}

#endif // SIZEBOUND_H
