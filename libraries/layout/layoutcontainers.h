#ifndef LAYOUTCONTAINERS_H
#define LAYOUTCONTAINERS_H

#include <vector>
#include "layoutcore.h"
#include "layoutwidgets.h"

template <typename CHILD>
class LayoutWithChildren : public LayoutBase
{
protected:
    std::vector<CHILD> children;
    Margins margins;
protected:
    LayoutWithChildren(LayoutContext* context) : LayoutBase(context) {}
public:
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    void setOuterMargins(int left, int right, int top, int bottom) override;
    void setInnerMargins(int hBetween, int vBetween) override;
    int numChildren() const { return children.size(); }
    void appendChild(CHILD child);
    void insertChild(CHILD child, int index);
    void removeChild(int index);
};

class LayoutSplitter : public LayoutWithChildren<LayoutPtr>
{
public: // TODO: make private
    bool isHorizontal{true};
    int splitPos; // width/height of first sub panel
    bool hovering{false};
    int desiredSplitPos{-1};
public:
    LayoutSplitter(Private privateTag, LayoutContext* context, bool isHorizontal, LayoutPtr first, LayoutPtr second);
    static std::shared_ptr<LayoutSplitter> make(LayoutContext* context, bool isHorizontal, LayoutPtr first, LayoutPtr second) { return std::make_shared<LayoutSplitter>(Private{}, context, isHorizontal, first, second); }
    std::string getTypeStr() const override { return "Splitter"; }
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void dragSplit(Vec2d pos);
    EvtRes onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    int getSplitPos() const { return splitPos; }
    RectI splitHandleRect() const;
private:
    int setFirstSize(const PropertyBag &parentProps, const RectI& newRect, int newSize); // returns width of first panel (plus left/top margin plus half of between margin)
};

enum class Justify {
    begin,
    end,
    center,
    stretch,
    spaceAround,
    spaceBetween,
    spaceEvenly
};

enum class CrossJustify {
    begin,
    end,
    center,
    stretch,
};

class LayoutStacked : public LayoutWithChildren<LayoutPtr> {
public:
    int growIdx{-1};  // index of child to grow to fill space
    bool isHorizontal{true};
    Justify justify{Justify::begin};
    CrossJustify crossJustify{CrossJustify::begin};
    int offset{0};
public:
    LayoutStacked(Private privateTag, LayoutContext *context, bool isHorizontal, Justify justify, CrossJustify crossJustify, std::vector<LayoutPtr> children);
    static std::shared_ptr<LayoutStacked> make(LayoutContext* context, bool isHorizontal, Justify justify, CrossJustify crossJustify, std::vector<LayoutPtr> children) { return std::make_shared<LayoutStacked>(Private{}, context, isHorizontal, justify, crossJustify, children); }
    std::string getTypeStr() const override { return "Stacked"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
};


template<typename CHILD>
void LayoutWithChildren<CHILD>::foreachChildImpl(std::function<void(LayoutBase *)> f,
                                             ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    for (auto& c : children) {
        f(c.get());
    }
}

template<typename CHILD>
void LayoutWithChildren<CHILD>::setOuterMargins(int left, int right, int top, int bottom)
{
    margins.left = left;
    margins.right = right;
    margins.top = top;
    margins.bottom = bottom;
}

template<typename CHILD>
void LayoutWithChildren<CHILD>::setInnerMargins(int hBetween, int vBetween)
{
    margins.hBetween = hBetween;
    margins.vBetween = vBetween;
}


template<typename CHILD>
void LayoutWithChildren<CHILD>::appendChild(CHILD child)
{
    children.append(child);
}

template<typename CHILD>
void LayoutWithChildren<CHILD>::insertChild(CHILD child, int index)
{
    children.insert(children.begin() + index, child);
}

template<typename CHILD>
void LayoutWithChildren<CHILD>::removeChild(int index)
{
    children.erase(children.begin()+index);
}



class LayoutGrid;
class LayoutGridRow;
using LayoutGridRowPtr = std::shared_ptr<LayoutGridRow>;

class LayoutGridRow : public LayoutWithChildren<LayoutPtr> {
public:
    LayoutGridRow(Private privateTag, LayoutContext* context, std::vector<LayoutPtr> children);
    static LayoutGridRowPtr make(LayoutContext* context, std::vector<LayoutPtr> children) { return std::make_shared<LayoutGridRow>(Private{}, context, children); }
    std::string getTypeStr() const override { return "GridRow"; }
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    virtual void resize(const PropertyBag &parentProps, const RectI& rect, std::vector<int> columnOffsets, std::vector<int> columnWidths);
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    int numColumns() const { return children.size(); }

    SizeBound2d getBound(const PropertyBag& parentProps) override;

    friend class LayoutGrid;
};

class LayoutGrid : public LayoutWithChildren<LayoutGridRowPtr> {
public:
    int numColumns{};
    std::vector<SizeBound> columnBounds;
    std::vector<SizeBound> rowBounds;
    std::vector<int> columnWidths;
    std::vector<int> rowHeights;
public:
    LayoutGrid(Private privateTag, LayoutContext* context, std::vector<LayoutGridRowPtr> rows);
    static std::shared_ptr<LayoutGrid> make(LayoutContext* context, std::vector<LayoutGridRowPtr> rows) { return std::make_shared<LayoutGrid>(Private{}, context, rows); }
    std::string getTypeStr() const override { return "Grid"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;

    SizeBound2d getBound(const PropertyBag& parentProps) override;
};


#endif // LAYOUTCONTAINERS_H
