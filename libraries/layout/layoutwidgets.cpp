#include "layoutwidgets.h"
#include "rectdraw.h"
#include "vec3d.h"

using namespace mssm;

LayoutLabel::LayoutLabel(Private privateTag, LayoutContext *context, std::string text, const FontInfo &sizeAndFace)
    : LayoutWithContent{context}, text{text}, sizeAndFace{sizeAndFace}
{
    hAlign = HAlign::center;
    vAlign = VAlign::center;

    setParentsOfChildren();
}

LayoutLabel::~LayoutLabel()
{
    std::cout << "Deleting label: " << text << std::endl;
}

void LayoutLabel::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    Vec2d tPos = textPos(contentRect, hAlign, vAlign);
    if (borderColor.a > 0 || backgroundColor.a > 0) {
        drawRect(g, contentRect, borderColor, backgroundColor);
    }
    g.text(tPos, sizeAndFace, text, textColor, hAlign, vAlign);
}

SizeBound2d LayoutLabel::getBound(const PropertyBag& parentProps)
{
    TextExtents extents;
    context->textExtents(sizeAndFace, text, extents);
    SizeBound2d bound{SizeBound(extents.textWidth), SizeBound(extents.fontHeight, extents.fontHeight)};
    grow(bound, padding);
    return bound;

//    return {SizeBound(extents.textWidth+padding.left+padding.right), SizeBound(extents.fontHeight + , extents.fontHeight + 4)};
}

void LayoutLabel::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
    contentRect = rect;
   // ::shrink(contentRect, padding);
}

void LayoutLabel::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}

LayoutColor::LayoutColor(Private privateTag, LayoutContext *context, mssm::Color color, SizeBound2d bound)
    : LayoutWithContent{context}, color{color}, bound{bound}
{
    setParentsOfChildren();
}

LayoutColor::~LayoutColor()
{
    std::cout << "Deleting Color: " << color.toRealVec3<Vec3d>() << std::endl;
}

void LayoutColor::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    drawRoundedRect(g, *this, 0, 10, light(color), color);
}

SizeBound2d LayoutColor::getBound(const PropertyBag& parentProps)
{
    return bound;
}

void LayoutColor::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
    contentRect.pos = rect.pos;
    contentRect.width = bound.xBound.constrain(rect.width);
    contentRect.height = bound.yBound.constrain(rect.height);
}

void LayoutColor::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}

LayoutBase::EvtRes LayoutColor::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    if (evt.action == MouseEvt::Action::press) {
        auto label = LayoutLabel::make(context, "ToolTip Example");
        label->backgroundColor = mssm::BLACK;
        label->borderColor = mssm::GREY;
        addToolTip(parentProps, evt.pos, label);
        return LayoutBase::EvtRes::consumed;
    }
    return LayoutBase::EvtRes::propagate;
}

LayoutButtonBase::LayoutButtonBase(Private privateTag, LayoutContext *context, ButtonType buttonType)
    : LayoutBase{context}, type{buttonType}
{
}

void LayoutButtonBase::onButtonPress(const PropertyBag& parentProps, bool pressValue)
{

}

void LayoutButtonBase::setChecked(bool newChecked)
{
    if (checked == newChecked) {
        return;
    }
    checked = newChecked;
}

LayoutBase::EvtRes LayoutButtonBase::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::scroll:
        break;
    case MouseEvt::Action::enter:
    case MouseEvt::Action::move:
        mode = DrawMode::hover;
        break;
    case MouseEvt::Action::drag:
        if (evt.insideElement) {
            mode = DrawMode::pressing;
        }
        else {
            mode = DrawMode::normal;
        }
        break;
    case MouseEvt::Action::press:
        mode = DrawMode::pressing;
        grabMouse();
        if (type == ButtonType::checkbox) {
            checked = !checked;
            onButtonPress(parentProps, checked);
        }
        else if (type == ButtonType::radio) {
            checked = true;
            onButtonPress(parentProps, checked);
        }
        break;
    case MouseEvt::Action::release:
        if (evt.insideElement) {
            if (hasDragFocus() && type == ButtonType::normal) {
                onButtonPress(parentProps, true);
            }
            mode = DrawMode::hover;
        }
        else {
            mode = DrawMode::normal;
        }
        if (hasDragFocus()) {
            releaseMouse();
        }
        break;
    case MouseEvt::Action::exit:
        mode = DrawMode::normal;
        break;
    }

    return EvtRes::propagate;
}

