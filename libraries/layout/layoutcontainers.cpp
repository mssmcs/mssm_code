#include "layoutcontainers.h"
#include "layoutwidgets.h"
#include <numeric>
#include "rectdraw.h"

using namespace mssm;

LayoutStacked::LayoutStacked(Private privateTag,
                             LayoutContext *context,
                             bool isHorizontal,
                             Justify justify,
                             CrossJustify crossJustify,
                             std::vector<LayoutPtr> children)
    : LayoutWithChildren<LayoutPtr>{context}
    , isHorizontal{isHorizontal}
    , justify{justify}
    , crossJustify{crossJustify}
{
    margins.hBetween = 3;
    margins.vBetween = 3;
    for (auto &c : children) {
        this->children.push_back(c);
    }
    setParentsOfChildren();
}

void LayoutStacked::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    pushClip(g, thisRect(), false);
    drawRect(g, thisRect(), YELLOW, TRANSPARENT);
    foreachChild([&g,&parentProps](auto *c) { c->draw(parentProps, g); }, ForeachContext::drawing, false, false);
    popClip(g);
}

SizeBound2d LayoutStacked::getBound(const PropertyBag& parentProps)
{
    if (children.empty()) {
        throw std::logic_error("empty children in getBound");
    }
    if (isHorizontal) {
        auto b = children[0]->getBound(parentProps);
        for (int i = 1; i < children.size(); i++) {
            b = hStack(b, children[i]->getBound(parentProps));
        }
        grow(b, margins, children.size() - 1, 0);
        return b;
    } else {
        auto b = children[0]->getBound(parentProps);
        for (int i = 1; i < children.size(); i++) {
            b = vStack(b, children[i]->getBound(parentProps));
        }
        grow(b, margins, 0, children.size() - 1);
        return b;
    }
}

// returns vector of sizes and vector of offsets as pair
std::pair<std::vector<int>, std::vector<int>> LayoutMain(Justify justify,
                                                         std::vector<SizeBound> bounds,
                                                         int growIdx,
                                                         int firstMargin,
                                                         int lastMargin,
                                                         int betweenSize,
                                                         int availLength)
{
    int numBetweens = bounds.size() - 1;

    int marginSpace = firstMargin + lastMargin + numBetweens * betweenSize;

    int toDistribute = std::max(0, availLength - marginSpace);

    auto sizes = distributeSizes(bounds, toDistribute, justify == Justify::stretch, growIdx);

    int totalSize = std::accumulate(sizes.begin(), sizes.end(), 0);
    //int remainingSpace = std::max(0, toDistribute - totalSize);

    // now we have all the sizes in the stack, we can resize the children

    // |_#####_#####_#####_____________|   Begin
    // |_____________#####_#####_#####_|   End
    // | #########_##########_########_|   Stretch
    // |____#####____#####____#####____|   Evenly
    // |___####______#####______####___|   Around
    // |_####________#####________####_|   Between

    std::vector<int> offsets(sizes.size());
    double offsetAccum{0};
    double offsetIncr{0};

    switch (justify) {
    case Justify::begin:
    case Justify::stretch:
        offsetAccum = firstMargin;
        offsets[0] = offsetAccum;
        for (int i = 1; i < sizes.size(); i++) {
            offsetAccum += sizes[i - 1] + betweenSize;
            offsets[i] = offsetAccum;
        }
        break;
    case Justify::end:
        offsetAccum = availLength - marginSpace + firstMargin - totalSize;
        offsets[0] = offsetAccum;
        for (int i = 1; i < sizes.size(); i++) {
            offsetAccum += sizes[i - 1] + betweenSize;
            offsets[i] = offsetAccum;
        }
        break;
    case Justify::center:
        offsetAccum = (availLength - totalSize - betweenSize * numBetweens) / 2.0;
        offsets[0] = offsetAccum;
        for (int i = 1; i < sizes.size(); i++) {
            offsetAccum += sizes[i - 1] + betweenSize;
            offsets[i] = offsetAccum;
        }
        break;
    case Justify::spaceAround:
        offsetIncr = (availLength - totalSize) / (sizes.size() * 2);
        offsetAccum = offsetIncr;
        offsets[0] = offsetAccum;
        for (int i = 1; i < sizes.size(); i++) {
            offsetAccum += sizes[i - 1] + offsetIncr * 2;
            offsets[i] = offsetAccum;
        }
        break;
    case Justify::spaceBetween:
        offsetIncr = (availLength - totalSize - firstMargin - lastMargin) / (sizes.size() - 1);
        offsetAccum = firstMargin;
        offsets[0] = offsetAccum;
        for (int i = 1; i < sizes.size(); i++) {
            offsetAccum += sizes[i - 1] + offsetIncr;
            offsets[i] = offsetAccum;
        }
        break;
    case Justify::spaceEvenly:
        offsetIncr = (availLength - totalSize) / (sizes.size() + 1);
        offsetAccum = offsetIncr;
        offsets[0] = offsetAccum;
        for (int i = 1; i < sizes.size(); i++) {
            offsetAccum += sizes[i - 1] + offsetIncr;
            offsets[i] = offsetAccum;
        }
        break;
    }

    return {sizes, offsets};
}

