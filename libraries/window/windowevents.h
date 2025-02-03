#ifndef WINDOWEVENTS_H
#define WINDOWEVENTS_H

#include <iostream>
#include <vector>


namespace gjh {

/* Defines for GLFW Function keys, so we don't have to include all of glfw in graphics.h  */
/* hope these don't change on us... */
#define fwdDefGLFW_KEY_ESCAPE             256
#define fwdDefGLFW_KEY_ENTER              257
#define fwdDefGLFW_KEY_TAB                258
#define fwdDefGLFW_KEY_BACKSPACE          259
#define fwdDefGLFW_KEY_INSERT             260
#define fwdDefGLFW_KEY_DELETE             261
#define fwdDefGLFW_KEY_RIGHT              262
#define fwdDefGLFW_KEY_LEFT               263
#define fwdDefGLFW_KEY_DOWN               264
#define fwdDefGLFW_KEY_UP                 265
#define fwdDefGLFW_KEY_PAGE_UP            266
#define fwdDefGLFW_KEY_PAGE_DOWN          267
#define fwdDefGLFW_KEY_HOME               268
#define fwdDefGLFW_KEY_END                269
#define fwdDefGLFW_KEY_CAPS_LOCK          280
#define fwdDefGLFW_KEY_SCROLL_LOCK        281
#define fwdDefGLFW_KEY_NUM_LOCK           282
#define fwdDefGLFW_KEY_PRINT_SCREEN       283
#define fwdDefGLFW_KEY_PAUSE              284
#define fwdDefGLFW_KEY_F1                 290
#define fwdDefGLFW_KEY_F2                 291
#define fwdDefGLFW_KEY_F3                 292
#define fwdDefGLFW_KEY_F4                 293
#define fwdDefGLFW_KEY_F5                 294
#define fwdDefGLFW_KEY_F6                 295
#define fwdDefGLFW_KEY_F7                 296
#define fwdDefGLFW_KEY_F8                 297
#define fwdDefGLFW_KEY_F9                 298
#define fwdDefGLFW_KEY_F10                299
#define fwdDefGLFW_KEY_F11                300
#define fwdDefGLFW_KEY_F12                301
#define fwdDefGLFW_KEY_F13                302
#define fwdDefGLFW_KEY_F14                303
#define fwdDefGLFW_KEY_F15                304
#define fwdDefGLFW_KEY_F16                305
#define fwdDefGLFW_KEY_F17                306
#define fwdDefGLFW_KEY_F18                307
#define fwdDefGLFW_KEY_F19                308
#define fwdDefGLFW_KEY_F20                309
#define fwdDefGLFW_KEY_F21                310
#define fwdDefGLFW_KEY_F22                311
#define fwdDefGLFW_KEY_F23                312
#define fwdDefGLFW_KEY_F24                313
#define fwdDefGLFW_KEY_F25                314
#define fwdDefGLFW_KEY_KP_0               320
#define fwdDefGLFW_KEY_KP_1               321
#define fwdDefGLFW_KEY_KP_2               322
#define fwdDefGLFW_KEY_KP_3               323
#define fwdDefGLFW_KEY_KP_4               324
#define fwdDefGLFW_KEY_KP_5               325
#define fwdDefGLFW_KEY_KP_6               326
#define fwdDefGLFW_KEY_KP_7               327
#define fwdDefGLFW_KEY_KP_8               328
#define fwdDefGLFW_KEY_KP_9               329
#define fwdDefGLFW_KEY_KP_DECIMAL         330
#define fwdDefGLFW_KEY_KP_DIVIDE          331
#define fwdDefGLFW_KEY_KP_MULTIPLY        332
#define fwdDefGLFW_KEY_KP_SUBTRACT        333
#define fwdDefGLFW_KEY_KP_ADD             334
#define fwdDefGLFW_KEY_KP_ENTER           335
#define fwdDefGLFW_KEY_KP_EQUAL           336
#define fwdDefGLFW_KEY_LEFT_SHIFT         340
#define fwdDefGLFW_KEY_LEFT_CONTROL       341
#define fwdDefGLFW_KEY_LEFT_ALT           342
#define fwdDefGLFW_KEY_LEFT_SUPER         343
#define fwdDefGLFW_KEY_RIGHT_SHIFT        344
#define fwdDefGLFW_KEY_RIGHT_CONTROL      345
#define fwdDefGLFW_KEY_RIGHT_ALT          346
#define fwdDefGLFW_KEY_RIGHT_SUPER        347
#define fwdDefGLFW_KEY_MENU               348
#define MAX_KEYS 400
#define MAX_MOUSE_BUTTONS 32


class Key {
    int code;
public:
    struct KeyConst // https://stackoverflow.com/a/70048197
    {
        int code;
        constexpr operator Key() const;
        constexpr operator int() const { return code; }
    };
    constexpr Key(char c) : code{static_cast<int>(c)} { }
    explicit constexpr Key(int c) : code{c}  { }
    explicit constexpr Key(KeyConst k) : code{ k.code } { }
    static constexpr KeyConst None{0};
    static constexpr KeyConst Left{fwdDefGLFW_KEY_LEFT};
    static constexpr KeyConst Right{fwdDefGLFW_KEY_RIGHT};
    static constexpr KeyConst Up{fwdDefGLFW_KEY_UP};
    static constexpr KeyConst Down{fwdDefGLFW_KEY_DOWN};
    static constexpr KeyConst LeftShift{fwdDefGLFW_KEY_LEFT_SHIFT};
    static constexpr KeyConst LeftCtrl{fwdDefGLFW_KEY_LEFT_CONTROL};
    static constexpr KeyConst LeftAlt{fwdDefGLFW_KEY_LEFT_ALT};
    static constexpr KeyConst RightShift{fwdDefGLFW_KEY_RIGHT_SHIFT};;
    static constexpr KeyConst RightCtrl{fwdDefGLFW_KEY_RIGHT_CONTROL};
    static constexpr KeyConst RightAlt{fwdDefGLFW_KEY_RIGHT_ALT};
    static constexpr KeyConst ESC{fwdDefGLFW_KEY_ESCAPE};
    static constexpr KeyConst ENTER{fwdDefGLFW_KEY_ENTER};
    static constexpr KeyConst PageUp{fwdDefGLFW_KEY_PAGE_UP};
    static constexpr KeyConst PageDown{fwdDefGLFW_KEY_PAGE_DOWN};
    static constexpr KeyConst Space{' '};
    static constexpr KeyConst Tab{fwdDefGLFW_KEY_TAB};
    static constexpr KeyConst Backspace{fwdDefGLFW_KEY_BACKSPACE};
    static constexpr KeyConst Delete{fwdDefGLFW_KEY_DELETE};
    static constexpr KeyConst Insert{fwdDefGLFW_KEY_INSERT};
    constexpr operator int() const { return code; }
    constexpr int keyCode() const { return code; }
    explicit constexpr operator char() const { return code; }
    constexpr bool operator==(KeyConst other) const { return code == other.code; }
    constexpr bool operator==(char other) const { return std::isalpha(other) ? (code == std::toupper(other)) : (code == other); }
    std::string toFormatString() const;
};

std::ostream& operator<<(std::ostream& os, const Key& k);

inline constexpr Key::KeyConst::operator Key() const
{
    return Key{code};
}

class MouseButton {
    int button{-1};
public:
    struct MouseButtonConst // https://stackoverflow.com/a/70048197
    {
        int button;
        constexpr operator MouseButton() const;
        constexpr operator int() const { return button; }
    };
    explicit constexpr MouseButton(int b) { button = b; }
    explicit constexpr MouseButton(MouseButtonConst b) : button{ b.button } { }