LayoutButton::LayoutButton(Private privateTag,
                           LayoutContext *context,
                           LayoutPtr child,
                           LayoutButton::ButtonType buttonType,
                           ButtonCallback callback, int buttonId)
  : LayoutButtonBase{privateTag, context, buttonType},
    child{child},
    callback{callback},
    buttonId{buttonId}
{
    setParentsOfChildren();
}

void LayoutButton::onButtonPress(const PropertyBag& parentProps, bool pressValue)
{
    if (callback) {
        callback(parentProps, std::static_pointer_cast<LayoutButton>(shared_from_this()), buttonId, pressValue);
    }
}

void LayoutButton::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    switch (type) {
    case ButtonType::normal:
        drawMomentary(parentProps, g, mode);
        break;
    case ButtonType::checkbox:
        drawCheckbox(parentProps, g, mode);
        break;
    case ButtonType::radio:
        drawRadio(parentProps, g, mode);
        break;
    }
}

void LayoutButton::drawCommon(const PropertyBag &parentProps, mssm::Canvas2d &g, mssm::Color color)
{
    RectI buttonRect = thisRect();


    switch (style) {
    case DrawStyle::button:
        if (roundRadius > 0) {
            drawRoundedRect(g, buttonRect, 0, roundRadius, light(color), color);
        }
        else {
            drawRect(g, buttonRect, light(color), color);
        }
        break;
    case DrawStyle::tabTop:
        buttonRect.growDown(roundRadius);
        drawRoundedRect(g, buttonRect, 0, roundRadius, light(color), color);
        break;
    case DrawStyle::tabBottom:
        buttonRect.growUp(roundRadius);
        drawRoundedRect(g, buttonRect, 0, roundRadius, light(color), color);
        break;
    case DrawStyle::tabLeft:
        buttonRect.growRight(roundRadius);
        drawRoundedRect(g, buttonRect, 0, roundRadius, light(color), color);
        break;
    case DrawStyle::tabRight:
        buttonRect.growLeft(roundRadius);
        drawRoundedRect(g, buttonRect, 0, roundRadius, light(color), color);
        break;
    case DrawStyle::menu:
        drawRect(g, *this, light(color), color);
        break;
    }

    drawRect(g, buttonRect, RED, TRANSPARENT);
}


void LayoutButton::drawMomentary(const PropertyBag &parentProps, mssm::Canvas2d &g, DrawMode mode)
{
    mssm::Color color;

    switch (mode) {
    case DrawMode::normal:
        color = rgb(40, 40, 40);
        break;
    case DrawMode::hover:
        color = rgb(70, 70, 70);
        break;
    case DrawMode::pressing:
        color = rgb(101, 101, 101);
        break;
    }

    drawCommon(parentProps, g, color);

    if (child) {
        child->draw(parentProps, g);
    }
}

void LayoutButton::drawCheckbox(const PropertyBag &parentProps, mssm::Canvas2d &g, DrawMode mode)
{
    mssm::Color color;

    if (checked) {
        color = rgb(101, 101, 101);
    }
    else {
        switch (mode) {
        case DrawMode::normal:
            color = rgb(40, 40, 40);
            break;
        case DrawMode::hover:
            color = rgb(70, 70, 70);
            break;
        case DrawMode::pressing:
            color = rgb(101, 101, 101);
            break;
        }
    }

    drawCommon(parentProps, g, color);

    child->draw(parentProps, g);
}

void LayoutButton::drawRadio(const PropertyBag &parentProps, mssm::Canvas2d &g, DrawMode mode)
{
    mssm::Color color;

    if (checked) {
        color = rgb(101, 101, 101);
    }
    else {
        switch (mode) {
        case DrawMode::normal:
            color = rgb(40, 40, 40);
            break;
        case DrawMode::hover:
            color = rgb(70, 70, 70);
            break;
        case DrawMode::pressing:
            color = rgb(101, 101, 101);
            break;
        }
    }

    drawCommon(parentProps, g, color);

    child->draw(parentProps, g);
}


