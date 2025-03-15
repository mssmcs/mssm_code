#ifndef RECTI_H
#define RECTI_H

#include <algorithm>
#include "vec2d.h"
#include "vecUtil.h"

enum class RectRegion {
    ulCorner,
    urCorner,
    lrCorner,
    llCorner,
    leftBorder,
    topBorder,
    rightBorder,
    bottomBorder,
    content,
    outside,
};

template<is2dVector V>
struct RectBase {
    V   pos;
    int width;
    int height;
public:
    // Constructors remain the same but add noexcept
    constexpr RectBase() noexcept : pos{0,0}, width{0}, height{0} {}

    template <is2dVector VV>
    constexpr RectBase(VV pos, int width, int height) noexcept
        : pos{cast<V>(pos)}, width{width}, height{height} {}

    constexpr RectBase(V pos, int width, int height) noexcept
        : pos{pos}, width{width}, height{height} {}

    constexpr RectBase(int x, int y, int width, int height) noexcept
        : pos{x,y}, width{width}, height{height} {}

    // Basic getters with noexcept
    constexpr int left(int inset = 0) const noexcept { return pos.x+inset; }
    constexpr int right(int inset = 0) const noexcept { return pos.x+width-1-inset; }
    constexpr int top(int inset = 0) const noexcept { return pos.y+inset; }
    constexpr int bottom(int inset = 0) const noexcept { return pos.y+height-1-inset; }

    // Position tests with noexcept
    constexpr bool toLeft(int x, int inset = 0) const noexcept { return x < left(inset); }
    constexpr bool toRight(int x, int inset = 0) const noexcept { return x > right(inset); }
    constexpr bool above(int y, int inset = 0) const noexcept { return y < top(inset); }
    constexpr bool below(int y, int inset = 0) const noexcept { return y > bottom(inset); }

    // Within tests with noexcept
    constexpr bool withinVertical(int y, int inset = 0) const noexcept {
        return !above(y, inset) && !below(y, inset);
    }

    constexpr bool withinHorizontal(int x, int inset = 0) const noexcept {
        return !toLeft(x, inset) && !toRight(x, inset);
    }

    // Use const references for vector parameters
    template <is2dVector VV>
    constexpr bool within(const VV& p, int inset = 0) const noexcept {
        return withinHorizontal(p.x, inset) && withinVertical(p.y, inset);
    }

    // Margin tests with improved parameter passing and noexcept
    constexpr bool withinLeftMargin(int x, int y, int inset = 0, int outset = 0) const noexcept
    {
        return withinVertical(y, 0) && x >= left(-outset) && x <= left(inset);
    }

    template <is2dVector VV>
    constexpr bool withinLeftMargin(const VV& p, int inset = 0, int outset = 0) const noexcept
    {
        return withinLeftMargin(p.x, p.y, inset, outset);
    }

    constexpr bool withinRightMargin(int x, int y, int inset = 0, int outset = 0) const noexcept
    {
        return withinVertical(y, 0) && x >= right(inset) && x <= right(-outset);
    }

    template <is2dVector VV>
    constexpr bool withinRightMargin(const VV& p, int inset = 0, int outset = 0) const noexcept
    {
        return withinRightMargin(p.x, p.y, inset, outset);
    }

    constexpr bool withinTopMargin(int x, int y, int inset = 0, int outset = 0) const noexcept
    {
        return withinHorizontal(x, 0) && y >= top(-outset) && y <= top(inset);
    }

    template <is2dVector VV>
    constexpr bool withinTopMargin(const VV& p, int inset = 0, int outset = 0) const noexcept
    {
        return withinTopMargin(p.x, p.y, inset, outset);
    }

    constexpr bool withinBottomMargin(int x, int y, int inset = 0, int outset = 0) const noexcept
    {
        return withinHorizontal(x, 0) && y >= bottom(inset) && y <= bottom(-outset);
    }

    template <is2dVector VV>
    constexpr bool withinBottomMargin(const VV& p, int inset = 0, int outset = 0) const noexcept
    {
        return withinBottomMargin(p.x, p.y, inset, outset);
    }