// returns vector of sizes and vector of offsets as pair
std::pair<std::vector<int>, std::vector<int>> LayoutCross(CrossJustify justify,
                                                          std::vector<SizeBound> bounds,
                                                          int firstMargin,
                                                          int lastMargin,
                                                          int availLength)
{
    int marginSpace = firstMargin + lastMargin;

    int extraSpace = std::max(0, availLength - marginSpace);

    // |_###############_____________|   Begin
    // |_____________###############_|   End
    // |_______##############________|   Center
    // |_###########################_|   Stretch

    std::vector<int> sizes(bounds.size());
    std::vector<int> offsets(bounds.size());

    double offsetAccum{0};
    double offsetIncr{0};

    switch (justify) {
    case CrossJustify::begin:
        for (int i = 0; i < bounds.size(); i++) {
            sizes[i] = bounds[i].minSize;
            offsets[i] = firstMargin;
        }
        break;
    case CrossJustify::stretch:
        for (int i = 0; i < bounds.size(); i++) {
            sizes[i] = extraSpace;
            offsets[i] = firstMargin;
        }
        break;
    case CrossJustify::end:
        for (int i = 0; i < bounds.size(); i++) {
            sizes[i] = bounds[i].minSize;
            offsets[i] = std::max(firstMargin, availLength - sizes[i] - lastMargin);
        }
        break;
    case CrossJustify::center:
        for (int i = 0; i < bounds.size(); i++) {
            sizes[i] = bounds[i].minSize;
            int maxOffset = availLength - sizes[i] - lastMargin;
            offsets[i] = std::max(firstMargin, std::min(maxOffset, (availLength - sizes[i]) / 2));
        }
        break;
    }

    return {sizes, offsets};
}