    static constexpr MouseButtonConst None{-1};
    static constexpr MouseButtonConst Left{0};
    static constexpr MouseButtonConst Right{1};
    static constexpr MouseButtonConst Middle{2};
    constexpr operator int() const { return button; }
    constexpr bool operator==(MouseButtonConst other) const { return button == other.button; }
    std::string toFormatString() const;
};

inline constexpr MouseButton::MouseButtonConst::operator MouseButton() const
{
    return MouseButton{button};
}

std::ostream& operator<<(std::ostream& os, const MouseButton& k);


enum class EvtType
{
    MousePress,   // arg = button,  x and y = mouse pos
    MouseRelease, // arg = button,  x and y = mouse pos
    MouseMove,    // arg = BITMASK OF BUTTONS!  1 = 0  2 = 1  4 = 2,  x and y = mouse pos
    MouseWheel,   // arg = delta, x and y = mouse pos
    KeyPress,     // arg = key
    KeyRepeat,    // arg = key
    KeyRelease,   // arg = key
    MusicEvent,   // arg:  0 = stopped,  1 = playing,  2 = paused
    WindowResize  // x and y => width and height.  arg = 0 windowed  arg = 1 fullscreen
};

enum class ModKey
{
    None  = 0,
    Shift = 1 << 0,
    Alt   = 1 << 1,
    Ctrl  = 1 << 2,
    CtrlAlt = Ctrl | Alt,
    CtrlShift = Ctrl | Shift,
    AltShift = Alt | Shift,
    CtrlAltShift = Ctrl | Alt | Shift
};


class Event
{
public:
    EvtType evtType;
    int     x;
    int     y;
    ModKey  mods;
    int     arg;
public:
#ifdef VEC2D_H
    Vec2d mousePos() const { return Vec2d(x, y); }
#endif
    bool hasCtrl() const { return static_cast<int>(mods) & static_cast<int>(ModKey::Ctrl);  }
    bool hasAlt() const { return static_cast<int>(mods) & static_cast<int>(ModKey::Alt);   }
    bool hasShift() const { return static_cast<int>(mods) & static_cast<int>(ModKey::Shift); }
    Key  key() const { return Key(arg); }
    MouseButton  mouseButton() const { return static_cast<MouseButton>(arg); }
};

std::ostream& operator<<(std::ostream& os, const Event& evt);

class WindowEventSink {
public:
    virtual void  postEvent(int x, int y, EvtType evtType, ModKey mods, int arg) = 0;
    virtual void  toggleFullScreen() = 0;
    virtual void  setMousePos(int x, int y) = 0;
    virtual void  setGotResize(int width, int height) = 0;
    virtual int   currMousePosX() const = 0;
    virtual int   currMousePosY() const = 0;
};


enum class CoreWindowCursor {
    arrow,
    iBeam,
    crosshair,
    hand,
    hresize,
    vresize
};

class EventSource {
public:
    virtual ~EventSource() = default;
    virtual bool isKeyPressed(Key c) const = 0;
    virtual bool onKeyPress(Key c) const  = 0;
    virtual bool onKeyRelease(Key c) const  = 0;
    virtual bool isMousePressed(MouseButton button) const = 0;
    virtual bool onMousePress(MouseButton button) const   = 0;
    virtual bool onMouseRelease(MouseButton button) const = 0;
    virtual Key getKeyPressed() const = 0;
    virtual MouseButton getMousePressed() const = 0;
    virtual int getWheelDelta() const = 0;

