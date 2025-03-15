#ifndef RECTI_H
#define RECTI_H

#include <algorithm>
#include <concepts>
#include <optional>
#include "vec2d.h"
#include "vecUtil.h"

/**
 * @brief Identifies different regions of a rectangle.
 *
 * Used for hit testing and determining where a point is in relation to a rectangle.
 */
enum class RectRegion {
    ulCorner,      ///< Upper-left corner
    urCorner,      ///< Upper-right corner
    lrCorner,      ///< Lower-right corner
    llCorner,      ///< Lower-left corner
    leftBorder,    ///< Left edge
    topBorder,     ///< Top edge
    rightBorder,   ///< Right edge
    bottomBorder,  ///< Bottom edge
    content,       ///< Inside the rectangle
    outside        ///< Outside the rectangle
};

/**
 * @brief Base template class for integer-based rectangles.
 *
 * RectBase provides a comprehensive set of methods for working with rectangles
 * using integer coordinates, which is ideal for pixel-perfect UI layouts.
 *
 * @tparam V A 2D vector type that has x and y components
 */
template<is2dVector V>
struct RectBase {
    //==============================================================================
    // Member Variables
    //==============================================================================

    V   pos;    ///< Upper-left corner position
    int width;  ///< Width of the rectangle
    int height; ///< Height of the rectangle

    //==============================================================================
    // Construction & Conversion
    //==============================================================================

    /**
     * @brief Default constructor, creates an empty rectangle at origin.
     */
    constexpr RectBase() noexcept : pos{0,0}, width{0}, height{0} {}

    /**
     * @brief Construct from a different vector type, width and height.
     *
     * @tparam VV Any 2D vector type
     * @param pos_ Upper-left position
     * @param width_ Width of rectangle
     * @param height_ Height of rectangle
     */
    template <is2dVector VV>
    constexpr RectBase(VV pos_, int width_, int height_) noexcept
        : pos{cast<V>(pos_)}, width{width_}, height{height_} {}

    /**
     * @brief Construct from a vector, width and height.
     *
     * @param pos_ Upper-left position
     * @param width_ Width of rectangle
     * @param height_ Height of rectangle
     */
    constexpr RectBase(V pos_, int width_, int height_) noexcept
        : pos{pos_}, width{width_}, height{height_} {}

    /**
     * @brief Construct from x, y coordinates, width and height.
     *
     * @param x X coordinate of upper-left
     * @param y Y coordinate of upper-left
     * @param width_ Width of rectangle
     * @param height_ Height of rectangle
     */
    constexpr RectBase(int x, int y, int width_, int height_) noexcept
        : pos{x,y}, width{width_}, height{height_} {}

    //==============================================================================
    // Factory Methods
    //==============================================================================

    /**
     * @brief Create a rectangle from left, top, right, bottom coordinates.
     *
     * @param left Left edge x-coordinate
     * @param top Top edge y-coordinate
     * @param right Right edge x-coordinate
     * @param bottom Bottom edge y-coordinate
     * @return A new rectangle with the specified bounds
     */
    [[nodiscard]] static constexpr RectBase fromLTRB(int left, int top, int right, int bottom) noexcept {
        return {left, top, right - left + 1, bottom - top + 1};
    }

    /**
     * @brief Create a rectangle centered at a point with specified dimensions.
     *
     * @param center Center point of the rectangle
     * @param width_ Width of the rectangle
     * @param height_ Height of the rectangle
     * @return A new rectangle centered at the specified point
     */
    [[nodiscard]] static constexpr RectBase fromCenter(const V& center, int width_, int height_) noexcept {
        return {center.x - width_ / 2, center.y - height_ / 2, width_, height_};
    }

    /**
     * @brief Create a rectangle that encompasses two points.
     *
     * @param p1 First point
     * @param p2 Second point
     * @return A rectangle containing both points
     */
    [[nodiscard]] static constexpr RectBase fromPoints(const V& p1, const V& p2) noexcept {
        int left = std::min(p1.x, p2.x);
        int top = std::min(p1.y, p2.y);
        int right = std::max(p1.x, p2.x);
        int bottom = std::max(p1.y, p2.y);
        return fromLTRB(left, top, right, bottom);
    }

    //==============================================================================
    // Edge Position Getters
    //==============================================================================

    /**
     * @brief Get the left edge x-coordinate.
     *
     * @param inset Optional inset from the edge
     * @return x-coordinate of the left edge with inset
     */
    [[nodiscard]] constexpr int left(int inset = 0) const noexcept { return pos.x + inset; }

    /**
     * @brief Get the right edge x-coordinate.
     *
     * @param inset Optional inset from the edge
     * @return x-coordinate of the right edge with inset
     */
    [[nodiscard]] constexpr int right(int inset = 0) const noexcept { return pos.x + width - 1 - inset; }

    /**
     * @brief Get the top edge y-coordinate.
     *
     * @param inset Optional inset from the edge
     * @return y-coordinate of the top edge with inset
     */
    [[nodiscard]] constexpr int top(int inset = 0) const noexcept { return pos.y + inset; }

    /**
     * @brief Get the bottom edge y-coordinate.
     *
     * @param inset Optional inset from the edge
     * @return y-coordinate of the bottom edge with inset
     */
    [[nodiscard]] constexpr int bottom(int inset = 0) const noexcept { return pos.y + height - 1 - inset; }

    //==============================================================================
    // Geometric Properties
    //==============================================================================

    /**
     * @brief Calculate the area of the rectangle.
     *
     * @return Area in square units
     */
    [[nodiscard]] constexpr int area() const noexcept { return width * height; }

    /**
     * @brief Calculate the perimeter of the rectangle.
     *
     * @return Perimeter in units
     */
    [[nodiscard]] constexpr int perimeter() const noexcept { return 2 * (width + height); }

    /**
     * @brief Calculate the aspect ratio (width/height).
     *
     * @return Aspect ratio, or 0 if height is 0
     */
    [[nodiscard]] constexpr double aspectRatio() const noexcept {
        return height != 0 ? static_cast<double>(width) / height : 0.0;
    }