void LayoutStacked::resize(const PropertyBag& parentProps, const RectI &newRect)
{
    std::vector<SizeBound> bounds;
    std::vector<SizeBound> crossBounds;

    for (auto &c : children) {
        auto bound = c->getBound(parentProps);
        crossBounds.push_back(isHorizontal ? bound.yBound : bound.xBound);
        bounds.push_back(isHorizontal ? bound.xBound : bound.yBound);
    }

    //    int crossSpace{};
    int firstMargin{};
    int lastMargin{};
    int availLength{};
    int betweenSize{};
    int firstCrossMargin{};
    int lastCrossMargin{};
    int availCrossLength{};

    if (isHorizontal) {
        betweenSize = margins.hBetween;
        firstMargin = margins.left;
        lastMargin = margins.right;
        firstCrossMargin = margins.top;
        lastCrossMargin = margins.bottom;
        availLength = newRect.width;
        availCrossLength = newRect.height;
        //        crossSpace = std::max(0, newRect.height-margins.vSum(0));
    } else {
        betweenSize = margins.vBetween;
        firstMargin = margins.top;
        lastMargin = margins.bottom;
        firstCrossMargin = margins.left;
        lastCrossMargin = margins.right;
        availLength = newRect.height;
        availCrossLength = newRect.width;
        //        crossSpace = std::max(0, newRect.width-margins.hSum(0));
    }

    auto [sizes, offsets]
        = LayoutMain(justify, bounds, growIdx, firstMargin, lastMargin, betweenSize, availLength);
    auto [crossSizes, crossOffsets] = LayoutCross(crossJustify,
                                                  crossBounds,
                                                  firstCrossMargin,
                                                  lastCrossMargin,
                                                  availCrossLength);

    if (isHorizontal) {
        Vec2i32 startPos = newRect.pos;

        for (int i = 0; i < children.size(); i++) {
            Vec2i32 newPos = startPos + Vec2i32{offsets[i], crossOffsets[i]};
            children[i]->resize(parentProps, {newPos, sizes[i], crossSizes[i]});
        }
    } else {
        Vec2i32 startPos = newRect.pos;
        for (int i = 0; i < children.size(); i++) {
            Vec2i32 newPos = startPos + Vec2i32{crossOffsets[i], offsets[i]};
            children[i]->resize(parentProps, {newPos, crossSizes[i], sizes[i]});
        }
    }

    setRect(newRect);
}

LayoutSplitter::LayoutSplitter(Private privateTag,
                               LayoutContext *context,
                               bool isHorizontal,
                               LayoutPtr first,
                               LayoutPtr second)
    : LayoutWithChildren<LayoutPtr>{context}
    , isHorizontal{isHorizontal}
{
    // dragHandle = LayoutDragHandle::make(context,
    //                                     [this](LayoutDragHandle::DragState dragState, Vec2d pos)
    //                                     { this->onDragHandle(dragState, pos); });

    margins.hBetween = 3;
    margins.vBetween = 3;

    children.push_back(first);
    children.push_back(second);

    setParentsOfChildren();
}

SizeBound2d LayoutSplitter::getBound(const PropertyBag& parentProps)
{
    if (isHorizontal) {
        SizeBound2d bound = hStack(children[0]->getBound(parentProps), children[1]->getBound(parentProps));
        bound.xBound.grow(margins.hSum(1));
        bound.yBound.grow(margins.vSum(0));
        return bound;
    } else {
        SizeBound2d bound = vStack(children[0]->getBound(parentProps), children[1]->getBound(parentProps));
        bound.xBound.grow(margins.hSum(0));
        bound.yBound.grow(margins.vSum(1));
        return bound;
    }
}

int preferredSize(int oldFirst, int oldSecond, int desiredTotal)
{
    double oldTotal = std::max(0, oldFirst + oldSecond);
    if (oldTotal == 0) {
        return desiredTotal / 2;
    }
    double scale = (double) desiredTotal / oldTotal;
    return scale * std::max(0, oldFirst);
}