SizeBound2d LayoutButton::getBound(const PropertyBag& parentProps)
{
    auto bound = child->getBound(parentProps);
    bound.grow(roundRadius/2,roundRadius/2);
    return bound;
}

void LayoutButton::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
    if (child) {
        child->resize(parentProps, rect.makeInset(roundRadius/2));
    }
}

void LayoutButton::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    if (child) {
        f(child.get());
    }
}


LayoutText::LayoutText(Private privateTag, LayoutContext *context, std::string text)
    : LayoutWithContent{context}, editBox{*context, context->defaultFont(), text}
{
    setParentsOfChildren();
}

LayoutBase::EvtRes LayoutText::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::exit:
        break;
    case MouseEvt::Action::move:
        break;
    case MouseEvt::Action::drag:
        if (hasDragFocus()) {
            editBox.onDrag(evt.pos);
        }
        break;
    case MouseEvt::Action::press:
        if (editBox.onClick(evt.pos)) {
            grabKeyboard();
            grabMouse();
        }
        break;
    case MouseEvt::Action::release:
        if (hasDragFocus()) {
            releaseMouse();
        }
        break;
    case MouseEvt::Action::scroll:
        break;
    }

    return EvtRes::propagate;
}

LayoutBase::EvtRes LayoutText::onKey(const PropertyBag &parentProps, const KeyEvt &key)
{
    switch (key.action) {
    case KeyEvt::Action::press:
    case KeyEvt::Action::repeat:
        if (hasKeyFocus()) {
            if (std::isprint(key.key)) {
                editBox.addChar(key.key, key.hasShift());
                return EvtRes::consumed;
            }
            else {
                switch (key.key) {
                case Key::Left:
                    editBox.onLeft(key.hasShift());
                    break;
                case Key::Right:
                    editBox.onRight(key.hasShift());
                    break;
                case Key::Delete:
                    editBox.onDelete();
                    break;
                case Key::Backspace:
                    editBox.onBackspace();
                    break;
                }
            }
        }
        break;
    case KeyEvt::Action::release:
        break;
    }
    return EvtRes::propagate;
}

void LayoutText::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    editBox.draw(g, hasKeyFocus());
}

SizeBound2d LayoutText::getBound(const PropertyBag& parentProps)
{
    return {SizeBound(editBox.boxHeight()*2), SizeBound(editBox.boxHeight(), editBox.boxHeight())};
}

void LayoutText::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
    auto bound = getBound(parentProps);
    contentRect.pos = rect.pos;
    contentRect.width = bound.xBound.constrain(rect.width);
    contentRect.height = bound.yBound.constrain(rect.height);
    editBox.setRect({{contentRect.pos.x, contentRect.pos.y}, contentRect.width, contentRect.height});
}

void LayoutText::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}

LayoutScroll::LayoutScroll(Private privateTag, LayoutContext *context, LayoutPtr child)
    : LayoutWithContent{context}, child{child}
{
    hScroll = LayoutSlider::make(context, true, 0, 0, 0);
    vScroll = LayoutSlider::make(context, false, 0, 0, 0);
    setParentsOfChildren();
}

LayoutBase::EvtRes LayoutScroll::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{/*
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::move:
    case MouseEvt::Action::drag:
    case MouseEvt::Action::press:
    case MouseEvt::Action::release:
    case MouseEvt::Action::exit:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::scroll:
        return EvtProp::defer;
        // if (!vScroll->within(evt.pos) && !hScroll->within(evt.pos)) {
        //     vScroll->applyWheel(evt.dragDelta.y);
        //     context->setNeedsResize();
        // }
        break;
    }*/

    return EvtRes::propagate;
}

LayoutBase::EvtRes LayoutScroll::onMouseDeferred(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::move:
    case MouseEvt::Action::drag:
    case MouseEvt::Action::press:
    case MouseEvt::Action::release:
    case MouseEvt::Action::exit:
    case MouseEvt::Action::enter:

        break;
    case MouseEvt::Action::scroll:
        if (!vScroll->within(evt.pos) && !hScroll->within(evt.pos)) {
            vScroll->applyWheel(evt.dragDelta.y);
            context->setNeedsResize();
            std::cout << "scrolling" << std::endl;
            return EvtRes::consumed;
        }
        break;
    }

    return EvtRes::propagate;
}