    /**
     * @brief Check if the rectangle is empty (has zero area).
     *
     * @return true if either width or height is <= 0
     */
    [[nodiscard]] constexpr bool empty() const noexcept { return width <= 0 || height <= 0; }

    /**
     * @brief Check if the rectangle is a square.
     *
     * @return true if width equals height
     */
    [[nodiscard]] constexpr bool isSquare() const noexcept { return width == height; }

    //==============================================================================
    // Position Tests
    //==============================================================================

    /**
     * @brief Test if an x-coordinate is to the left of the rectangle.
     *
     * @param x x-coordinate to test
     * @param inset Optional inset
     * @return true if x is to the left of the rectangle
     */
    [[nodiscard]] constexpr bool toLeft(int x, int inset = 0) const noexcept { return x < left(inset); }

    /**
     * @brief Test if an x-coordinate is to the right of the rectangle.
     *
     * @param x x-coordinate to test
     * @param inset Optional inset
     * @return true if x is to the right of the rectangle
     */
    [[nodiscard]] constexpr bool toRight(int x, int inset = 0) const noexcept { return x > right(inset); }

    /**
     * @brief Test if a y-coordinate is above the rectangle.
     *
     * @param y y-coordinate to test
     * @param inset Optional inset
     * @return true if y is above the rectangle
     */
    [[nodiscard]] constexpr bool above(int y, int inset = 0) const noexcept { return y < top(inset); }

    /**
     * @brief Test if a y-coordinate is below the rectangle.
     *
     * @param y y-coordinate to test
     * @param inset Optional inset
     * @return true if y is below the rectangle
     */
    [[nodiscard]] constexpr bool below(int y, int inset = 0) const noexcept { return y > bottom(inset); }

    /**
     * @brief Test if a y-coordinate is within the vertical span of the rectangle.
     *
     * @param y y-coordinate to test
     * @param inset Optional inset
     * @return true if y is within the vertical span
     */
    [[nodiscard]] constexpr bool withinVertical(int y, int inset = 0) const noexcept {
        return !above(y, inset) && !below(y, inset);
    }

    /**
     * @brief Test if an x-coordinate is within the horizontal span of the rectangle.
     *
     * @param x x-coordinate to test
     * @param inset Optional inset
     * @return true if x is within the horizontal span
     */
    [[nodiscard]] constexpr bool withinHorizontal(int x, int inset = 0) const noexcept {
        return !toLeft(x, inset) && !toRight(x, inset);
    }

    /**
     * @brief Test if a point is within the rectangle.
     *
     * @tparam VV Any 2D vector type
     * @param p Point to test
     * @param inset Optional inset
     * @return true if the point is within the rectangle
     */
    template <is2dVector VV>
    [[nodiscard]] constexpr bool within(const VV& p, int inset = 0) const noexcept {
        return withinHorizontal(p.x, inset) && withinVertical(p.y, inset);
    }

    /**
     * @brief Test if a point is within the rectangle (alias for within).
     *
     * @tparam VV Any 2D vector type
     * @param p Point to test
     * @param inset Optional inset
     * @return true if the point is within the rectangle
     */
    template <is2dVector VV>
    [[nodiscard]] constexpr bool contains(const VV& p, int inset = 0) const noexcept {
        return within(p, inset);
    }

    /**
     * @brief Test if another rectangle is fully contained within this one.
     *
     * @param other Rectangle to test
     * @return true if other is fully contained
     */
    [[nodiscard]] constexpr bool containsRect(const RectBase& other) const noexcept {
        return other.left() >= left() && other.right() <= right() &&
               other.top() >= top() && other.bottom() <= bottom();
    }

    //==============================================================================
    // Margin Tests
    //==============================================================================

    /**
     * @brief Test if a point is within the left margin of the rectangle.
     *
     * @param x x-coordinate to test
     * @param y y-coordinate to test
     * @param inset Inset from the edge
     * @param outset Outset from the edge
     * @return true if point is within the left margin
     */
    [[nodiscard]] constexpr bool withinLeftMargin(int x, int y, int inset = 0, int outset = 0) const noexcept {
        return withinVertical(y, 0) && x >= left(-outset) && x <= left(inset);
    }

    /**
     * @brief Test if a point is within the left margin of the rectangle.
     *
     * @tparam VV Any 2D vector type
     * @param p Point to test
     * @param inset Inset from the edge
     * @param outset Outset from the edge
     * @return true if point is within the left margin
     */
    template <is2dVector VV>
    [[nodiscard]] constexpr bool withinLeftMargin(const VV& p, int inset = 0, int outset = 0) const noexcept {
        return withinLeftMargin(p.x, p.y, inset, outset);
    }

    /**
     * @brief Test if a point is within the right margin of the rectangle.
     *
     * @param x x-coordinate to test
     * @param y y-coordinate to test
     * @param inset Inset from the edge
     * @param outset Outset from the edge
     * @return true if point is within the right margin
     */
    [[nodiscard]] constexpr bool withinRightMargin(int x, int y, int inset = 0, int outset = 0) const noexcept {
        return withinVertical(y, 0) && x >= right(inset) && x <= right(-outset);
    }

    /**
     * @brief Test if a point is within the right margin of the rectangle.
     *
     * @tparam VV Any 2D vector type
     * @param p Point to test
     * @param inset Inset from the edge
     * @param outset Outset from the edge
     * @return true if point is within the right margin
     */
    template <is2dVector VV>
    [[nodiscard]] constexpr bool withinRightMargin(const VV& p, int inset = 0, int outset = 0) const noexcept {
        return withinRightMargin(p.x, p.y, inset, outset);
    }

    /**
     * @brief Test if a point is within the top margin of the rectangle.
     *
     * @param x x-coordinate to test
     * @param y y-coordinate to test
     * @param inset Inset from the edge
     * @param outset Outset from the edge
     * @return true if point is within the top margin
     */
    [[nodiscard]] constexpr bool withinTopMargin(int x, int y, int inset = 0, int outset = 0) const noexcept {
        return withinHorizontal(x, 0) && y >= top(-outset) && y <= top(inset);
    }