    // Region identification with const references
    RectRegion region(int x, int y, int inset, int outset) const noexcept;

    template <is2dVector VV>
    RectRegion region(const VV& p, int inset, int outset) const noexcept {
        return region(p.x, p.y, inset, outset);
    }

    // Position getters with noexcept
    constexpr V center() const noexcept { return {pos.x+width/2, pos.y+height/2}; }
    constexpr V upperLeft(int inset = 0) const noexcept { return {pos.x+inset, pos.y+inset}; }
    constexpr V upperRight(int inset = 0) const noexcept { return {pos.x+width-1-inset, pos.y+inset}; }
    constexpr V lowerLeft(int inset = 0) const noexcept { return {pos.x+inset, pos.y+height-1-inset}; }
    constexpr V lowerRight(int inset = 0) const noexcept { return {pos.x+width-1-inset, pos.y+height-1-inset}; }
    constexpr V topCenter(int inset = 0) const noexcept { return {pos.x+width/2, pos.y+inset}; }
    constexpr V bottomCenter(int inset = 0) const noexcept { return {pos.x+width/2, pos.y+height-1-inset}; }
    constexpr V leftCenter(int inset = 0) const noexcept { return {pos.x+inset, pos.y+height/2}; }
    constexpr V rightCenter(int inset = 0) const noexcept { return {pos.x+width-1-inset, pos.y+height/2}; }

    // Static utility methods with improved parameter passing
    static int preferredLeftWidth(const RectBase<V>& leftRect, const RectBase<V>& rightRect,
                                  int desiredTotalWidth) noexcept;
    static int preferredTopHeight(const RectBase<V>& topRect, const RectBase<V>& bottomRect,
                                  int desiredTotalHeight) noexcept;

    static void setLeftWidth(RectBase<V>& leftRect, RectBase<V>& rightRect, int leftWidth);
    static void setTopHeight(RectBase<V>& topRect, RectBase<V>& bottomRect, int topHeight);

    static void setWidths(RectBase<V>& leftRect, RectBase<V>& rightRect,
                          int leftWidth, int rightWidth) noexcept;
    static void setHeights(RectBase<V>& topRect, RectBase<V>& bottomRect,
                           int topHeight, int bottomHeight) noexcept;

    static void setPosHPair(RectBase<V>& leftRect, RectBase<V>& rightRect,
                            const V& pos) noexcept;
    static void setPosVPair(RectBase<V>& topRect, RectBase<V>& bottomRect,
                            const V& pos) noexcept;

    static void scaleHorizontalPair(RectBase<V>& leftRect, RectBase<V>& rightRect,
                                    const V& newPos, int newTotalWidth, int newHeight) noexcept;
    static void scaleVerticalPair(RectBase<V>& topRect, RectBase<V>& bottomRect,
                                  const V& newPos, int newWidth, int newTotalHeight) noexcept;

    static void setSplitPointHorizontal(RectBase<V>& leftRect, RectBase<V>& rightRect, int rightStartX);
    static void setSplitPointVertical(RectBase<V>& topRect, RectBase<V>& bottomRect, int bottomStartY);

    // Add basic safety checks to existing methods
    constexpr bool overlaps(const RectBase<V>& other) const noexcept
    {
        if (width <= 0 || height <= 0 || other.width <= 0 || other.height <= 0) {
            return false;
        }

        if (toLeft(other.right()) ||
            toRight(other.left()) ||
            above(other.bottom()) ||
            below(other.top())) {
            return false;
        }
        return true;
    }

    constexpr bool empty() const noexcept {
        return width <= 0 || height <= 0;
    }