void LayoutScroll::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    pushClip(g, thisRect(), false);

    if (extraX) {
        hScroll->draw(parentProps, g);
    }

    if (extraY) {
        vScroll->draw(parentProps, g);
    }

    constexpr const int scrollGutter = 2;
    int vScrollWidth = vScroll->width ? vScroll->width + scrollGutter : 0;
    int hScrollHeight = hScroll->height ? hScroll->height + scrollGutter : 0;
    g.pushClip(left(), top(), std::max(0,width-vScrollWidth), std::max(0,height-hScrollHeight), false);
    child->draw(parentProps, g);
    popClip(g);
}

SizeBound2d LayoutScroll::getBound(const PropertyBag& parentProps)
{
    return {SizeBound(0), SizeBound(0)};
}

void LayoutScroll::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);

    xScroll = hScroll->value;
    yScroll = vScroll->value;
    contentRect.pos = rect.pos-Vec2i32{xScroll, yScroll};

    auto childBound = child->getBound(parentProps);

    contentRect.width = childBound.xBound.minSize;
    contentRect.height = childBound.yBound.minSize;

    // compute what doesn't fit into rect
    extraX = contentRect.width - rect.width;
    extraY = contentRect.height - rect.height;

    if (extraX > 0) {
        extraX += barSize;
        xScroll = std::max(0, std::min(xScroll, extraX));
    }
    else {
        extraX = 0;
        xScroll = 0;
    }

    if (extraY > 0) {
        extraY += barSize;
        yScroll = std::max(0, std::min(yScroll, extraY));
    }
    else {
        extraY = 0;
        yScroll = 0;
    }

    if (extraX) {
        auto bar = bottomSubrect(barSize, 2);
        if (extraY) {
            bar.width -= barSize;
        }
        hScroll->resize(parentProps, bar);
    }
    else {
        hScroll->height = 0;
    }

    if (extraY) {
        auto bar = rightSubrect(barSize, 2);
        if (extraX) {
            bar.height -= barSize;
        }
        vScroll->resize(parentProps, bar);
    }
    else {
        vScroll->width = 0;
    }

    hScroll->proportion = (double(rect.width-vScroll->width)/contentRect.width);
    hScroll->minValue = 0;
    hScroll->maxValue = extraX;
    hScroll->value = xScroll;

    vScroll->proportion = (double(rect.height-hScroll->height)/contentRect.height);
    vScroll->minValue = 0;
    vScroll->maxValue = extraY;
    vScroll->value = yScroll;

    contentRect.pos = rect.pos-Vec2i32{xScroll, yScroll};

    child->resize(parentProps, contentRect);
}

void LayoutScroll::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    if (hScroll) {
        f(hScroll.get());
    }
    if (vScroll) {
        f(vScroll.get());
    }
    if (child) {
        f(child.get());
    }
}

LayoutSlider::LayoutSlider(Private privateTag,
                           LayoutContext *context,
                           bool isHorizontal,
                           double minValue,
                           double maxValue,
                           double value)
    : LayoutBase{context}, isHorizontal{isHorizontal}, minValue{minValue}, maxValue{maxValue}, value{value}
{
    setParentsOfChildren();
}

LayoutBase::EvtRes LayoutSlider::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    const int dragThreshold = 10;

    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::scroll:
        applyWheel(evt.dragDelta.y);
        std::cout << "scrolling2" << std::endl;

        context->setNeedsResize();
        return EvtRes::consumed;
    case MouseEvt::Action::move:
        hovering = true;
        break;
    case MouseEvt::Action::drag:
        hovering = true;
        if (evt.dragMax > dragThreshold) {
            int dragDistPixels = isHorizontal ? evt.dragDelta.x : evt.dragDelta.y;
            auto origPos = posFromValue(dragStartValue);
            auto newPos = origPos + dragDistPixels;
            value = valueFromPos(newPos);
            context->setNeedsResize();
        }
        break;
    case MouseEvt::Action::press:
        hovering = true;
        dragStartValue = value;
        grabMouse();
        break;
    case MouseEvt::Action::release:
        if (!evt.insideElement) {
            hovering = false;
        }
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