    /**
     * @brief Test if a point is within the top margin of the rectangle.
     *
     * @tparam VV Any 2D vector type
     * @param p Point to test
     * @param inset Inset from the edge
     * @param outset Outset from the edge
     * @return true if point is within the top margin
     */
    template <is2dVector VV>
    [[nodiscard]] constexpr bool withinTopMargin(const VV& p, int inset = 0, int outset = 0) const noexcept {
        return withinTopMargin(p.x, p.y, inset, outset);
    }

    /**
     * @brief Test if a point is within the bottom margin of the rectangle.
     *
     * @param x x-coordinate to test
     * @param y y-coordinate to test
     * @param inset Inset from the edge
     * @param outset Outset from the edge
     * @return true if point is within the bottom margin
     */
    [[nodiscard]] constexpr bool withinBottomMargin(int x, int y, int inset = 0, int outset = 0) const noexcept {
        return withinHorizontal(x, 0) && y >= bottom(inset) && y <= bottom(-outset);
    }

    /**
     * @brief Test if a point is within the bottom margin of the rectangle.
     *
     * @tparam VV Any 2D vector type
     * @param p Point to test
     * @param inset Inset from the edge
     * @param outset Outset from the edge
     * @return true if point is within the bottom margin
     */
    template <is2dVector VV>
    [[nodiscard]] constexpr bool withinBottomMargin(const VV& p, int inset = 0, int outset = 0) const noexcept {
        return withinBottomMargin(p.x, p.y, inset, outset);
    }

    //==============================================================================
    // Region Identification
    //==============================================================================

    /**
     * @brief Determine which region of the rectangle a point is in.
     *
     * @param x x-coordinate to test
     * @param y y-coordinate to test
     * @param inset Inset for the inner border
     * @param outset Outset for the outer border
     * @return The region where the point is located
     */
    [[nodiscard]] RectRegion region(int x, int y, int inset, int outset) const noexcept;

    /**
     * @brief Determine which region of the rectangle a point is in.
     *
     * @tparam VV Any 2D vector type
     * @param p Point to test
     * @param inset Inset for the inner border
     * @param outset Outset for the outer border
     * @return The region where the point is located
     */
    template <is2dVector VV>
    [[nodiscard]] RectRegion region(const VV& p, int inset, int outset) const noexcept {
        return region(p.x, p.y, inset, outset);
    }

    //==============================================================================
    // Position Getters
    //==============================================================================

    /**
     * @brief Get the center point of the rectangle.
     *
     * @return Center point
     */
    [[nodiscard]] constexpr V center() const noexcept { return {pos.x + width/2, pos.y + height/2}; }

    /**
     * @brief Get the upper-left point of the rectangle.
     *
     * @param inset Optional inset
     * @return Upper-left point
     */
    [[nodiscard]] constexpr V upperLeft(int inset = 0) const noexcept { return {pos.x + inset, pos.y + inset}; }

    /**
     * @brief Get the upper-right point of the rectangle.
     *
     * @param inset Optional inset
     * @return Upper-right point
     */
    [[nodiscard]] constexpr V upperRight(int inset = 0) const noexcept {
        return {pos.x + width - 1 - inset, pos.y + inset};
    }

    /**
     * @brief Get the lower-left point of the rectangle.
     *
     * @param inset Optional inset
     * @return Lower-left point
     */
    [[nodiscard]] constexpr V lowerLeft(int inset = 0) const noexcept {
        return {pos.x + inset, pos.y + height - 1 - inset};
    }

    /**
     * @brief Get the lower-right point of the rectangle.
     *
     * @param inset Optional inset
     * @return Lower-right point
     */
    [[nodiscard]] constexpr V lowerRight(int inset = 0) const noexcept {
        return {pos.x + width - 1 - inset, pos.y + height - 1 - inset};
    }

    /**
     * @brief Get the midpoint of the top edge.
     *
     * @param inset Optional inset
     * @return Top center point
     */
    [[nodiscard]] constexpr V topCenter(int inset = 0) const noexcept {
        return {pos.x + width/2, pos.y + inset};
    }

    /**
     * @brief Get the midpoint of the bottom edge.
     *
     * @param inset Optional inset
     * @return Bottom center point
     */
    [[nodiscard]] constexpr V bottomCenter(int inset = 0) const noexcept {
        return {pos.x + width/2, pos.y + height - 1 - inset};
    }

    /**
     * @brief Get the midpoint of the left edge.
     *
     * @param inset Optional inset
     * @return Left center point
     */
    [[nodiscard]] constexpr V leftCenter(int inset = 0) const noexcept {
        return {pos.x + inset, pos.y + height/2};
    }

    /**
     * @brief Get the midpoint of the right edge.
     *
     * @param inset Optional inset
     * @return Right center point
     */
    [[nodiscard]] constexpr V rightCenter(int inset = 0) const noexcept {
        return {pos.x + width - 1 - inset, pos.y + height/2};
    }

    //==============================================================================
    // Rectangle Operations
    //==============================================================================

    /**
     * @brief Test if this rectangle overlaps with another.
     *
     * @param other Rectangle to test
     * @return true if rectangles overlap
     */
    [[nodiscard]] constexpr bool overlaps(const RectBase<V>& other) const noexcept
    {
        if (empty() || other.empty()) {
            return false;
        }

        // SIMD-friendly comparison - avoid branching
        bool noOverlap = (right() < other.left()) ||
                         (left() > other.right()) ||
                         (bottom() < other.top()) ||
                         (top() > other.bottom());

        return !noOverlap;
    }

    /**
     * @brief Test if this rectangle overlaps with another (alias for overlaps).
     *
     * @param other Rectangle to test
     * @return true if rectangles overlap
     */
    [[nodiscard]] constexpr bool intersects(const RectBase<V>& other) const noexcept {
        return overlaps(other);
    }

