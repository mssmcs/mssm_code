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
    constexpr RectBase() : pos{0,0}, width{0}, height{0} {}
    template <is2dVector VV>
    constexpr RectBase(VV pos, int width, int height) : pos{cast<VV>(pos)}, width{width}, height{height} {}
    constexpr RectBase(V pos, int width, int height) : pos{pos}, width{width}, height{height} {}
    constexpr RectBase(int x, int y, int width, int height) : pos{x,y}, width{width}, height{height} {}

    constexpr int left(int inset = 0) const { return pos.x+inset; }
    constexpr int right(int inset = 0) const { return pos.x+width-1-inset; }
    constexpr int top(int inset = 0) const { return pos.y+inset; }
    constexpr int bottom(int inset = 0) const { return pos.y+height-1-inset; }

    constexpr bool toLeft(int x, int inset = 0) const { return x < left(inset); }
    constexpr bool toRight(int x, int inset = 0) const { return x > right(inset); }
    constexpr bool above(int y, int inset = 0) const { return y < top(inset); }
    constexpr bool below(int y, int inset = 0) const { return y > bottom(inset); }

    constexpr bool withinVertical(int y, int inset = 0) const { return !above(y, inset) && !below(y, inset); }
    constexpr bool withinHorizontal(int x, int inset = 0) const { return !toLeft(x, inset) && !toRight(x, inset); }

    template <is2dVector VV>
    constexpr bool within(VV p, int inset = 0) const { return withinHorizontal(p.x, inset) && withinVertical(p.y, inset);  }

    constexpr bool withinLeftMargin(int x, int y, int inset = 0, int outset = 0) const
    {
        return withinVertical(y, 0) && x >= left(-outset) && x <= left(inset);
    }

    constexpr bool withinLeftMargin(V p, int inset = 0, int outset = 0) const
    {
        return withinLeftMargin(p.x, p.y, inset, outset);
    }

    constexpr bool withinRightMargin(int x, int y, int inset = 0, int outset = 0) const
    {
        return withinVertical(y, 0) && x >= right(inset) && x <= right(-outset);
    }

    template <is2dVector VV>
    constexpr bool withinRightMargin(VV p, int inset = 0, int outset = 0) const
    {
        return withinRightMargin(p.x, p.y, inset, outset);
    }

    constexpr bool withinTopMargin(int x, int y, int inset = 0, int outset = 0) const
    {
        return withinHorizontal(x, 0) && y >= top(-outset) && y <= top(inset);
    }

    template <is2dVector VV>
    constexpr bool withinTopMargin(VV p, int inset = 0, int outset = 0) const
    {
        return withinTopMargin(p.x, p.y, inset, outset);
    }

    constexpr bool withinBottomMargin(int x, int y, int inset = 0, int outset = 0) const
    {
        return withinHorizontal(x, 0) && y >= bottom(inset) && y <= bottom(-outset);
    }

    template <is2dVector VV>
    constexpr bool withinBottomMargin(VV p, int inset = 0, int outset = 0) const
    {
        return withinBottomMargin(p.x, p.y, inset, outset);
    }

    RectRegion region(int x, int y, int inset, int outset) const;

    template <is2dVector VV>
    RectRegion region(VV p, int inset, int outset) const { return region(p.x, p.y, inset, outset); }

    constexpr V center() const { return {pos.x+width/2, pos.y+height/2}; }

    constexpr V upperLeft(int inset = 0) const { return {pos.x+inset, pos.y+inset}; }
    constexpr V upperRight(int inset = 0) const { return {pos.x+width-1-inset, pos.y+inset}; }
    constexpr V lowerLeft(int inset = 0) const { return {pos.x+inset, pos.y+height-1-inset}; }
    constexpr V lowerRight(int inset = 0) const { return {pos.x+width-1-inset, pos.y+height-1-inset}; }

    constexpr V topCenter(int inset = 0) const { return {pos.x+width/2, pos.y+inset}; }
    constexpr V bottomCenter(int inset = 0) const { return {pos.x+width/2, pos.y+height-1-inset}; }
    constexpr V leftCenter(int inset = 0) const { return {pos.x+inset, pos.y+height/2}; }
    constexpr V rightCenter(int inset = 0) const { return {pos.x+width-1-inset, pos.y+height/2}; }

    static int preferredLeftWidth(RectBase<V>& leftRect, RectBase<V>& rightRect, int desiredTotalWidth);
    static int preferredTopHeight(RectBase<V>& topRect, RectBase<V>& bottomRect, int desiredTotalHeight);

    static void setLeftWidth(RectBase<V>& leftRect, RectBase<V>& rightRect, int leftWidth);
    static void setTopHeight(RectBase<V>& topRect, RectBase<V>& bottomRect, int topHeight);

    static void setWidths(RectBase<V>& leftRect, RectBase<V>& rightRect, int leftWidth, int rightWidth);
    static void setHeights(RectBase<V>& topRect, RectBase<V>& bottomRect, int topHeight, int bottomHeight);

    static void setPosHPair(RectBase<V>& leftRect, RectBase<V>& rightRect, V pos);
    static void setPosVPair(RectBase<V>& topRect, RectBase<V>& bottomRect, V pos);

    static void scaleHorizontalPair(RectBase<V>& leftRect, RectBase<V>& rightRect, V newPos, int newTotalWidth, int newHeight);
    static void scaleVerticalPair(RectBase<V>& topRect, RectBase<V>& bottomRect, V newPos, int newWidth, int newTotalHeight);

    static void setSplitPointHorizontal(RectBase<V>& leftRect, RectBase<V>& rightRect, int rightStartX);
    static void setSplitPointVertical(RectBase<V>& topRect, RectBase<V>& bottomRect, int bottomStartY);

    constexpr bool overlaps(const RectBase<V>& other) const
    {
        if (toLeft(other.right()) ||
            toRight(other.left()) ||
            above(other.bottom()) ||
            below(other.top())) {
            return false;
        }
        return true;
    }

    constexpr bool empty() const {
        return width == 0 || height == 0;
    }

    // sets this rect to the intersection of this and other
    // return true if intersection is non-empty
    constexpr bool intersect(const RectBase<V>& other)
    {
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

    constexpr void growDown(int amount) { height += amount; }
    constexpr void growUp(int amount) { pos.y -= amount; height += amount; }
    constexpr void growRight(int amount) { width += amount; }
    constexpr void growLeft(int amount) { pos.x -= amount; width += amount; }

    constexpr RectBase<V> makeInset(int inset) const;

    constexpr void shrink(int inset);

    constexpr void removeLeft(int amount);
    constexpr void removeRight(int amount);
    constexpr void removeTop(int amount);
    constexpr void removeBottom(int amount);

    constexpr bool widthDiffers(const RectBase<V>& other) const { return width != other.width; }
    constexpr bool heightDiffers(const RectBase<V>& other) const { return height != other.height; }
    constexpr bool sizeDiffers(const RectBase<V>& other) const { return widthDiffers(other) || heightDiffers(other); }
    constexpr bool sizeSame(const RectBase<V>& other) const { return !widthDiffers(other) && !heightDiffers(other); }

    constexpr void centerWithin(const RectBase<V>& container) { pos = container.center() - V{width/2, height/2}; }

    constexpr RectBase<V> leftSubrect(int newWidth) const { return { pos, newWidth, height }; }
    constexpr RectBase<V> rightSubrect(int newWidth) const { return { pos.x + width - newWidth, pos.y, newWidth, height }; }
    constexpr RectBase<V> topSubrect(int newHeight) const { return { pos, width, newHeight }; }
    constexpr RectBase<V> bottomSubrect(int newHeight) const { return { pos.x, pos.y + height - newHeight, width, newHeight }; }

    constexpr RectBase<V> leftSubrect(int newWidth, int inset) const { return { pos.x+inset, pos.y+inset, newWidth, height-2*inset }; }
    constexpr RectBase<V> rightSubrect(int newWidth, int inset) const { return { pos.x + width - newWidth-inset, pos.y+inset, newWidth, height-2*inset }; }
    constexpr RectBase<V> topSubrect(int newHeight, int inset) const { return { pos.x+inset, pos.y+inset, width-2*inset, newHeight }; }
    constexpr RectBase<V> bottomSubrect(int newHeight, int inset) const { return { pos.x+inset, pos.y + height - newHeight-inset, width-2*inset, newHeight }; }

    constexpr RectBase<V> splitLeft(int leftWidth); // returns left subrect and resizes original to be the remaining right side
    constexpr RectBase<V> splitRight(int rightWidth); // returns right subrect and resizes original to be the remaining left side
    constexpr RectBase<V> splitTop(int topHeight); // returns top subrect and resizes original to be the remaining bottom side
    constexpr RectBase<V> splitBottom(int bottomHeight); // returns bottom subrect and resizes original to be the remaining top side
};