    // sets this rect to the intersection of this and other
    // return true if intersection is non-empty
    constexpr bool intersect(const RectBase<V>& other) noexcept
    {
        if (empty() || other.empty()) {
            width = 0;
            height = 0;
            return false;
        }

        int newLeft = std::max(left(), other.left());
        int newRight = std::min(right(), other.right());
        int newTop = std::max(top(), other.top());
        int newBottom = std::min(bottom(), other.bottom());

        if (newLeft > newRight || newTop > newBottom) {
            width = 0;
            height = 0;
            return false;
        }

        pos = {newLeft, newTop};
        width = newRight - newLeft + 1;
        height = newBottom - newTop + 1;
        return true;
    }

    constexpr void growDown(int amount) noexcept { height += amount; }
    constexpr void growUp(int amount) noexcept { pos.y -= amount; height += amount; }
    constexpr void growRight(int amount) noexcept { width += amount; }
    constexpr void growLeft(int amount) noexcept { pos.x -= amount; width += amount; }

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

    constexpr void shrink(int inset) noexcept {
        if (width <= 2 * inset || height <= 2 * inset) {
            // Center the empty rectangle
            int centerX = pos.x + width/2;
            int centerY = pos.y + height/2;
            pos.x = centerX;
            pos.y = centerY;
            width = 0;
            height = 0;
            return;
        }

        pos.x += inset;
        pos.y += inset;
        width -= inset*2;
        height -= inset*2;
    }

    constexpr void removeLeft(int amount) noexcept {
        amount = std::min(amount, width);
        pos.x += amount;
        width -= amount;
    }

    constexpr void removeRight(int amount) noexcept {
        width = std::max(0, width - amount);
    }

    constexpr void removeTop(int amount) noexcept {
        amount = std::min(amount, height);
        pos.y += amount;
        height -= amount;
    }

    constexpr void removeBottom(int amount) noexcept {
        height = std::max(0, height - amount);
    }

    constexpr bool widthDiffers(const RectBase<V>& other) const noexcept {
        return width != other.width;
    }

    constexpr bool heightDiffers(const RectBase<V>& other) const noexcept {
        return height != other.height;
    }

    constexpr bool sizeDiffers(const RectBase<V>& other) const noexcept {
        return widthDiffers(other) || heightDiffers(other);
    }

    constexpr bool sizeSame(const RectBase<V>& other) const noexcept {
        return !widthDiffers(other) && !heightDiffers(other);
    }

    constexpr void centerWithin(const RectBase<V>& container) noexcept {
        pos = container.center() - V{width/2, height/2};
    }

    // Subrect methods with noexcept
    constexpr RectBase<V> leftSubrect(int newWidth) const noexcept {
        return { pos, std::min(newWidth, width), height };
    }

    constexpr RectBase<V> rightSubrect(int newWidth) const noexcept {
        newWidth = std::min(newWidth, width);
        return { pos.x + width - newWidth, pos.y, newWidth, height };
    }

    constexpr RectBase<V> topSubrect(int newHeight) const noexcept {
        return { pos, width, std::min(newHeight, height) };
    }

    constexpr RectBase<V> bottomSubrect(int newHeight) const noexcept {
        newHeight = std::min(newHeight, height);
        return { pos.x, pos.y + height - newHeight, width, newHeight };
    }

    constexpr RectBase<V> leftSubrect(int newWidth, int inset) const noexcept {
        int safeWidth = std::max(0, width - 2 * inset);
        newWidth = std::min(newWidth, safeWidth);
        return { pos.x + inset, pos.y + inset, newWidth, std::max(0, height - 2 * inset) };
    }

    constexpr RectBase<V> rightSubrect(int newWidth, int inset) const noexcept {
        int safeWidth = std::max(0, width - 2 * inset);
        newWidth = std::min(newWidth, safeWidth);
        return { pos.x + width - newWidth - inset, pos.y + inset,
                newWidth, std::max(0, height - 2 * inset) };
    }

    constexpr RectBase<V> topSubrect(int newHeight, int inset) const noexcept {
        int safeHeight = std::max(0, height - 2 * inset);
        newHeight = std::min(newHeight, safeHeight);
        return { pos.x + inset, pos.y + inset, std::max(0, width - 2 * inset), newHeight };
    }