void LayoutSplitter::resize(const PropertyBag& parentProps, const RectI &newRect)
{
    if (isHorizontal) {
        bool widthChanged = newRect.widthDiffers(*this);
        RectI left = children[0]->thisRect();
        RectI right = children[1]->thisRect();
        int spaceH = std::max(0, newRect.width - margins.hSum(1));
        // calc preferred size of left element, accounting for margins
        int requestSize = left.width;
        if (desiredSplitPos >= 0) {
            requestSize = desiredSplitPos;
        }
        if (widthChanged) {
            requestSize = preferredSize(left.width, right.width, spaceH);
            desiredSplitPos = requestSize;
        }
        splitPos = setFirstSize(parentProps, newRect, requestSize);
    } else {
        bool heightChanged = newRect.heightDiffers(*this);
        auto top = children[0]->thisRect();
        auto bottom = children[1]->thisRect();
        int spaceV = std::max(0, newRect.height - margins.vSum(1));
        // calc preferred size of left element, accounting for margins
        int requestSize = top.height;
        if (desiredSplitPos >= 0) {
            requestSize = desiredSplitPos;
        }
        if (heightChanged) {
            requestSize = preferredSize(top.height, bottom.height, spaceV);
            desiredSplitPos = requestSize;
        }
        splitPos = setFirstSize(parentProps, newRect, requestSize);
    }
}

void LayoutSplitter::dragSplit(Vec2d pos)
{
    if (isHorizontal) {
        auto left = children[0]->thisRect();
        desiredSplitPos = std::max(0.0, pos.x - left.pos.x); // TODO: account for margins?
    } else {
        auto top = children[0]->thisRect();
        desiredSplitPos = std::max(0.0, pos.y - top.pos.y); // TODO: account for margins?
    }
    context->setNeedsResize();
}

LayoutBase::EvtRes LayoutSplitter::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    RectI splitHandle = splitHandleRect();

    //dragHandle->onMouse(reason, evt);
    const int dragThreshold = 10;

    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::scroll:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::move:
        hovering = splitHandle.within(evt.pos);
        break;
    case MouseEvt::Action::drag:
        if (hasDragFocus()) {
            hovering = true;
            if (evt.dragMax > dragThreshold) {
                dragSplit(evt.pos);
            }
        }
        break;
    case MouseEvt::Action::press:
        if (splitHandle.within(evt.pos)) {
            hovering = true;
            grabMouse();
        }
        break;
    case MouseEvt::Action::release:
        if (hasDragFocus()) {
            releaseMouse();
        }
        break;
    case MouseEvt::Action::exit:
        if (!hasDragFocus()) {
            hovering = false;
        }
        break;
    }

    return EvtRes::propagate;
}

void LayoutSplitter::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    if (hovering) {
        if (isHorizontal) {
            context->setCursor(mssm::CoreWindowCursor::hresize);
            double x = getSplitPos();
            double y1 = pos.y + margins.top;
            double y2 = pos.y + height - margins.bottom;
            g.line({x, y1}, {x, y2}, hasDragFocus() ? mssm::YELLOW : mssm::WHITE);
        } else {
            context->setCursor(mssm::CoreWindowCursor::vresize);
            double y = getSplitPos();
            double x1 = pos.x + margins.left;
            double x2 = pos.x + width - margins.right;
            g.line({x1, y}, {x2, y}, hasDragFocus() ? mssm::YELLOW : mssm::WHITE);
        }
    }

    pushClip(g, thisRect(), false);
    foreachChild([&g,&parentProps](auto *c) { c->draw(parentProps, g); }, ForeachContext::drawing, false, false);
    popClip(g);
}

RectI LayoutSplitter::splitHandleRect() const
{
    int radius = 5;
    RectI handle;
    if (isHorizontal) {
        handle.pos.x = getSplitPos() - radius;
        handle.pos.y = pos.y;
        handle.width = 2 * radius;
        handle.height = height;
    } else {
        handle.pos.x = pos.x;
        handle.pos.y = getSplitPos() - radius;
        handle.width = width;
        handle.height = 2 * radius;
    }
    return handle;
}

// void LayoutSplitter::onDragHandle(LayoutDragHandle::DragState state, Vec2d pos)
// {
//     if (state == LayoutDragHandle::DragState::dragging) {
//         dragSplit(pos);
//     }
// }