template<is2dVector V>
constexpr RectBase<V> intersect(const RectBase<V>& r1, const RectBase<V>& r2)
{
    int newLeft = std::max(r1.left(), r2.left());
    int newRight = std::min(r1.right(), r2.right());
    int newTop = std::max(r1.top(), r2.top());
    int newBottom = std::min(r1.bottom(), r2.bottom());
    if (newLeft > newRight || newTop > newBottom) {
        return {};
    }
    return {newLeft, newTop, newRight-newLeft+1, newBottom-newTop+1};
}

template<is2dVector V>
constexpr RectBase<V> RectBase<V>::splitLeft(int leftWidth)
{
    RectBase<V> ret{pos, leftWidth, height};
    removeLeft(leftWidth);
    return ret;
}

template<is2dVector V>
constexpr RectBase<V> RectBase<V>::splitRight(int rightWidth)
{
    RectBase<V> ret{pos.x + width - rightWidth, pos.y, rightWidth, height};
    removeRight(rightWidth);
    return ret;
}

template<is2dVector V>
constexpr RectBase<V> RectBase<V>::splitTop(int topHeight)
{
    RectBase<V> ret{pos, width, topHeight};
    removeTop(topHeight);
    return ret;
}

template<is2dVector V>
constexpr RectBase<V> RectBase<V>::splitBottom(int bottomHeight)
{
    RectBase<V> ret{pos.x, pos.y + height - bottomHeight, width, bottomHeight};
    removeBottom(bottomHeight);
    return ret;
}

