#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <vector>
#include <cstdint>
#include <cmath>

#include "windowevents.h"

namespace gjh {

class EventManager : public WindowEventSink, public EventSource {
private:
    struct FocusContext {
        uint32_t context;
        FocusContext() : context{0} {}
        FocusContext(uint32_t context) : context{context} {}
    };

    enum class KeyState {
        up,            // still up (2nd and subsequent frames after release)
        justPressed,   // just pressed this frame
        down,          // still down (2nd and subsequent frames after press)
        justReleased,  // just released this frame
    };

    class KeyTracker {
        KeyState state;
        FocusContext focus;
    public:
        void setJustPressed() { state = KeyState::justPressed; }
        void setJustReleased() { state = KeyState::justReleased; }
        void setDown() { state = KeyState::down; }
        void setUp() { state = KeyState::up; }
        KeyState getState() const { return state; }
        bool isPressed() const { return state == KeyState::down || state == KeyState::justPressed; }
        bool isJustPressed() const { return state == KeyState::justPressed; }
        bool isJustReleased() const { return state == KeyState::justReleased; }
    };

    class MouseTracker : public KeyTracker {
    public:
        int mouseDownX;
        int mouseDownY;
        int mouseDragMaxX;
        int mouseDragMaxY;
    public:
        void onMouseDown(int x, int y) { setJustPressed(); mouseDownX = x; mouseDownY = y; mouseDragMaxX = 0; mouseDragMaxY = 0; }
        void onMouseDrag(int x, int y) { mouseDragMaxX = std::max(mouseDragMaxX, std::abs(x - mouseDownX)); mouseDragMaxY = std::max(mouseDragMaxY, std::abs(y - mouseDownY)); }
        void onMouseUp(int x, int y) { setJustReleased(); }
        double maxDrag() const { return std::hypot(mouseDragMaxX, mouseDragMaxY); }
    };

    FocusContext keyboardFocus;
    FocusContext mouseFocus;

    int minKeyStateIdx; // minimum idx of keyState that was changed this update cycle
    int maxKeyStateIdx;                  // max idx of keyState that was changed this update cycle
    std::vector<KeyTracker> keyState;

    int minMouseStateIdx; // minimum idx of keyState that was changed this update cycle
    int maxMouseStateIdx;                  // max idx of keyState that was changed this update cycle
    std::vector<MouseTracker> mouseState;

    Key pressedKey{Key::None}; // most recent key pressed, but one frame only
    MouseButton pressedButton{MouseButton::None};      // most recent mouse button pressed, but one frame only
    int wheelDelta{0};

    int currMouseX{0};
    int currMouseY{0};

    std::vector<Event> _events;
public:
    EventManager();
    virtual ~EventManager() = default;

    bool   isKeyPressed(Key c) const override { return keyState[(int)c].isPressed(); }
    bool   onKeyPress(Key c) const override { return keyState[(int)c].isJustPressed(); }
    bool   onKeyRelease(Key c) const override { return keyState[(int)c].isJustReleased(); }

    bool   isMousePressed(MouseButton button) const override { return mouseState[(int)button].isPressed();; }
    bool   onMousePress(MouseButton button) const override { return mouseState[(int)button].isJustPressed(); }
    bool   onMouseRelease(MouseButton button) const override { return mouseState[(int)button].isJustReleased(); }

    double maxDragDistance(MouseButton button) const override { return mouseState[(int)button].maxDrag(); }

    int   mouseDragStartX(MouseButton button) const override { return mouseState[(int)button].mouseDownX; }
    int   mouseDragStartY(MouseButton button) const override { return mouseState[(int)button].mouseDownY; };

    Key    getKeyPressed() const override { return pressedKey; }
    MouseButton getMousePressed() const override { return pressedButton; }
    int    getWheelDelta() const override { return wheelDelta; }

    const std::vector<Event>& events() const override ;
protected:
    void postEvent(int x, int y, EvtType evtType, ModKey mods, int arg) override;
    void updateInputs();
private:
    void resetKeyChangeIdx();
    void updateKeyChangeIdx(int idx);

    void resetMouseChangeIdx();
    void updateMouseChangeIdx(int idx);

    // EventSource interface
public:
    constexpr int mousePosX() const override { return currMouseX; }
    constexpr int mousePosY() const override { return currMouseY; }

    // WindowEventSink interface
public:
    constexpr void setMousePos(int x, int y) override { currMouseX = x; currMouseY = y; }
    constexpr int currMousePosX() const override { return currMouseX; }
    constexpr int currMousePosY() const override { return currMouseY; }
};

}

#endif // EVENTMANAGER_H
