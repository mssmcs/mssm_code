#include "eventmanager.h"
#include <limits>
#include <cmath>

using namespace mssm;
using namespace std;

EventManager::EventManager()
{
    keyState.resize(MAX_KEYS + 1);            //TODO: this is lame
    mouseState.resize(MAX_MOUSE_BUTTONS + 1); //TODO: this is lame

    resetKeyChangeIdx();
    resetMouseChangeIdx();
}

void EventManager::resetKeyChangeIdx()
{
    minKeyStateIdx = numeric_limits<int>::max();
    maxKeyStateIdx = numeric_limits<int>::lowest();
}

void EventManager::updateKeyChangeIdx(int idx)
{
    if (idx < minKeyStateIdx) {
        minKeyStateIdx = idx;
    }
    if (idx > maxKeyStateIdx) {
        if (idx >= keyState.size()) {
            keyState.resize(idx + 1);
        }
        maxKeyStateIdx = idx;
    }
}

void EventManager::resetMouseChangeIdx()
{
    minMouseStateIdx = numeric_limits<int>::max();
    maxMouseStateIdx = numeric_limits<int>::lowest();
}

void EventManager::updateMouseChangeIdx(int idx)
{
    if (idx < minMouseStateIdx) {
        minMouseStateIdx = idx;
    }
    if (idx > maxMouseStateIdx) {
        if (idx >= mouseState.size()) {
            mouseState.resize(idx + 1);
        }
        maxMouseStateIdx = idx;
    }
}

void EventManager::postEvent(int x, int y, EvtType evtType, ModKey mods, int arg)
{
    Event evt{evtType, x, y, mods, arg};

    switch (evtType) {
    case EvtType::MouseWheel:
        wheelDelta += arg;
        break;
    case EvtType::KeyPress:
        updateKeyChangeIdx(arg);
        keyState[arg].setJustPressed();
        pressedKey = Key{arg};
        break;
    case EvtType::KeyRelease:
        updateKeyChangeIdx(arg);
        keyState[arg].setJustReleased();
        break;
    case EvtType::MousePress:
        updateMouseChangeIdx(arg);
        mouseState[arg].onMouseDown(x, y);
        pressedButton = static_cast<MouseButton>(arg);
        break;
    case EvtType::MouseRelease:
        updateMouseChangeIdx(arg);
        mouseState[arg].onMouseUp(x, y);
        break;
    case EvtType::MouseMove:
        if (arg) {  // handling dragging
            int buttonBit = 1;
            for (int i = 0; i < 3; i++) {
                if (arg & buttonBit) {
                    MouseTracker &mouseTracker = mouseState[i];
                    if (mouseTracker.isPressed()) {
                        mouseTracker.onMouseDrag(x, y);
                    }
                }
                buttonBit <<= 1;
            }
        }
        break;
    default:
        break;
    }

    _events.emplace_back(std::move(evt));
}

void EventManager::updateInputs()
{
    for (int idx = minKeyStateIdx; idx <= maxKeyStateIdx; idx++) {
        switch (keyState[idx].getState()) {
        case KeyState::justPressed:
            keyState[idx].setDown();
            break;
        case KeyState::justReleased:
            keyState[idx].setUp();
            break;
        case KeyState::down:
        case KeyState::up:
            break;
        }
    }

    for (int idx = minMouseStateIdx; idx <= maxMouseStateIdx; idx++) {
        MouseTracker &mouseTracker = mouseState[idx];
        switch (mouseTracker.getState()) {
        case KeyState::justPressed:
            mouseTracker.setDown();
            break;
        case KeyState::justReleased:
            mouseTracker.setUp();
            break;
        case KeyState::down:
        case KeyState::up:
            break;
        }
    }

    resetKeyChangeIdx();
    resetMouseChangeIdx();
    pressedKey = Key::None;
    pressedButton = MouseButton::None;
    wheelDelta = 0;

    _events.clear();
}