    /**
     * @brief Calculate the intersection of this rectangle with another.
     *
     * Sets this rectangle to the intersection and returns whether intersection exists.
     *
     * @param other Rectangle to intersect with
     * @return true if intersection is non-empty
     */
    constexpr bool intersect(const RectBase<V>& other) noexcept
    {
        if (empty() || other.empty()) {
            width = 0;
            height = 0;
            return false;
        }

        int newLeft = std::max(left(), other.left());
        int newRight = std::min(right(), other.right());

        // Early out - no horizontal overlap
        if (newLeft > newRight) {
            width = 0;
            height = 0;
            return false;
        }

        int newTop = std::max(top(), other.top());
        int newBottom = std::min(bottom(), other.bottom());

        // Early out - no vertical overlap
        if (newTop > newBottom) {
            width = 0;
            height = 0;
            return false;
        }

        pos = {newLeft, newTop};
        width = newRight - newLeft + 1;
        height = newBottom - newTop + 1;
        return true;
    }

    /**
     * @brief Calculate the union of this rectangle with another.
     *
     * @param other Rectangle to union with
     * @return A new rectangle that is the union
     */
    [[nodiscard]] constexpr RectBase united(const RectBase& other) const noexcept {
        if (empty()) return other;
        if (other.empty()) return *this;

        int newLeft = std::min(left(), other.left());
        int newTop = std::min(top(), other.top());
        int newRight = std::max(right(), other.right());
        int newBottom = std::max(bottom(), other.bottom());

        return fromLTRB(newLeft, newTop, newRight, newBottom);
    }

    //==============================================================================
    // Rectangle Transformations
    //==============================================================================

    /**
     * @brief Create a new inset rectangle.
     *
     * @param inset Amount to inset on all sides
     * @return A new inset rectangle
     */
    [[nodiscard]] constexpr RectBase<V> makeInset(int inset) const noexcept {
        if (width <= 2 * inset || height <= 2 * inset) {
            // Return empty rectangle centered at same position
            return {pos.x + width/2, pos.y + height/2, 0, 0};
        }

        RectBase<V> result;
        result.pos = { pos.x + inset, pos.y + inset };
        result.width = width - inset*2;
        result.height = height - inset*2;
        return result;
    }

    /**
     * @brief Shrink this rectangle by inset amount.
     *
     * @param inset Amount to shrink on all sides
     * @return Reference to this rectangle after shrinking
     */
    constexpr RectBase& shrink(int inset) noexcept {
        if (width <= 2 * inset || height <= 2 * inset) {
            // Center the empty rectangle
            int centerX = pos.x + width/2;
            int centerY = pos.y + height/2;
            pos.x = centerX;
            pos.y = centerY;
            width = 0;
            height = 0;
            return *this;
        }

        pos.x += inset;
        pos.y += inset;
        width -= inset*2;
        height -= inset*2;

        return *this;
    }

    /**
 * @brief Grow this rectangle by outset amount.
 *
 * @param outset Amount to grow on all sides
 * @return Reference to this rectangle after growing
 */
    constexpr RectBase& growOutset(int outset) noexcept {
        pos.x -= outset;
        pos.y -= outset;
        width += outset * 2;
        height += outset * 2;

        return *this;
    }

    /**
 * @brief Create a new grown rectangle.
 *
 * @param outset Amount to grow on all sides
 * @return A new expanded rectangle
 */
    [[nodiscard]] constexpr RectBase makeGrown(int outset) const noexcept {
        RectBase result = *this;
        result.growOutset(outset);
        return result;
    }

    /**
     * @brief Position this rectangle to be centered in a container.
     *
     * @param container Rectangle to center within
     * @return Reference to this rectangle after centering
     */
    constexpr RectBase& centerWithin(const RectBase<V>& container) noexcept {
        pos = container.center() - V{width/2, height/2};
        return *this;
    }

    /**
     * @brief Create a copy of this rectangle centered within a container.
     *
     * @param container Rectangle to center within
     * @return A new centered rectangle
     */
    [[nodiscard]] constexpr RectBase centered(const RectBase<V>& container) const noexcept {
        RectBase result = *this;
        result.centerWithin(container);
        return result;
    }

    /**
     * @brief Grow the rectangle downward by specified amount.
     *
     * @param amount Amount to grow down
     * @return Reference to this rectangle
     */
    constexpr RectBase& growDown(int amount) noexcept { height += amount; return *this; }

    /**
     * @brief Grow the rectangle upward by specified amount.
     *
     * @param amount Amount to grow up
     * @return Reference to this rectangle
     */
    constexpr RectBase& growUp(int amount) noexcept { pos.y -= amount; height += amount; return *this; }

    /**
     * @brief Grow the rectangle rightward by specified amount.
     *
     * @param amount Amount to grow right
     * @return Reference to this rectangle
     */
    constexpr RectBase& growRight(int amount) noexcept { width += amount; return *this; }

    /**
     * @brief Grow the rectangle leftward by specified amount.
     *
     * @param amount Amount to grow left
     * @return Reference to this rectangle
     */
    constexpr RectBase& growLeft(int amount) noexcept { pos.x -= amount; width += amount; return *this; }

    //==============================================================================
    // Rectangle Removal Operations
    //==============================================================================

    /**
     * @brief Remove a portion from the left side of the rectangle.
     *
     * @param amount Amount to remove from left
     * @return Reference to this rectangle
     */
    constexpr RectBase& removeLeft(int amount) noexcept {
        amount = std::min(amount, width);
        pos.x += amount;
        width -= amount;
        return *this;
    }

    /**
     * @brief Remove a portion from the right side of the rectangle.
     *
     * @param amount Amount to remove from right
     * @return Reference to this rectangle
     */
    constexpr RectBase& removeRight(int amount) noexcept {
        width = std::max(0, width - amount);
        return *this;
    }