int LayoutSplitter::setFirstSize(const PropertyBag& parentProps, const RectI &newRect, int newSize)
{
    int retPos{};

    if (isHorizontal) {
        RectI left = children[0]->thisRect();
        RectI right = children[1]->thisRect();

        auto leftBounds = children[0]->getBound(parentProps);
        auto rightBounds = children[1]->getBound(parentProps);

        int spaceH = std::max(0, newRect.width - margins.hSum(1));

        // calc preferred size of left element, accounting for margins
        int newLeftWidth = newSize;

        // allow splitter to be dragged even if one of the children has a max size
        leftBounds.xBound.maxSize = SizeBound::maxV;
        rightBounds.xBound.maxSize = SizeBound::maxV;

        if (!limit(leftBounds.xBound, rightBounds.xBound, newLeftWidth, spaceH)) {
            std::cout << "Uh Oh bounds problem!!" << std::endl;
            //return;
        }

        int newRightWidth = rightBounds.xBound.constrain(spaceH - newLeftWidth);

        Vec2i32 p = newRect.pos;
        p.x += margins.left;
        p.y += margins.top;
        left.pos = p;
        left.width = newLeftWidth;
        p.x += newLeftWidth + margins.hBetween;
        right.pos = p;
        right.width = newRightWidth;

        int spaceV = std::max(0, newRect.height - margins.vSum(0));

        left.height = spaceV;  // leftBounds.yBound.constrain(spaceV);
        right.height = spaceV; // rightBounds.yBound.constrain(spaceV);

        // the width that brings us to the center of the splitter (or just to the left of center if hBetween margin is even size)
        retPos = left.right() + margins.hBetween / 2 + margins.hBetween % 2;

        children[0]->resize(parentProps, left);
        children[1]->resize(parentProps, right);
    } else {
        auto top = children[0]->thisRect();
        auto bottom = children[1]->thisRect();
        auto topBounds = children[0]->getBound(parentProps);
        auto bottomBounds = children[1]->getBound(parentProps);

        int spaceV = std::max(0, newRect.height - margins.vSum(1));

        int newTopHeight = newSize;

        // allow splitter to be dragged even if one of the children has a max size
        topBounds.yBound.maxSize = SizeBound::maxV;
        bottomBounds.yBound.maxSize = SizeBound::maxV;

        if (!limit(topBounds.yBound, bottomBounds.yBound, newTopHeight, spaceV)) {
            std::cout << "Uh Oh bounds problem!!" << std::endl;
            //return;
        }

        int newBottomHeight = bottomBounds.yBound.constrain(spaceV - newTopHeight);

        Vec2i32 p = newRect.pos;
        p.x += margins.left;
        p.y += margins.top;
        top.pos = p;
        top.height = newTopHeight;
        p.y += newTopHeight + margins.vBetween;
        bottom.pos = p;
        bottom.height = newBottomHeight;

        int spaceH = std::max(0, newRect.width - margins.hSum(0));

        top.width = spaceH;    // topBounds.xBound.constrain(spaceH);
        bottom.width = spaceH; // bottomBounds.xBound.constrain(spaceH);

        // the height that brings us to the center of the splitter (or just to the top of center if vBetween margin is even size)
        retPos = top.bottom() + margins.vBetween / 2 + margins.vBetween % 2;

        children[0]->resize(parentProps, top);
        children[1]->resize(parentProps, bottom);
    }

    width = newRect.width;
    height = newRect.height;
    pos = newRect.pos;

    return retPos;
}

LayoutGridRow::LayoutGridRow(Private privateTag,
                             LayoutContext *context,
                             std::vector<LayoutPtr> children)
    : LayoutWithChildren<LayoutPtr>{context}
{
    for (auto &c : children) {
        this->children.push_back(c);
    }
    setParentsOfChildren();
}

void LayoutGridRow::resize(const PropertyBag& parentProps, const RectI& rect)
{
    throw std::logic_error("Should call resize with column widths instead of this method");
}

