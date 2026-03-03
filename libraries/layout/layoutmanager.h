#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

//#include "graphics.h"
#include "layouthelper.h"
#include <cstdint>

class LayoutManager
{
public:
    struct FrameStats {
        uint64_t frameIndex{0};
        uint64_t nodesVisited{0};
        uint64_t eventsProcessed{0};
        uint64_t overlayCount{0};
        bool resizedThisFrame{false};
    };

private:
    double lastWidth{-1};
    double lastHeight{-1};
    LayoutContext *context;
    LayoutPtr layout;
    Vec2d lastMousePos;
    FrameStats stats{};

public:
    LayoutManager(LayoutContext *context, LayoutPtr layout);
    LayoutManager(LayoutContext *context, LayoutHelper::Builder builder);

    void propagateEvents(const PropertyBag &parentProps, double elapsedTimeS);
    void draw(mssm::CoreWindow &window, mssm::Canvas2d &g);
    const FrameStats& frameStats() const { return stats; }

    LayoutPtr findByName(std::string name);

    template<typename T> T* findByName(std::string name)
    {
        LayoutPtr p = findByName(name);
        if (p) {
            return dynamic_cast<T*>(p.get());
        }
        return nullptr;
    }

    void propagateMouse(const PropertyBag &parentProps, const RectI& clip, MouseEvt& evt);
    void propagateKey(const PropertyBag &parentProps, const RectI& clip, KeyEvt& evt);

protected:
   // void updateHoverElement(LayoutPtr newHoverElement, Vec2i32 pos);
    virtual void resize(const PropertyBag& parentProps, const RectI& rect);

};

#endif // LAYOUTMANAGER_H