    /**
     * @brief Remove a portion from the top side of the rectangle.
     *
     * @param amount Amount to remove from top
     * @return Reference to this rectangle
     */
    constexpr RectBase& removeTop(int amount) noexcept {
        amount = std::min(amount, height);
        pos.y += amount;
        height -= amount;
        return *this;
    }


/**
     * @brief Remove a portion from the bottom side of the rectangle.
     *
     * @param amount Amount to remove from bottom
     * @return Reference to this rectangle
     */
constexpr RectBase& removeBottom(int amount) noexcept {
    height = std::max(0, height - amount);
    return *this;
}

//==============================================================================
// Comparison Operations
//==============================================================================

/**
     * @brief Check if widths differ between rectangles.
     *
     * @param other Rectangle to compare
     * @return true if widths are different
     */
[[nodiscard]] constexpr bool widthDiffers(const RectBase<V>& other) const noexcept {
    return width != other.width;
}

/**
     * @brief Check if heights differ between rectangles.
     *
     * @param other Rectangle to compare
     * @return true if heights are different
     */
[[nodiscard]] constexpr bool heightDiffers(const RectBase<V>& other) const noexcept {
    return height != other.height;
}

/**
     * @brief Check if dimensions differ between rectangles.
     *
     * @param other Rectangle to compare
     * @return true if either width or height differs
     */
[[nodiscard]] constexpr bool sizeDiffers(const RectBase<V>& other) const noexcept {
    return widthDiffers(other) || heightDiffers(other);
}

/**
     * @brief Check if dimensions are the same between rectangles.
     *
     * @param other Rectangle to compare
     * @return true if both width and height are the same
     */
[[nodiscard]] constexpr bool sizeSame(const RectBase<V>& other) const noexcept {
    return !widthDiffers(other) && !heightDiffers(other);
}

//==============================================================================
// Subrectangle Operations
//==============================================================================

/**
     * @brief Create a subrectangle from the left side.
     *
     * @param newWidth Width of the subrectangle
     * @return A new rectangle
     */
[[nodiscard]] constexpr RectBase<V> leftSubrect(int newWidth) const noexcept {
    return { pos, std::min(newWidth, width), height };
}

/**
     * @brief Create a subrectangle from the right side.
     *
     * @param newWidth Width of the subrectangle
     * @return A new rectangle
     */
[[nodiscard]] constexpr RectBase<V> rightSubrect(int newWidth) const noexcept {
    newWidth = std::min(newWidth, width);
    return { pos.x + width - newWidth, pos.y, newWidth, height };
}

/**
     * @brief Create a subrectangle from the top side.
     *
     * @param newHeight Height of the subrectangle
     * @return A new rectangle
     */
[[nodiscard]] constexpr RectBase<V> topSubrect(int newHeight) const noexcept {
    return { pos, width, std::min(newHeight, height) };
}

/**
     * @brief Create a subrectangle from the bottom side.
     *
     * @param newHeight Height of the subrectangle
     * @return A new rectangle
     */
[[nodiscard]] constexpr RectBase<V> bottomSubrect(int newHeight) const noexcept {
    newHeight = std::min(newHeight, height);
    return { pos.x, pos.y + height - newHeight, width, newHeight };
}

/**
     * @brief Create an inset subrectangle from the left side.
     *
     * @param newWidth Width of the subrectangle
     * @param inset Amount to inset from edges
     * @return A new rectangle
     */
[[nodiscard]] constexpr RectBase<V> leftSubrect(int newWidth, int inset) const noexcept {
    int safeWidth = std::max(0, width - 2 * inset);
    newWidth = std::min(newWidth, safeWidth);
    return { pos.x + inset, pos.y + inset, newWidth, std::max(0, height - 2 * inset) };
}

/**
     * @brief Create an inset subrectangle from the right side.
     *
     * @param newWidth Width of the subrectangle
     * @param inset Amount to inset from edges
     * @return A new rectangle
     */
[[nodiscard]] constexpr RectBase<V> rightSubrect(int newWidth, int inset) const noexcept {
    int safeWidth = std::max(0, width - 2 * inset);
    newWidth = std::min(newWidth, safeWidth);
    return { pos.x + width - newWidth - inset, pos.y + inset,
            newWidth, std::max(0, height - 2 * inset) };
}

/**
     * @brief Create an inset subrectangle from the top side.
     *
     * @param newHeight Height of the subrectangle
     * @param inset Amount to inset from edges
     * @return A new rectangle
     */
[[nodiscard]] constexpr RectBase<V> topSubrect(int newHeight, int inset) const noexcept {
    int safeHeight = std::max(0, height - 2 * inset);
    newHeight = std::min(newHeight, safeHeight);
    return { pos.x + inset, pos.y + inset, std::max(0, width - 2 * inset), newHeight };
}

/**
     * @brief Create an inset subrectangle from the bottom side.
     *
     * @param newHeight Height of the subrectangle
     * @param inset Amount to inset from edges
     * @return A new rectangle
     */
[[nodiscard]] constexpr RectBase<V> bottomSubrect(int newHeight, int inset) const noexcept {
    int safeHeight = std::max(0, height - 2 * inset);
    newHeight = std::min(newHeight, safeHeight);
    return { pos.x + inset, pos.y + height - newHeight - inset,
            std::max(0, width - 2 * inset), newHeight };
}

//==============================================================================
// Split Operations
//==============================================================================

/**
     * @brief Split off a rectangle from the left and modify this rectangle.
     *
     * @param leftWidth Width of the rectangle to split off
     * @return A new rectangle from the left side
     */
[[nodiscard]] constexpr RectBase<V> splitLeft(int leftWidth) noexcept;

/**
     * @brief Split off a rectangle from the right and modify this rectangle.
     *
     * @param rightWidth Width of the rectangle to split off
     * @return A new rectangle from the right side
     */
[[nodiscard]] constexpr RectBase<V> splitRight(int rightWidth) noexcept;

/**
     * @brief Split off a rectangle from the top and modify this rectangle.
     *
     * @param topHeight Height of the rectangle to split off
     * @return A new rectangle from the top side
     */
[[nodiscard]] constexpr RectBase<V> splitTop(int topHeight) noexcept;

/**
     * @brief Split off a rectangle from the bottom and modify this rectangle.
     *
     * @param bottomHeight Height of the rectangle to split off
     * @return A new rectangle from the bottom side
     */
[[nodiscard]] constexpr RectBase<V> splitBottom(int bottomHeight) noexcept;

//==============================================================================
// Static Utility Methods
//==============================================================================

/**
     * @brief Calculate the preferred left width when resizing a pair of rectangles.
     *
     * @param leftRect Left rectangle
     * @param rightRect Right rectangle
     * @param desiredTotalWidth New total width
     * @return Suggested width for the left rectangle
     */
[[nodiscard]] static constexpr int preferredLeftWidth(
    const RectBase<V>& leftRect,
    const RectBase<V>& rightRect,
    int desiredTotalWidth) noexcept;

/**
     * @brief Calculate the preferred top height when resizing a pair of rectangles.
     *
     * @param topRect Top rectangle
     * @param bottomRect Bottom rectangle
     * @param desiredTotalHeight New total height
     * @return Suggested height for the top rectangle
     */
[[nodiscard]] static constexpr int preferredTopHeight(
    const RectBase<V>& topRect,
    const RectBase<V>& bottomRect,
    int desiredTotalHeight) noexcept;

/**
     * @brief Set the width of the left rectangle in a pair, adjusting the right one.
     *
     * @param leftRect Left rectangle to modify
     * @param rightRect Right rectangle to modify
     * @param leftWidth New width for the left rectangle
     */
static void setLeftWidth(RectBase<V>& leftRect, RectBase<V>& rightRect, int leftWidth);

/**
     * @brief Set the height of the top rectangle in a pair, adjusting the bottom one.
     *
     * @param topRect Top rectangle to modify
     * @param bottomRect Bottom rectangle to modify
     * @param topHeight New height for the top rectangle
     */
static void setTopHeight(RectBase<V>& topRect, RectBase<V>& bottomRect, int topHeight);

/**
     * @brief Set the widths of a pair of adjacent rectangles.
     *
     * @param leftRect Left rectangle to modify
     * @param rightRect Right rectangle to modify
     * @param leftWidth New width for the left rectangle
     * @param rightWidth New width for the right rectangle
     */
static constexpr void setWidths(
    RectBase<V>& leftRect,
    RectBase<V>& rightRect,
    int leftWidth,
    int rightWidth) noexcept;

/**
     * @brief Set the heights of a pair of adjacent rectangles.
     *
     * @param topRect Top rectangle to modify
     * @param bottomRect Bottom rectangle to modify
     * @param topHeight New height for the top rectangle
     * @param bottomHeight New height for the bottom rectangle
     */
static constexpr void setHeights(
    RectBase<V>& topRect,
    RectBase<V>& bottomRect,
    int topHeight,
    int bottomHeight) noexcept;

/**
     * @brief Set the position of a horizontal pair of rectangles.
     *
     * @param leftRect Left rectangle to modify
     * @param rightRect Right rectangle to modify
     * @param pos New position for the left rectangle
     */
static constexpr void setPosHPair(
    RectBase<V>& leftRect,
    RectBase<V>& rightRect,
    const V& pos) noexcept;

/**
     * @brief Set the position of a vertical pair of rectangles.
     *
     * @param topRect Top rectangle to modify
     * @param bottomRect Bottom rectangle to modify
     * @param pos New position for the top rectangle
     */
static constexpr void setPosVPair(
    RectBase<V>& topRect,
    RectBase<V>& bottomRect,
    const V& pos) noexcept;

/**
     * @brief Scale a horizontal pair of rectangles to new dimensions.
     *
     * @param leftRect Left rectangle to modify
     * @param rightRect Right rectangle to modify
     * @param newPos New position for the left rectangle
     * @param newTotalWidth New total width
     * @param newHeight New height for both rectangles
     */
static constexpr void scaleHorizontalPair(
    RectBase<V>& leftRect,
    RectBase<V>& rightRect,
    const V& newPos,
    int newTotalWidth,
    int newHeight) noexcept;

/**
     * @brief Scale a vertical pair of rectangles to new dimensions.
     *
     * @param topRect Top rectangle to modify
     * @param bottomRect Bottom rectangle to modify
     * @param newPos New position for the top rectangle
     * @param newWidth New width for both rectangles
     * @param newTotalHeight New total height
     */
static constexpr void scaleVerticalPair(
    RectBase<V>& topRect,
    RectBase<V>& bottomRect,
    const V& newPos,
    int newWidth,
    int newTotalHeight) noexcept;

/**
     * @brief Set the split point between two horizontal rectangles.
     *
     * @param leftRect Left rectangle to modify
     * @param rightRect Right rectangle to modify
     * @param rightStartX New x-coordinate for the start of the right rectangle
     */
static void setSplitPointHorizontal(
    RectBase<V>& leftRect,
    RectBase<V>& rightRect,
    int rightStartX);

/**
     * @brief Set the split point between two vertical rectangles.
     *
     * @param topRect Top rectangle to modify
     * @param bottomRect Bottom rectangle to modify
     * @param bottomStartY New y-coordinate for the start of the bottom rectangle
     */
static void setSplitPointVertical(
    RectBase<V>& topRect,
    RectBase<V>& bottomRect,
    int bottomStartY);
};