template<is2dVector V>
constexpr std::pair<RectBase<V>, RectBase<V>> splitLeft(const RectBase<V>& orig, int leftWidth) // returns left and right subrects
{
    return {{orig.pos, leftWidth, orig.height}, {orig.pos+V{leftWidth,0}, orig.width-leftWidth, orig.height}};
}

template<is2dVector V>
constexpr std::pair<RectBase<V>, RectBase<V>> splitRight(const RectBase<V>& orig, int rightWidth) // returns left and right subrects
{
    return {{orig.pos, orig.width-rightWidth, orig.height}, {orig.pos+V{orig.width-rightWidth}, rightWidth, orig.height}};
}

template<is2dVector V>
constexpr std::pair<RectBase<V>, RectBase<V>> splitTop(const RectBase<V>& orig, int topHeight) // returns top and bottom subrects
{
    return {{orig.pos, orig.width, topHeight}, {orig.pos+V{0,topHeight}, orig.width, orig.height-topHeight}};
}

template<is2dVector V>
constexpr std::pair<RectBase<V>, RectBase<V>> splitBottom(const RectBase<V>& orig, int bottomHeight) // returns top and bottom subrects
{
    return {{orig.pos, orig.width, orig.height-bottomHeight}, {orig.pos+V{0,orig.height-bottomHeight}, orig.width, bottomHeight}};
}

template<is2dVector V>
int RectBase<V>::preferredLeftWidth(RectBase<V> &leftRect, RectBase<V> &rightRect, int desiredTotalWidth)
{
    double oldTotalWidth = leftRect.width + rightRect.width;
    if (oldTotalWidth == 0) {
        return desiredTotalWidth/2;
    }
    double scalex = (double)desiredTotalWidth / oldTotalWidth;
    return scalex * leftRect.width;
}

template<is2dVector V>
int RectBase<V>::preferredTopHeight(RectBase<V> &topRect, RectBase<V> &bottomRect, int desiredTotalHeight)
{
    double oldTotalHeight = topRect.height + bottomRect.height;
    if (oldTotalHeight == 0) {
        return oldTotalHeight/2;
    }
    double scaley = (double)desiredTotalHeight / oldTotalHeight;
    return scaley * topRect.height;
}

template<is2dVector V>
void RectBase<V>::setLeftWidth(RectBase<V> &leftRect, RectBase<V> &rightRect, int leftWidth)
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
void RectBase<V>::setTopHeight(RectBase<V> &topRect, RectBase<V> &bottomRect, int topHeight)
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
inline void RectBase<V>::setWidths(RectBase<V> &leftRect,
                                   RectBase<V> &rightRect,
                                   int leftWidth,
                                   int rightWidth)
{
    leftRect.width = leftWidth;
    rightRect.width = rightWidth;
    rightRect.pos.x = leftRect.right()+1;
}

