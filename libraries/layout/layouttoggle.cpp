
#include "layouttoggle.h"
#include "rectdraw.h"

using namespace mssm;

LayoutToggle::LayoutToggle(Private privateTag, LayoutContext *context, ToggleStyle style)
    : LayoutBase{context}, style{style}
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

void LayoutToggle::resize(const PropertyBag &parentProps, const RectI &rect)
{
    setRect(rect);
}

void LayoutToggle::foreachChildImpl(std::function<void(LayoutBase *)> f,
                                    ForeachContext context,
                                    bool includeOverlay,
                                    bool includeCollapsed)
{
}