void LayoutSlider::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    // do in resize?
    handleSize = std::max(minHandleSize, static_cast<int>((isHorizontal ? width : height)*proportion));
    int handlePos = posFromValue(value);
    RectI handleRect = calcHandleRect(handlePos);

    const auto medGrey = rgb(35, 35, 35);
    const auto darkGrey = rgb(10,10,10);
    const auto ltGrey = rgb(84, 84, 84);

    drawRect(g, *this, TRANS, hovering ? medGrey : darkGrey);
    handleRect.shrink(2);
    drawRect(g, handleRect, TRANS, hovering ? ltGrey : medGrey);
}

SizeBound2d LayoutSlider::getBound(const PropertyBag& parentProps)
{
    return SizeBound2d{SizeBound{50}, SizeBound{30}};
}

void LayoutSlider::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
}

void LayoutSlider::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}

void LayoutSlider::stepValue(int count)
{
    double t = tFromValue(value);
    t += count * clickStep;
    t = std::clamp(t, 0., 1.);
    value = valueFromT(t);
}

void LayoutSlider::applyWheel(int count)
{
    double t = tFromValue(value);
    t -= count * wheelStep;
    t = std::clamp(t, 0., 1.);
    value = valueFromT(t);
}

void ButtonSet::onButtonPress(const PropertyBag &parentProps, LayoutButtonPtr button, int buttonIndex, bool checked)
{
    if (isRadio) {
        for (int i = 0; i < buttons.size(); ++i) {
            if (i != buttonIndex) {
                buttons[i]->setChecked(false);
            }
        }
    }
}

LayoutDragHandle::LayoutDragHandle(Private privateTag, LayoutContext *context, std::function<void(DragState, Vec2d)> dragCallback)
    : LayoutBase{context}, dragCallback{dragCallback}
{
}

LayoutBase::EvtRes LayoutDragHandle::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    const int dragThreshold = 10;

    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::scroll:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::move:
        hovering = true;
        break;
    case MouseEvt::Action::drag:
        hovering = true;
        if (evt.dragMax > dragThreshold) {
            dragCallback(DragState::dragging, evt.dragDelta);
        }
        break;
    case MouseEvt::Action::press:
        hovering = true;
        dragCallback(DragState::start, evt.dragDelta);
        grabMouse();
        break;
    case MouseEvt::Action::release:
        if (!evt.insideElement) {
            hovering = false;
        }
        if (hasDragFocus()) {
            dragCallback(DragState::end, evt.dragDelta);
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

void LayoutDragHandle::draw(const PropertyBag& parentProps, mssm::Canvas2d& g)
{
    drawRect(g, thisRect(), WHITE, BLUE);
}

SizeBound2d LayoutDragHandle::getBound(const PropertyBag& parentProps)
{
    return bound;
}

void LayoutDragHandle::resize(const PropertyBag& parentProps, const RectI& rect)
{
    setRect(rect);
}

void LayoutDragHandle::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}

void HoverTrack::onMouse(LayoutBase* element, const MouseEvt &evt, bool captureOnPress)
{
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::scroll:
    case MouseEvt::Action::enter:
        break;
    case MouseEvt::Action::move:
        mode = DrawMode::hover;
        break;
    case MouseEvt::Action::drag:
        if (evt.insideElement) {
            mode = DrawMode::pressing;
        }
        else {
            mode = DrawMode::normal;
        }
        break;
    case MouseEvt::Action::press:
        mode = DrawMode::pressing;
        if (captureOnPress) {
            element->grabMouse();
        }
        break;
    case MouseEvt::Action::release:
        if (evt.insideElement) {
            // if (type == ButtonType::normal) {
            //     onButtonPress(true);
            // }
            mode = DrawMode::hover;
        }
        else {
            mode = DrawMode::normal;
        }
        if (element->hasDragFocus()) {
            element->releaseMouse();
        }
        break;
    case MouseEvt::Action::exit:
        mode = DrawMode::normal;
        break;
    }
}