    constexpr RectBase<V> bottomSubrect(int newHeight, int inset) const noexcept {
        int safeHeight = std::max(0, height - 2 * inset);
        newHeight = std::min(newHeight, safeHeight);
        return { pos.x + inset, pos.y + height - newHeight - inset,
                std::max(0, width - 2 * inset), newHeight };
    }

    // Split operations with bounds checking
    constexpr RectBase<V> splitLeft(int leftWidth) noexcept;
    constexpr RectBase<V> splitRight(int rightWidth) noexcept;
    constexpr RectBase<V> splitTop(int topHeight) noexcept;
    constexpr RectBase<V> splitBottom(int bottomHeight) noexcept;
};

// Add noexcept to free functions
template<is2dVector V>
constexpr RectBase<V> intersect(const RectBase<V>& r1, const RectBase<V>& r2) noexcept
{
    if (r1.empty() || r2.empty()) {
        return {};
    }

    int newLeft = std::max(r1.left(), r2.left());
    int newRight = std::min(r1.right(), r2.right());
    int newTop = std::max(r1.top(), r2.top());
    int newBottom = std::min(r1.bottom(), r2.bottom());

    if (newLeft > newRight || newTop > newBottom) {
        return {};
    }

    return {newLeft, newTop, newRight-newLeft+1, newBottom-newTop+1};
}

// Implement split methods with bounds checking
template<is2dVector V>
constexpr RectBase<V> RectBase<V>::splitLeft(int leftWidth) noexcept
{
    leftWidth = std::min(leftWidth, width);
    RectBase<V> ret{pos, leftWidth, height};
    removeLeft(leftWidth);
    return ret;
}

template<is2dVector V>
constexpr RectBase<V> RectBase<V>::splitRight(int rightWidth) noexcept
{
    rightWidth = std::min(rightWidth, width);
    RectBase<V> ret{pos.x + width - rightWidth, pos.y, rightWidth, height};
    removeRight(rightWidth);
    return ret;
}

template<is2dVector V>
constexpr RectBase<V> RectBase<V>::splitTop(int topHeight) noexcept
{
    topHeight = std::min(topHeight, height);
    RectBase<V> ret{pos, width, topHeight};
    removeTop(topHeight);
    return ret;
}

template<is2dVector V>
constexpr RectBase<V> RectBase<V>::splitBottom(int bottomHeight) noexcept
{
    bottomHeight = std::min(bottomHeight, height);
    RectBase<V> ret{pos.x, pos.y + height - bottomHeight, width, bottomHeight};
    removeBottom(bottomHeight);
    return ret;
}

// Safe pair splitting operations
template<is2dVector V>
constexpr std::pair<RectBase<V>, RectBase<V>> splitLeft(const RectBase<V>& orig, int leftWidth) noexcept
{
    leftWidth = std::min(leftWidth, orig.width);
    return {{orig.pos, leftWidth, orig.height},
            {orig.pos+V{leftWidth,0}, orig.width-leftWidth, orig.height}};
}

template<is2dVector V>
constexpr std::pair<RectBase<V>, RectBase<V>> splitRight(const RectBase<V>& orig, int rightWidth) noexcept
{
    rightWidth = std::min(rightWidth, orig.width);
    return {{orig.pos, orig.width-rightWidth, orig.height},
            {orig.pos+V{orig.width-rightWidth,0}, rightWidth, orig.height}};
}

template<is2dVector V>
constexpr std::pair<RectBase<V>, RectBase<V>> splitTop(const RectBase<V>& orig, int topHeight) noexcept
{
    topHeight = std::min(topHeight, orig.height);
    return {{orig.pos, orig.width, topHeight},
            {orig.pos+V{0,topHeight}, orig.width, orig.height-topHeight}};
}

template<is2dVector V>
constexpr std::pair<RectBase<V>, RectBase<V>> splitBottom(const RectBase<V>& orig, int bottomHeight) noexcept
{
    bottomHeight = std::min(bottomHeight, orig.height);
    return {{orig.pos, orig.width, orig.height-bottomHeight},
            {orig.pos+V{0,orig.height-bottomHeight}, orig.width, bottomHeight}};
}