void LayoutGridRow::resize(const PropertyBag& parentProps, const RectI &newRect,
                           std::vector<int> columnOffsets,
                           std::vector<int> columnWidths)
{
    // TODO: constrain rect heights by each child's max height?
    auto kidPos = newRect.pos;
    for (int i = 0; i < children.size(); i++) {
        kidPos.x += columnOffsets[i];
        children[i]->resize(parentProps, {kidPos, columnWidths[i], newRect.height});
        kidPos.x += columnWidths[i];
    }
    width = newRect.width;
    height = newRect.height;
    pos = newRect.pos;
}

void LayoutGridRow::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    pushClip(g, thisRect(), false);
    foreachChild([&g,&parentProps](auto *c) { c->draw(parentProps, g); }, ForeachContext::drawing, false, false);
    popClip(g);
}

SizeBound2d LayoutGridRow::getBound(const PropertyBag& parentProps)
{
    if (children.empty()) {
        throw std::logic_error("empty children in getBound");
    } else {
        auto b = children[0]->getBound(parentProps);
        for (int i = 1; i < children.size(); i++) {
            b = hStack(b, children[i]->getBound(parentProps));
        }
        return b;
    }
}

LayoutGrid::LayoutGrid(Private privateTag,
                       LayoutContext *context,
                       std::vector<LayoutGridRowPtr> rows)
    : LayoutWithChildren<LayoutGridRowPtr>{context}
{
    margins.hBetween = 5;
    margins.vBetween = 5;

    for (auto &r : rows) {
        // r->parent = this;

        if (!numColumns) {
            numColumns = r->numColumns();
        } else if (numColumns != r->numColumns()) {
            throw std::runtime_error("Inconsistent number of columns");
        }
        children.push_back(r);
    }
    setParentsOfChildren();
}

void LayoutGrid::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    pushClip(g, thisRect(), false);
    foreachChild([&g,&parentProps](auto *c) { c->draw(parentProps, g); }, ForeachContext::drawing, false, false);
    popClip(g);
}

void LayoutGrid::resize(const PropertyBag& parentProps, const RectI &newRect)
{
    getBound(parentProps); // force recalc columnbounds, etc?

    int spaceH = std::max(0, newRect.width - margins.hSum(numColumns - 1));
    int spaceV = std::max(0, newRect.height - margins.vSum(children.size() - 1));

    columnWidths = distributeSizes(columnBounds, spaceH, true, -1);
    rowHeights = distributeSizes(rowBounds, spaceV, true, -1);

    std::vector<int> columnOffsets;

    columnOffsets.push_back(margins.left);
    for (int i = 0; i < numColumns - 1; i++) {
        columnOffsets.push_back(margins.hBetween);
    }

    auto childPos = newRect.pos;

    childPos.y += margins.top;

    int rowNum = 0;

    for (auto &r : children) {
        r->resize(parentProps, {childPos, spaceH, rowHeights[rowNum]}, columnOffsets, columnWidths);
        childPos.y += rowHeights[rowNum] + margins.vBetween;
        rowNum++;
    }

    width = newRect.width;
    height = newRect.height;
    pos = newRect.pos;
}

SizeBound2d LayoutGrid::getBound(const PropertyBag& parentProps)
{
    columnBounds.clear();
    columnBounds.resize(numColumns);
    rowBounds.clear();

    SizeBound heightBound;

    for (auto &r : children) {
        auto rowBound = r->getBound(parentProps);
        rowBounds.push_back(rowBound.yBound);
        heightBound = heightBound + rowBound.yBound;
        for (int i = 0; i < numColumns; i++) {
            auto cb = r->children[i]->getBound(parentProps).xBound;
            columnBounds[i] = constraintIntersectionSafe(columnBounds[i], cb);
        }
    }

    SizeBound widthBound = sum(columnBounds);

    widthBound.grow(margins.hSum(numColumns - 1));
    heightBound.grow(margins.vSum(children.size() - 1));

    return {widthBound, heightBound};
}