LayoutAdapter::LayoutAdapter(Private privateTag, LayoutContext *context, LayoutPtr child)
    : LayoutBase(context)
    , child(child)
{
    setParentsOfChildren();
}

void LayoutAdapter::draw(const PropertyBag& parentProps, mssm::Canvas2d &g)
{
    child->draw(parentProps, g);
}

SizeBound2d LayoutAdapter::getBound(const PropertyBag& parentProps)
{
    return child->getBound(parentProps);
}

void LayoutAdapter::resize(const PropertyBag& parentProps, const RectI& rect)
{
    child->resize(parentProps, rect);
    setRect(rect);
}

void LayoutAdapter::foreachChildImpl(std::function<void(LayoutBase *)> f, ForeachContext context,
                                 bool includeOverlay,
                                 bool includeCollapsed)
{
    if (child) {
        f(child.get());
    }
}

LayoutAdapterPadded::LayoutAdapterPadded(Private privateTag,
                                         LayoutContext *context,
                                         LayoutPtr child,
                                         Padding padding)
    : LayoutAdapter(privateTag, context, child), padding{padding}
{

}

SizeBound2d LayoutAdapterPadded::getBound(const PropertyBag& parentProps)
{
    auto bound = child->getBound(parentProps);
    grow(bound, padding);
    return bound;
}

void LayoutAdapterPadded::resize(const PropertyBag& parentProps, const RectI& rect)
{
    child->resize(parentProps, shrunk(rect, padding));
    setRect(rect);
}

LayoutAdapterClickable::LayoutAdapterClickable(Private privateTag,
                                               LayoutContext *context,
                                               LayoutPtr child,
                                               Padding padding,
                                               std::function<void()> clickCallback)
    : LayoutAdapterPadded{privateTag, context, child, padding}, clickCallback{clickCallback}
{

}

LayoutBase::EvtRes LayoutAdapterClickable::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    switch (evt.action) {
    case MouseEvt::Action::none:
    case MouseEvt::Action::enter:
    case MouseEvt::Action::exit:
    case MouseEvt::Action::move:
    case MouseEvt::Action::drag:
    case MouseEvt::Action::release:
    case MouseEvt::Action::scroll:
        break;
    case MouseEvt::Action::press:
        clickCallback();
        break;
    }

    return EvtRes::consumed;
}

void LayoutLeaf::resize(const PropertyBag &parentProps, const RectI &rect)
{
    setRect(rect);
}

void LayoutLeaf::foreachChildImpl(std::function<void(LayoutBase *)> f,
                              ForeachContext context,
                              bool includeOverlay,
                              bool includeCollapsed)
{
}

LayoutToggle::LayoutToggle(Private privateTag, LayoutContext *context, ToggleStyle style)
    : LayoutLeaf{context}, style{style}
{

}

void LayoutToggle::draw(const PropertyBag &parentProps, mssm::Canvas2d &g)
{
    bool checked = parentProps.getProperty<bool>("Checked");
    switch (style) {
    case ToggleStyle::expander:
        if (checked) {
            g.text(cast<Vec2d>(thisRect().center()), 30, "-", BLACK, HAlign::center, VAlign::center);
        }
        else {
            g.text(cast<Vec2d>(thisRect().center()), 30, "+", BLACK, HAlign::center, VAlign::center);
        }
        break;
    case ToggleStyle::checkbox:
        drawRect(g, thisRect(), WHITE, TRANS);
        if (checked) {
            g.text(cast<Vec2d>(thisRect().center()), 30, "X", BLACK, HAlign::center, VAlign::center);
        }
        break;
    case ToggleStyle::radio:
        g.ellipseV(thisRect().center(), width*3/4, height*3/4, BLACK);
        if (checked) {
            g.ellipseV(thisRect().center(), width/2, height/2, BLACK, BLACK);
        }
        break;
    }
}

SizeBound2d LayoutToggle::getBound(const PropertyBag &parentProps)
{
    return {SizeBound{20}, SizeBound{20}};
}


LayoutImage::LayoutImage(Private privateTag, LayoutContext *context, mssm::Image img)
  : LayoutBase{context}, image{img}
{
    setName("image");
    //bound.grow(img.width(),image.height());
    setParentsOfChildren();
}