// Static utility methods implementation
template<is2dVector V>
int RectBase<V>::preferredLeftWidth(const RectBase<V>& leftRect, const RectBase<V>& rightRect,
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
int RectBase<V>::preferredTopHeight(const RectBase<V>& topRect, const RectBase<V>& bottomRect,
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
inline void RectBase<V>::setWidths(RectBase<V>& leftRect,
                                   RectBase<V>& rightRect,
                                   int leftWidth,
                                   int rightWidth) noexcept
{
    leftRect.width = leftWidth;
    rightRect.width = rightWidth;
    rightRect.pos.x = leftRect.right()+1;
}

template<is2dVector V>
inline void RectBase<V>::setHeights(RectBase<V>& topRect,
                                    RectBase<V>& bottomRect,
                                    int topHeight,
                                    int bottomHeight) noexcept
{
    topRect.height = topHeight;
    bottomRect.height = bottomHeight;
    bottomRect.pos.y = topRect.bottom()+1;
}

template<is2dVector V>
inline void RectBase<V>::setPosHPair(RectBase<V>& leftRect, RectBase<V>& rightRect,
                                     const V& pos) noexcept
{
    leftRect.pos = pos;
    rightRect.pos = {leftRect.right()+1, pos.y};
}

template<is2dVector V>
inline void RectBase<V>::setPosVPair(RectBase<V>& topRect, RectBase<V>& bottomRect,
                                     const V& pos) noexcept
{
    topRect.pos = pos;
    bottomRect.pos = {pos.x, topRect.bottom()+1};
}

template<is2dVector V>
inline void RectBase<V>::scaleHorizontalPair(RectBase<V>& leftRect, RectBase<V>& rightRect,
                                             const V& newPos, int newTotalWidth, int newHeight) noexcept
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
inline void RectBase<V>::scaleVerticalPair(RectBase<V>& topRect, RectBase<V>& bottomRect,
                                           const V& newPos, int newWidth, int newTotalHeight) noexcept
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
void RectBase<V>::setSplitPointHorizontal(RectBase<V>& leftRect,
                                          RectBase<V>& rightRect,
                                          int rightStartX)
{
    leftRect.width = rightStartX - leftRect.pos.x;
    int rightEndX = rightRect.right(-1);
    rightRect.pos.x = rightStartX;
    rightRect.width = rightEndX - rightStartX;
}

template<is2dVector V>
void RectBase<V>::setSplitPointVertical(RectBase<V>& topRect,
                                        RectBase<V>& bottomRect,
                                        int bottomStartY)
{
    topRect.height = bottomStartY - topRect.pos.y;
    int bottomEndY = bottomRect.bottom(-1);
    bottomRect.pos.y = bottomStartY;
    bottomRect.height = bottomEndY - bottomStartY;
}

// Region identification method implementation
template<is2dVector V>
RectRegion RectBase<V>::region(int x, int y, int inset, int outset) const noexcept
{
    if (toLeft(x, -outset) ||
        toRight(x, -outset) ||
        above(y, -outset) ||
        below(y, -outset)) {
        return RectRegion::outside;
    }

    if (toLeft(x, inset)) {
        if (above(y, inset)) {
            return RectRegion::ulCorner;
        }
        if (below(y, inset)) {
            return RectRegion::llCorner;
        }
        return RectRegion::leftBorder;
    }

    if (toRight(x, inset)) {
        if (above(y, inset)) {
            return RectRegion::urCorner;
        }
        if (below(y, inset)) {
            return RectRegion::lrCorner;
        }
        return RectRegion::rightBorder;
    }

    if (above(y, inset)) {
        return RectRegion::topBorder;
    }

    if (below(y, inset)) {
        return RectRegion::bottomBorder;
    }

    return RectRegion::content;
}

using RectI = RectBase<Vec2i32>;

#endif // RECTI_H