//==============================================================================
// Free Function Implementations
//==============================================================================

/**
 * @brief Calculate the intersection of two rectangles.
 *
 * @tparam V Vector type used in the rectangles
 * @param r1 First rectangle
 * @param r2 Second rectangle
 * @return A new rectangle that is the intersection
 */
template<is2dVector V>
[[nodiscard]] constexpr RectBase<V> intersect(const RectBase<V>& r1, const RectBase<V>& r2) noexcept
{
    if (r1.empty() || r2.empty()) {
        return {};
    }

    int newLeft = std::max(r1.left(), r2.left());
    int newRight = std::min(r1.right(), r2.right());

    // Early out - no horizontal overlap
    if (newLeft > newRight) {
        return {};
    }

    int newTop = std::max(r1.top(), r2.top());
    int newBottom = std::min(r1.bottom(), r2.bottom());

    // Early out - no vertical overlap
    if (newTop > newBottom) {
        return {};
    }

    return {newLeft, newTop, newRight-newLeft+1, newBottom-newTop+1};
}

/**
 * @brief Calculate the union of two rectangles.
 *
 * @tparam V Vector type used in the rectangles
 * @param r1 First rectangle
 * @param r2 Second rectangle
 * @return A new rectangle that is the union
 */
template<is2dVector V>
[[nodiscard]] constexpr RectBase<V> unionRect(const RectBase<V>& r1, const RectBase<V>& r2) noexcept
{
    if (r1.empty()) return r2;
    if (r2.empty()) return r1;

    int newLeft = std::min(r1.left(), r2.left());
    int newTop = std::min(r1.top(), r2.top());
    int newRight = std::max(r1.right(), r2.right());
    int newBottom = std::max(r1.bottom(), r2.bottom());

    return RectBase<V>::fromLTRB(newLeft, newTop, newRight, newBottom);
}

