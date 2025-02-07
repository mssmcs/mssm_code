#ifndef LAYOUTCONTEXT_H
#define LAYOUTCONTEXT_H

#include "windowbase.h"
#include "propertybag.h"
#include "recti.h"
#include "textgeometry.h"
#include "vec2d.h"
#include "windowevents.h"
#include <memory>
#include <vector>

class LayoutBase;

using LayoutPtr = std::shared_ptr<LayoutBase>;

class MouseEvt {
public:
    enum class Action {
        none,
        enter,
        exit,
        move,
        drag,
        press,
        release,
        scroll
    };
    Action action{};
    mssm::MouseButton button{mssm::MouseButton::Left};
    Vec2d pos;
    mssm::ModKey  mods;
    Vec2d dragDelta;
    double dragMax;
    bool insideElement{false};
};

class KeyEvt {
public:
    enum class Action {
        press,
        repeat,
        release
    };
    Action action{};
    mssm::Key key{mssm::Key::None};
    mssm::ModKey  mods;
    Vec2d pos;
    bool insideElement{false};
    bool hasCtrl() const { return static_cast<int>(mods) & static_cast<int>(mssm::ModKey::Ctrl);  }
    bool hasAlt() const { return static_cast<int>(mods) & static_cast<int>(mssm::ModKey::Alt);   }
    bool hasShift() const { return static_cast<int>(mods) & static_cast<int>(mssm::ModKey::Shift); }
};

enum class MouseEventReason {
    debug,
    exit,
    within,
    containsGrab,
    isGrab,
    hoverOnly,  // mouse is grabbed elsewhere.  This is just to track mouse enter/exit
};

class HoverChain {
    std::vector<LayoutPtr> chain;
public:
    void onRemove(LayoutPtr ptr);
};

class LayoutContext : public TextMetrics {
    LayoutPtr keyFocus{};
    LayoutPtr dragFocus{};
    LayoutPtr toolTip{};  // overlay! don't treat as child
    bool needsResize{true};
    RectI windowRect;
    bool debug{false};
    //HoverChain hoverChain;
    std::vector<LayoutPtr> hoverChain;
public: // TODO: make private later
    std::vector<LayoutPtr> overlays;
public:
    void updateWindowRect(const RectI& windowRect);
    const RectI& getWindowRect() const { return windowRect; }

 //   virtual mssm::CoreWindow* getWindow() = 0;
    // focus handling
    void setKeyboardFocus(LayoutPtr focusElement);
    void setDragFocus(LayoutPtr focusElement);
    bool hasDragFocus(LayoutPtr focusElement) const { return dragFocus == focusElement; }
    bool isAnyKeyboardFocus() const { return static_cast<bool>(keyFocus); }
    bool isAnyDragFocus() const { return static_cast<bool>(dragFocus); }

    void setDebug(bool debug) { this->debug = debug; }
    bool getDebug() const { return debug; }

    // TextMetrics
    virtual const FontInfo &defaultFont() const = 0;
    virtual void   textExtents(const FontInfo &sizeAndFace, const std::string& str, TextExtents& extents) = 0;
    virtual double textWidth(const FontInfo &sizeAndFace, const std::string& str) = 0;
    virtual std::vector<double> getCharacterXOffsets(const FontInfo& sizeAndFace, double startX, const std::string& text) = 0;

    virtual std::vector<mssm::Event> events() = 0;
    virtual Vec2d mouseDragStart(mssm::MouseButton button) const = 0;
    virtual double maxDragDistance(mssm::MouseButton button) const = 0;

    // cursor
    virtual void setCursor(mssm::CoreWindowCursor cursor) = 0;
    virtual mssm::CoreWindowCursor getCursor() const = 0;

    virtual Vec2d mousePos() const = 0;

    virtual void debugMouse(const RectI& rect, MouseEventReason reason, const MouseEvt& evt) = 0;

    void setNeedsResize() { needsResize = true; }
    bool getNeedsResize() const { return needsResize; }
    void clearNeedsResize() { needsResize = false; }

    void pushOverlay(LayoutPtr overlay);
    void removeOverlay(LayoutPtr overlay);

    void updateHoverChain(const PropertyBag &parentProps, LayoutPtr hoverElement, bool isInside, Vec2d pos);
private:
    void sendEnter(const PropertyBag &parentProps, LayoutPtr element, Vec2d pos);
    void sendExit(const PropertyBag& parentProps, LayoutPtr element, Vec2d pos);
    void truncateHoverChain(const PropertyBag &parentProps, int depth, Vec2d pos);
};

#endif // LAYOUTCONTEXT_H