template<is2dVector V>
inline void RectBase<V>::setHeights(RectBase<V> &topRect,
                                    RectBase<V> &bottomRect,
                                    int topHeight,
                                    int bottomHeight)
{
    topRect.height = topHeight;
    bottomRect.height = bottomHeight;
    bottomRect.pos.y = topRect.bottom()+1;
}

template<is2dVector V>
inline void RectBase<V>::setPosHPair(RectBase<V> &leftRect, RectBase<V> &rightRect, V pos)
{
    leftRect.pos = pos;
    rightRect.pos = {leftRect.right()+1, pos.y};
}

template<is2dVector V>
inline void RectBase<V>::setPosVPair(RectBase<V> &topRect, RectBase<V> &bottomRect, V pos)
{
    topRect.pos = pos;
    bottomRect.pos = {pos.x, topRect.bottom()+1};
}

template<is2dVector V>
inline void RectBase<V>::scaleHorizontalPair(RectBase<V> &leftRect, RectBase<V> &rightRect, V newPos, int newTotalWidth, int newHeight)
{
    int newLeftWidth = preferredLeftWidth(leftRect, rightRect, newTotalWidth);
   //  double oldTotalWidth = leftRect.width + rightRect.width;
   //  double scalex = (double)newTotalWidth / oldTotalWidth;
   // // int proposedLeftWidth = scalex * leftRect.width;

    leftRect.pos = newPos;
    leftRect.width = newLeftWidth;
    leftRect.height = newHeight;
    rightRect.width = newTotalWidth - leftRect.width;
    rightRect.height = newHeight;
    rightRect.pos.x = leftRect.right()+1;
    rightRect.pos.y = newPos.y;
}

template<is2dVector V>
inline void RectBase<V>::scaleVerticalPair(RectBase<V> &topRect, RectBase<V> &bottomRect, V newPos, int newWidth, int newTotalHeight)
{
    int newTopHeight = preferredTopHeight(topRect, bottomRect, newTotalHeight);


    // double oldTotalHeight = topRect.height + bottomRect.height;
    // double scaley = (double)newTotalHeight / oldTotalHeight;
    topRect.pos = newPos;
    topRect.height = newTopHeight;
    topRect.width = newWidth;
    bottomRect.height = newTotalHeight - topRect.height;
    bottomRect.width = newWidth;
    bottomRect.pos.x = newPos.x;
    bottomRect.pos.y = topRect.bottom()+1;
}

template<is2dVector V>
constexpr RectBase<V> RectBase<V>::makeInset(int inset) const
{
    RectBase<V> result;
    result.pos = { pos.x + inset, pos.y + inset };
    result.width = width - inset*2;
    result.height = height - inset*2;
    return result;
}

template<is2dVector V>
constexpr void RectBase<V>::shrink(int inset)
{
    pos.x += inset;
    pos.y += inset;
    width -= inset*2;
    height -= inset*2;
}

template<is2dVector V>
constexpr void RectBase<V>::removeLeft(int amount)
{
    pos.x += amount;
    width -= amount;
}

template<is2dVector V>
constexpr void RectBase<V>::removeRight(int amount)
{
    width -= amount;
}

template<is2dVector V>
constexpr void RectBase<V>::removeTop(int amount)
{
    pos.y += amount;
    height -= amount;
}

template<is2dVector V>
constexpr void RectBase<V>::removeBottom(int amount)
{
    height -= amount;
}

template<is2dVector V>
RectRegion RectBase<V>::region(int x, int y, int inset, int outset) const
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

template<is2dVector V>
void RectBase<V>::setSplitPointHorizontal(RectBase<V> &leftRect,
                                       RectBase<V> &rightRect,
                                       int rightStartX)
{
    leftRect.width = rightStartX - leftRect.pos.x;
    int rightEndX = rightRect.right(-1);
    rightRect.pos.x = rightStartX;
    rightRect.width = rightEndX - rightStartX;
}

template<is2dVector V>
void RectBase<V>::setSplitPointVertical(RectBase<V> &topRect,
                                     RectBase<V> &bottomRect,
                                     int bottomStartY)
{
    topRect.height = bottomStartY - topRect.pos.y;
    int bottomEndY = bottomRect.bottom(-1);
    bottomRect.pos.y = bottomStartY;
    bottomRect.height = bottomEndY - bottomStartY;
}

using RectI = RectBase<Vec2i32>;





#endif // RECTI_H