/**
 * @brief Split a rectangle horizontally, returning left and right parts.
 *
 * @tparam V Vector type used in the rectangles
 * @param orig Original rectangle
 * @param leftWidth Width of the left part
 * @return A pair of rectangles: {left part, right part}
 */
template<is2dVector V>
[[nodiscard]] constexpr std::pair<RectBase<V>, RectBase<V>> splitLeft(const RectBase<V>& orig, int leftWidth) noexcept
{
    leftWidth = std::min(leftWidth, orig.width);
    return {{orig.pos, leftWidth, orig.height},
            {orig.pos+V{leftWidth,0}, orig.width-leftWidth, orig.height}};
}

/**
 * @brief Split a rectangle horizontally, returning left and right parts.
 *
 * @tparam V Vector type used in the rectangles
 * @param orig Original rectangle
 * @param rightWidth Width of the right part
 * @return A pair of rectangles: {left part, right part}
 */
template<is2dVector V>
[[nodiscard]] constexpr std::pair<RectBase<V>, RectBase<V>> splitRight(const RectBase<V>& orig, int rightWidth) noexcept
{
    rightWidth = std::min(rightWidth, orig.width);
    return {{orig.pos, orig.width-rightWidth, orig.height},
            {orig.pos+V{orig.width-rightWidth,0}, rightWidth, orig.height}};
}

/**
 * @brief Split a rectangle vertically, returning top and bottom parts.
 *
 * @tparam V Vector type used in the rectangles
 * @param orig Original rectangle
 * @param topHeight Height of the top part
 * @return A pair of rectangles: {top part, bottom part}
 */
template<is2dVector V>
[[nodiscard]] constexpr std::pair<RectBase<V>, RectBase<V>> splitTop(const RectBase<V>& orig, int topHeight) noexcept
{
    topHeight = std::min(topHeight, orig.height);
    return {{orig.pos, orig.width, topHeight},
            {orig.pos+V{0,topHeight}, orig.width, orig.height-topHeight}};
}

/**
 * @brief Split a rectangle vertically, returning top and bottom parts.
 *
 * @tparam V Vector type used in the rectangles
 * @param orig Original rectangle
 * @param bottomHeight Height of the bottom part
 * @return A pair of rectangles: {top part, bottom part}
 */
template<is2dVector V>
[[nodiscard]] constexpr std::pair<RectBase<V>, RectBase<V>> splitBottom(const RectBase<V>& orig, int bottomHeight) noexcept
{
    bottomHeight = std::min(bottomHeight, orig.height);
    return {{orig.pos, orig.width, orig.height-bottomHeight},
            {orig.pos+V{0,orig.height-bottomHeight}, orig.width, bottomHeight}};
}

//==============================================================================
// Method Implementations
//==============================================================================

template<is2dVector V>
[[nodiscard]] constexpr RectBase<V> RectBase<V>::splitLeft(int leftWidth) noexcept
{
    leftWidth = std::min(leftWidth, width);
    RectBase<V> ret{pos, leftWidth, height};
    removeLeft(leftWidth);
    return ret;
}

template<is2dVector V>
[[nodiscard]] constexpr RectBase<V> RectBase<V>::splitRight(int rightWidth) noexcept
{
    rightWidth = std::min(rightWidth, width);
    RectBase<V> ret{pos.x + width - rightWidth, pos.y, rightWidth, height};
    removeRight(rightWidth);
    return ret;
}

template<is2dVector V>
[[nodiscard]] constexpr RectBase<V> RectBase<V>::splitTop(int topHeight) noexcept
{
    topHeight = std::min(topHeight, height);
    RectBase<V> ret{pos, width, topHeight};
    removeTop(topHeight);
    return ret;
}

template<is2dVector V>
[[nodiscard]] constexpr RectBase<V> RectBase<V>::splitBottom(int bottomHeight) noexcept
{
    bottomHeight = std::min(bottomHeight, height);
    RectBase<V> ret{pos.x, pos.y + height - bottomHeight, width, bottomHeight};
    removeBottom(bottomHeight);
    return ret;
}

template<is2dVector V>
RectRegion RectBase<V>::region(int x, int y, int inset, int outset) const noexcept
{
    // Fast path - check outside first
    if (toLeft(x, -outset) ||
        toRight(x, -outset) ||
        above(y, -outset) ||
        below(y, -outset)) {
        return RectRegion::outside;
    }

    // Check if inside content area
    if (!toLeft(x, inset) && !toRight(x, inset) &&
        !above(y, inset) && !below(y, inset)) {
        return RectRegion::content;
    }

    // Handle borders and corners
    bool onLeftSide = toLeft(x, inset);
    bool onRightSide = toRight(x, inset);
    bool onTopSide = above(y, inset);
    bool onBottomSide = below(y, inset);

    // Check corners
    if (onLeftSide && onTopSide) return RectRegion::ulCorner;
    if (onLeftSide && onBottomSide) return RectRegion::llCorner;
    if (onRightSide && onTopSide) return RectRegion::urCorner;
    if (onRightSide && onBottomSide) return RectRegion::lrCorner;

    // Check borders
    if (onLeftSide) return RectRegion::leftBorder;
    if (onRightSide) return RectRegion::rightBorder;
    if (onTopSide) return RectRegion::topBorder;
    if (onBottomSide) return RectRegion::bottomBorder;

    // Should never reach here but just in case
    return RectRegion::content;
}

