#ifndef LAYOUTCORE_H
#define LAYOUTCORE_H

#include "canvas2d.h"
#include "layoutcontext.h"
#include "propertybag.h"
#include "recti.h"
#include "sizebound.h"

class Padding {
public:
    int top;
    int bottom;
    int left;
    int right;
public:
    Padding(int pad = 0) : top{pad}, bottom{pad}, left{pad}, right{pad} {}
    Padding(int tb, int lr) : top{tb}, bottom{tb}, left{lr}, right{lr} {}
    Padding(int t, int b, int l, int r) : top{t}, bottom{b}, left{l}, right{r} {}
};

class Margins : public Padding {
public:
    int hBetween;
    int vBetween;
public:    
    Margins(int all = 0) : Padding(all), hBetween{all}, vBetween{all} {}
    Margins(int around, int between) : Padding(around), hBetween{between}, vBetween{between} {}
    Margins(int tb, int lr, int between) : Padding(tb, lr), hBetween{between}, vBetween{between} {}
    Margins(int tb, int lr, int hBetween, int vBetween) : Padding(tb, lr), hBetween{hBetween}, vBetween{vBetween} {}
    Margins(int t, int b, int l, int r, int hBetween, int vBetween) : Padding(t, b, l, r), hBetween{hBetween}, vBetween{vBetween} {}
    int hSum(int betweenCount) const { return left + right + betweenCount*hBetween; }
    int vSum(int betweenCount) const { return top + bottom + betweenCount*vBetween; }
    Vec2i32 ul() const { return {left, top}; }
};

Vec2d textPos(const RectI& rect, HAlign hAlign, VAlign vAlign);


/**
 * @brief Grow a rectangle by adding padding on all sides.
 *
 * @tparam V Vector type used in the rectangle
 * @param rect Rectangle to modify
 * @param pad Padding to add on all sides
 */
template<is2dVector V>
void grow(RectBase<V> &rect, const Padding &pad)
{
    rect.pos.x -= pad.left;
    rect.pos.y -= pad.top;
    rect.width += pad.left + pad.right;
    rect.height += pad.top + pad.bottom;
}

/**
 * @brief Shrink a rectangle by adding padding on all sides.
 *
 * @tparam V Vector type used in the rectangle
 * @param rect Rectangle to modify
 * @param pad Padding to remove from all sides
 */
template<is2dVector V>
void shrink(RectBase<V> &rect, const Padding &pad)
{
    rect.pos.x += pad.left;
    rect.pos.y += pad.top;
    rect.width -= pad.left + pad.right;
    rect.height -= pad.top + pad.bottom;
    if (rect.width < 0) {
        rect.pos.x += rect.width / 2;
        rect.width = 0;
    }
    if (rect.height < 0) {
        rect.pos.y += rect.height / 2;
        rect.height = 0;
    }
}

/**
 * @brief Create a grown rectangle by adding padding on all sides.
 *
 * @tparam V Vector type used in the rectangle
 * @param rect Original rectangle
 * @param pad Padding to add on all sides
 * @return A new grown rectangle
 */
template<is2dVector V>
[[nodiscard]] RectBase<V> grown(const RectBase<V> &rect, const Padding &pad)
{
    RectBase<V> ret = rect;
    grow(ret, pad);
    return ret;
}

/**
 * @brief Create a shrunk rectangle by removing padding from all sides.
 *
 * @tparam V Vector type used in the rectangle
 * @param rect Original rectangle
 * @param pad Padding to remove from all sides
 * @return A new shrunk rectangle
 */
template<is2dVector V>
[[nodiscard]] RectBase<V> shrunk(const RectBase<V> &rect, const Padding &pad)
{
    RectBase<V> ret = rect;
    shrink(ret, pad);
    return ret;
}

void grow(SizeBound2d& bound, const Padding& pad);
void grow(SizeBound2d& bound, const Margins& margins, int hBetweenCount, int vBetweenCount);

// Over
// IsOver NotGrabbed Grabbed(NotSelf) Grabbed(Self) Click Move Release
//   T        T                                        T
//   T                     T                           T
//   T                                     T           T

//   F        T                                        T
//   F                     T                           T
//   F                                     T           T

//   T        T                                             T
//   T                     T                                T
//   T                                     T                T

//   F        T                                             T
//   F                     T                                T
//   F                                     T                T

//   T        T                                                  T
//   T                     T                                     T
//   T                                     T                     T

//   F        T                                                  T
//   F                     T                                     T
//   F                                     T                     T

// How parents and children work:
// foreachChild iterates direct children
// traversePreOrder and traversePostOrder iterate all descendants
// all children have parents
// overlays (menus, tooltips) are not children, but they can have parents as well
// overlays can have children.  The "root" overlay has isOverlay set to true
// the parent of an overlay is the element that created it.  That parent is responsible
// for managing the lifetime of the overlay

// Secondary iteration mechanism for overlays?
// does each element have a link to potential overlay?  wasteful?


class LayoutBase : public RectI, public std::enable_shared_from_this<LayoutBase> {
protected:
    class Private {};
public:
    enum class BroadcastMessage {
        closeTooltip
    };
    enum class EvtProp {
        normal,
        hover,
    };
    enum class EvtRes {
        consumed,
        hover,
        propagate
    };
    enum class ForeachContext {
        drawing,
        events,
        other
    };

protected:
    std::string typeStr;
    std::string name;
    int id;
    static int nextId;
    LayoutContext* context{};
    LayoutBase* parent{};

