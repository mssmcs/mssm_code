#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

//#include "graphics.h"
#include "layouthelper.h"

class LayoutManager
{
    double lastWidth{-1};
    double lastHeight{-1};
    LayoutContext *context;
    LayoutPtr layout;
    Vec2d lastMousePos;
    LayoutPtr lastHoverElement;
    // LayoutPtr hoverElement;

public:
    LayoutManager(LayoutContext *context, LayoutPtr layout);
    LayoutManager(LayoutContext *context, LayoutHelper::Builder builder);

    void propagateEvents(const PropertyBag &parentProps);
    void draw(mssm::CoreWindow &window, mssm::Canvas2d &g);

    LayoutPtr findByName(std::string name);

    template<typename T> T* findByName(std::string name)
    {
        LayoutPtr p = findByName(name);
        if (p) {
            return dynamic_cast<T*>(p.get());
        }
        return nullptr;
    }

    LayoutBase::EvtProp propagateMouse(const PropertyBag &parentProps, const RectI& clip, MouseEvt& evt);
    LayoutBase::EvtProp propagateKey(const PropertyBag &parentProps, const RectI& clip, KeyEvt& evt);

protected:
   // void updateHoverElement(LayoutPtr newHoverElement, Vec2i32 pos);
};

#endif // LAYOUTMANAGER_H