template<is2dVector V>
[[nodiscard]] constexpr int RectBase<V>::preferredLeftWidth(
    const RectBase<V>& leftRect,
    const RectBase<V>& rightRect,
    int desiredTotalWidth) noexcept
{
    double oldTotalWidth = leftRect.width + rightRect.width;
    if (oldTotalWidth <= 0) {
        return desiredTotalWidth/2;
    }
    double scalex = static_cast<double>(desiredTotalWidth) / oldTotalWidth;
    return static_cast<int>(scalex * leftRect.width);
}

template<is2dVector V>
[[nodiscard]] constexpr int RectBase<V>::preferredTopHeight(
    const RectBase<V>& topRect,
    const RectBase<V>& bottomRect,
    int desiredTotalHeight) noexcept
{
    double oldTotalHeight = topRect.height + bottomRect.height;
    if (oldTotalHeight <= 0) {
        return desiredTotalHeight/2;
    }
    double scaley = static_cast<double>(desiredTotalHeight) / oldTotalHeight;
    return static_cast<int>(scaley * topRect.height);
}

template<is2dVector V>
void RectBase<V>::setLeftWidth(RectBase<V>& leftRect, RectBase<V>& rightRect, int leftWidth)
{
    int totalWidth = leftRect.width + rightRect.width;
    if (leftWidth >= totalWidth) {
        throw std::logic_error("leftWidth must be less than totalWidth");
    }
    leftRect.width = leftWidth;
    rightRect.pos.x = leftRect.pos.x + leftWidth;
    rightRect.width = totalWidth - leftWidth;
}

template<is2dVector V>
void RectBase<V>::setTopHeight(RectBase<V>& topRect, RectBase<V>& bottomRect, int topHeight)
{
    int totalHeight = topRect.height + bottomRect.height;
    if (topHeight >= totalHeight) {
        throw std::logic_error("topHeight must be less than totalHeight");
    }
    topRect.height = topHeight;
    bottomRect.pos.y = topRect.pos.y + topHeight;
    bottomRect.height = totalHeight - topHeight;
}

template<is2dVector V>
inline constexpr void RectBase<V>::setWidths(
    RectBase<V>& leftRect,
    RectBase<V>& rightRect,
    int leftWidth,
    int rightWidth) noexcept
{
    leftRect.width = leftWidth;
    rightRect.width = rightWidth;
    rightRect.pos.x = leftRect.right()+1;
}

template<is2dVector V>
inline constexpr void RectBase<V>::setHeights(
    RectBase<V>& topRect,
    RectBase<V>& bottomRect,
    int topHeight,
    int bottomHeight) noexcept
{
    topRect.height = topHeight;
    bottomRect.height = bottomHeight;
    bottomRect.pos.y = topRect.bottom()+1;
}

template<is2dVector V>
inline constexpr void RectBase<V>::setPosHPair(
    RectBase<V>& leftRect,
    RectBase<V>& rightRect,
    const V& pos) noexcept
{
    leftRect.pos = pos;
    rightRect.pos = {leftRect.right()+1, pos.y};
}

template<is2dVector V>
inline constexpr void RectBase<V>::setPosVPair(
    RectBase<V>& topRect,
    RectBase<V>& bottomRect,
    const V& pos) noexcept
{
    topRect.pos = pos;
    bottomRect.pos = {pos.x, topRect.bottom()+1};
}

template<is2dVector V>
inline constexpr void RectBase<V>::scaleHorizontalPair(
    RectBase<V>& leftRect,
    RectBase<V>& rightRect,
    const V& newPos,
    int newTotalWidth,
    int newHeight) noexcept
{
    int newLeftWidth = preferredLeftWidth(leftRect, rightRect, newTotalWidth);

    leftRect.pos = newPos;
    leftRect.width = newLeftWidth;
    leftRect.height = newHeight;
    rightRect.width = newTotalWidth - leftRect.width;
    rightRect.height = newHeight;
    rightRect.pos.x = leftRect.right()+1;
    rightRect.pos.y = newPos.y;
}

template<is2dVector V>
inline constexpr void RectBase<V>::scaleVerticalPair(
    RectBase<V>& topRect,
    RectBase<V>& bottomRect,
    const V& newPos,
    int newWidth,
    int newTotalHeight) noexcept
{
    int newTopHeight = preferredTopHeight(topRect, bottomRect, newTotalHeight);

    topRect.pos = newPos;
    topRect.height = newTopHeight;
    topRect.width = newWidth;
    bottomRect.height = newTotalHeight - topRect.height;
    bottomRect.width = newWidth;
    bottomRect.pos.x = newPos.x;
    bottomRect.pos.y = topRect.bottom()+1;
}

template<is2dVector V>
void RectBase<V>::setSplitPointHorizontal(
    RectBase<V>& leftRect,
    RectBase<V>& rightRect,
    int rightStartX)
{
    leftRect.width = rightStartX - leftRect.pos.x;
    int rightEndX = rightRect.right(-1);
    rightRect.pos.x = rightStartX;
    rightRect.width = rightEndX - rightStartX;
}

template<is2dVector V>
void RectBase<V>::setSplitPointVertical(
    RectBase<V>& topRect,
    RectBase<V>& bottomRect,
    int bottomStartY)
{
    topRect.height = bottomStartY - topRect.pos.y;
    int bottomEndY = bottomRect.bottom(-1);
    bottomRect.pos.y = bottomStartY;
    bottomRect.height = bottomEndY - bottomStartY;
}

/**
 * @brief Integer-based rectangle type for pixel-perfect layout.
 *
 * RectI is a specialization of RectBase using Vec2i32 (32-bit integer vector).
 * This is ideal for UI layout where you want pixel-perfect positioning.
 */
using RectI = RectBase<Vec2i32>;

#endif // RECTI_H