    LayoutPtr overlayElement{}; // tooltip, menu, context menu, etc
private:
    bool isKeyFocusElement{false};
    bool isDragFocusElement{false};
    bool containsKeyFocusElement{false};
    bool containsDragFocusElement{false};
    bool showingTooltip{false};
    bool isCollapsed{false};

    int layer{0};  // 0 is base layer, 1+ are overlays
    int depth{0};  // 0 for base layer root or for roots of overlays
protected:
    LayoutBase(LayoutContext* context) : context{context} { id = nextId++; }
public:
    virtual ~LayoutBase() = default;
    virtual std::string getTypeStr() const = 0;
    virtual void setParent(LayoutBase* parent);
    std::string getName() const { return name; }
    void setName(std::string nm) { name = nm; }
    void updateLayer(int layer, int depth);
    int getLayer() const { return layer; }
    int getDepth() const { return depth; }
    bool isOverlayRoot() const { return layer > 0 && depth == 0; }
    virtual void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) = 0;
    constexpr RectI thisRect() const { return *this; }
    RectRegion region(Vec2d pos, int innerBorder, int outerBorder) const { return RectI::region(pos.x, pos.y, innerBorder, outerBorder); }
    virtual SizeBound2d getBound(const PropertyBag& parentProps) = 0;
    virtual void resize(const PropertyBag& parentProps, const RectI& rect) = 0;

    virtual void onSetKeyFocus();
    virtual void onClearKeyFocus();
    virtual void onSetDragFocus();
    virtual void onClearDragFocus();

    bool hasOverlay() const { return overlayElement != nullptr; }
    LayoutPtr getOverlay() const { return overlayElement; }

    constexpr bool hasKeyFocus() const { return isKeyFocusElement; }
    constexpr bool hasDragFocus() const { return isDragFocusElement; }

    double distance(Vec2d pos);

    std::shared_ptr<LayoutBase> getBasePtr() { return shared_from_this();}

    std::shared_ptr<LayoutBase> getParentPtr() {
        if (parent) {
            return parent->getBasePtr();
        }
        return {};
    }

    std::shared_ptr<LayoutBase> getLocalRoot();

    virtual void foreachChild(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed);
    virtual void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) = 0;

    inline void foreachAncestor(std::function<void(LayoutBase *)> f)
    {
        LayoutBase *curr = this;
        while (curr->depth > 0) {
            curr = curr->parent;
            f(curr);
        }
    }

    LayoutPtr findByName(std::string name);

    void traversePreOrder(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed);
    void traversePostorder(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed);
    void setParentsOfChildren();

    void broadcastRecursive(BroadcastMessage message);

    virtual void setOuterMargins(int borders);
    virtual void setOuterMargins(int leftRight, int topBottom);
    virtual void setOuterMargins(int left, int right, int top, int bottom);
    virtual void setInnerMargins(int hBetween, int vBetween);
    virtual void setInnerMargins(int between);

    void setRect(const RectI& other) {
        pos = other.pos;
        width = other.width;
        height = other.height;
    }

    void grabKeyboard();
    void releaseKeyboard();
    void grabMouse();
    void releaseMouse();

    void localReleaseMouseAndKeyboard();

    bool isAnyKeyboardFocus() const { return context->isAnyKeyboardFocus(); }
    bool isAnyDragFocus() const { return context->isAnyDragFocus(); }

    bool canLeftClick() const { return !isAnyDragFocus(); }
    void setCursor(mssm::CoreWindowCursor cursor) { context->setCursor(cursor); }

    mssm::CoreWindowCursor getCursor() const { return context->getCursor(); }

    void pushClip(mssm::Canvas2d& g, const RectI& rect, bool replace);
    void popClip(mssm::Canvas2d& g);

    virtual EvtRes propagateMouse(const PropertyBag& parentProps, EvtProp prop, const RectI& clip, MouseEvt& evt);
    virtual EvtRes onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt& evt);
    virtual EvtRes onMouseDeferred(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt& evt);

    virtual EvtRes propagateKey(const PropertyBag& parentProps, EvtProp prop, const RectI& clip, KeyEvt& evt);
    virtual EvtRes onKey(const PropertyBag& parentProps, const KeyEvt& evt);
    virtual EvtRes onKeyDeferred(const PropertyBag& parentProps, const KeyEvt& evt);

    virtual void debugDraw(mssm::Canvas2d& g);

    void setOverlay(LayoutPtr overlay);
    void closeOverlay(); // if this element hosts an overlay, close it
    void closeOverlayRecursive();  // close any overlays on this element or any descendents

    void addToolTip(const PropertyBag &parentProps, Vec2d pos, LayoutPtr tip);

    std::string trail() const;

    bool isShowingTooltip() const { return showingTooltip; }

    bool isLocalRoot() const { return depth == 0; }

    void setCollapsed(bool collapsed);
    bool getCollapsed() const { return isCollapsed; }
};

std::string to_string(RectRegion region);

#endif // LAYOUTCORE_H