LayoutImage::~LayoutImage()
{

}

void LayoutImage::updateImage(mssm::Image img)
{
    image = img;
}

void LayoutImage::draw(const PropertyBag &parentProps, mssm::Canvas2d &g)
{
    Vec2d pos{thisRect().upperLeft()};
    g.image(pos, width, height, image);
}

SizeBound2d LayoutImage::getBound(const PropertyBag &parentProps)
{
    return bound;
}

void LayoutImage::resize(const PropertyBag &parentProps, const RectI &rect)
{
    setRect(rect);
    // contentRect.pos = rect.pos;
    // contentRect.width = bound.xBound.constrain(rect.width);
    // contentRect.height = bound.yBound.constrain(rect.height);
}

void LayoutImage::foreachChildImpl(std::function<void (LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
}

LayoutBase::EvtRes LayoutImage::onMouse(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt)
{
    return LayoutBase::EvtRes::propagate;
}

LayoutModalHolder::LayoutModalHolder(Private privateTag, LayoutContext *context, LayoutPtr child, LayoutPtr modal)
    : LayoutBase{context}, child{child}, modal{modal}
{
    setName("modalHolder");
    //bound.grow(img.width(),image.height());
    setParentsOfChildren();
}

void LayoutModalHolder::draw(const PropertyBag &parentProps, mssm::Canvas2d &g)
{
    pushClip(g, thisRect(), false);
    foreachChild([&g,&parentProps](auto *c) { c->draw(parentProps, g); }, ForeachContext::drawing, false, false);
    popClip(g);
}

SizeBound2d LayoutModalHolder::getBound(const PropertyBag &parentProps)
{
    return child->getBound(parentProps);
}

void LayoutModalHolder::resize(const PropertyBag &parentProps, const RectI &rect)
{
    SizeBound2d modalBound = modal ? modal->getBound(parentProps) : SizeBound2d{};
    SizeBound2d childBound = child ? child->getBound(parentProps) : SizeBound2d{};

    RectI modalRect{0, 0, modalBound.xBound.minSize, modalBound.yBound.minSize };
    modalRect = modalRect.centered(rect);

    if (modal) {
        modal->resize(parentProps, modalRect);
    }

    if (child) {
        child->resize(parentProps, rect);
    }

    setRect(rect);
}

void LayoutModalHolder::foreachChildImpl(std::function<void (LayoutBase *)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed)
{
    bool traverseChild{false};
    bool traverseModal{false};

    switch (context) {
    case LayoutBase::ForeachContext::drawing:
        traverseChild = true;
        traverseModal = true;
        break;
    case LayoutBase::ForeachContext::events:
        if (modal) {
            traverseModal = true;
        }
        else {
            traverseChild = true;
        }
        break;
    case LayoutBase::ForeachContext::other:
        traverseChild = true;
        traverseModal = true;
        break;
    }

    if (child && traverseChild) {
        f(child.get());
    }

    if (modal && traverseModal) {
        f(modal.get());
    }
}

void LayoutModalHolder::setModal(LayoutPtr modal)
{
    if (child) {
        child->localReleaseMouseAndKeyboard();
    }
    this->modal = modal;
    context->setNeedsResize();
    setParentsOfChildren();
}

void LayoutModalHolder::clearModal()
{
    modal.reset();
    context->setNeedsResize();
}

LayoutModalFrame::LayoutModalFrame(Private privateTag, LayoutContext *context, LayoutPtr child)
    : LayoutAdapter(privateTag, context, child)
{

}

// LayoutBase::EvtProp LayoutModalFrame::onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt)
// {
//     return EvtProp::consumed;
// }

// LayoutBase::EvtProp LayoutModalFrame::onMouseDeferred(const PropertyBag &parentProps, MouseEventReason reason, const MouseEvt &evt)
// {
//     return EvtProp::consumed;
// }

// LayoutBase::EvtProp LayoutModalFrame::onKey(const PropertyBag &parentProps, const KeyEvt &evt)
// {
//     return EvtProp::consumed;
// }

// LayoutBase::EvtProp LayoutModalFrame::onKeyDeferred(const PropertyBag &parentProps, const KeyEvt &evt)
// {
//     return EvtProp::consumed;
// }