    virtual double maxDragDistance(MouseButton button) const = 0;

    virtual int   mouseDragStartX(MouseButton button) const = 0;
    virtual int   mouseDragStartY(MouseButton button) const = 0;

    template <typename V>
    constexpr V mouseDragStart(MouseButton button) const { return V{mouseDragStartX(button), mouseDragStartY(button)}; }

    virtual int   mousePosX() const = 0;
    virtual int   mousePosY() const = 0;

    template <typename V>
    constexpr V mousePos() const { return V{mousePosX(), mousePosY()}; }

    virtual const std::vector<Event>& events() const = 0;

    bool   isKeyPressed(char c) const { return isKeyPressed(Key{std::isalpha(c) ? std::toupper(c) : c}); }
    bool   isKeyPressed(Key::KeyConst c) const { return isKeyPressed(Key(c)); }

    bool   onKeyPress(char c) const { return onKeyPress(Key{std::isalpha(c) ? std::toupper(c) : c}); }
    bool   onKeyPress(Key::KeyConst c) const { return onKeyPress(Key{c}); }

    bool   onKeyRelease(char c) const { return onKeyRelease(Key{std::isalpha(c) ? std::toupper(c) : c}); }
    bool   onKeyRelease(Key::KeyConst c) const { return onKeyRelease(Key{c}); }

    bool   isShiftKeyPressed() const { return isKeyPressed(Key::LeftShift) || isKeyPressed(Key::RightShift); }
    bool   isCtrlKeyPressed() const { return isKeyPressed(Key::LeftCtrl) || isKeyPressed(Key::RightCtrl); }
    bool   isAltKeyPressed() const { return isKeyPressed(Key::LeftAlt) || isKeyPressed(Key::RightAlt); }

    virtual double timeMicros() const = 0;
    virtual double timeSeconds() const = 0;
    virtual double timeMs() const = 0;
    virtual double elapsedMs() const = 0;
    virtual double elapsedMicros() const = 0;
    virtual double elapsedSeconds() const = 0;
};

class EvtSourceWrapper : public gjh::EventSource
{
protected:
    gjh::EventSource* eventSource{};
public:
    EvtSourceWrapper(gjh::EventSource* es) : eventSource{es} {}
    inline bool isKeyPressed(gjh::Key c) const override { return eventSource->isKeyPressed(c); }
    inline bool onKeyPress(gjh::Key c) const override { return eventSource->onKeyPress(c); }
    inline bool onKeyRelease(gjh::Key c) const override { return eventSource->onKeyRelease(c); }
    inline bool isMousePressed(gjh::MouseButton button) const override { return eventSource->isMousePressed(button); }
    inline bool onMousePress(gjh::MouseButton button) const override { return eventSource->onMousePress(button); }
    inline bool onMouseRelease(gjh::MouseButton button) const override { return eventSource->onMouseRelease(button); }
    inline gjh::Key getKeyPressed() const override  { return eventSource->getKeyPressed(); }
    inline gjh::MouseButton getMousePressed() const override { return eventSource->getMousePressed(); }
    inline int getWheelDelta() const override  { return eventSource->getWheelDelta(); }
    inline double maxDragDistance(gjh::MouseButton button) const override { return eventSource->maxDragDistance(button); }
    inline int mouseDragStartX(gjh::MouseButton button) const override { return eventSource->mouseDragStartX(button); }
    inline int mouseDragStartY(gjh::MouseButton button) const override { return eventSource->mouseDragStartY(button); }
    inline int mousePosX() const override { return eventSource->mousePosX(); }
    inline int mousePosY() const override { return eventSource->mousePosY(); }
    inline const std::vector<gjh::Event> &events() const override { return eventSource->events(); }
    inline double timeMicros() const override { return eventSource->timeMicros(); }
    inline double timeSeconds() const override { return eventSource->timeSeconds(); }
    inline double timeMs() const override { return eventSource->timeMs(); }
    inline double elapsedMs() const override { return eventSource->elapsedMs(); }
    inline double elapsedMicros() const override { return eventSource->elapsedMicros(); }
    inline double elapsedSeconds() const override  { return eventSource->elapsedSeconds(); }    
};


} // namespace gjh

#endif // WINDOWEVENTS_H
