#ifndef GROBS_H
#define GROBS_H

// #define _USE_MATH_DEFINES
// #include <cmath>

#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <functional>
#include <sstream>
// #include <format>
//#include "format_helper.h"

#include <deque>

#include "array.h"

//#include <ranges>
//#include "nanovg.h"

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif

#ifdef INCLUDE_SOUND
#include "soloud.h"
#include "soloud_wavstream.h"
#endif

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif 

#include "vec2d.h"
#include "rand.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

//#include <concepts>

class QNanoPainter;
class GLFWwindow;
class NVGcontext;

//template <typename T>
//concept is_vc2d_integral =  std::conjunction_v<
//                std::is_integral<decltype(T::x)>,
//                std::is_integral<decltype(T::y)>
//            >;

//template <typename T>
//concept is_vec2d_floating_point =
//            std::conjunction_v<
//                std::is_floating_point<decltype(T::x)>,
//                std::is_floating_point<decltype(T::y)>
//            >;

//template <typename T>
//concept is_vec2d_numeric = is_vc2d_integral<T> || is_vec2d_floating_point<T>;

//template <typename R, typename V>
//concept is_vec2d_range = std::is_convertible<R, std::ranges::range<R>> &&
//                         std::same_as<std::ranges::range_reference_t<R>, V> &&
//                         is_vec2d_numeric<V>;

namespace mssm
{

enum class HAlign {
    left   = 1<<0,
    center = 1<<1,
    right  = 1<<2
};

enum class VAlign {
    top      = 1<<3,
    center   = 1<<4,
    bottom   = 1<<5,
    baseline = 1<<6,
};

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


class Key {
    int code;
public:
    struct KeyConst // https://stackoverflow.com/a/70048197
    {
        int code;
        constexpr operator Key() const;
        constexpr operator int() const { return code; }
    };
    explicit constexpr Key(int c) { code = c; }
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
    MouseMove,    // arg = button,  x and y = mouse pos
    MouseWheel,   // arg = delta, x and y = mouse pos
    KeyPress,     // arg = key
    KeyRelease,   // arg = key
    MusicEvent,   // arg:  0 = stopped,  1 = playing,  2 = paused
    WindowResize  // x and y => width and height.  arg = 0 windowed  arg = 1 fullscreen
};

enum class ModKey
{
    Shift = 1 << 0,
    Alt   = 1 << 1,
    Ctrl  = 1 << 2
};

class Color
{
public:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a{255};
public:
    constexpr Color(int c) : r((c >> 16)&0xFF), g((c >> 8)&0xFF), b(c&0xFF), a(0xFF) {}
    constexpr Color()  : r(0), g(0), b(0), a(255) {}
    constexpr Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255)  : r(_r), g(_g), b(_b), a(_a) {}
    constexpr Color(int _r, int _g, int _b, int _a = 255)  : r(_r), g(_g), b(_b), a(_a) {}
    unsigned int toUIntRGBA() const { return a & (b << 8) & (g << 16) & (r << 24); }
    int toIntRGB() const { return b & (g << 8) & (r << 16); }
};
#undef TRANSPARENT

constexpr Color TRANSPARENT(0,0,0,0);
constexpr Color TRANS(0,0,0,0);
constexpr Color WHITE(255,255,255);
constexpr Color GREY(128,128,128);
constexpr Color BLACK(0,0,0);
constexpr Color RED(255,0,0);
constexpr Color GREEN(0,255,0);
constexpr Color BLUE(0,0,255);
constexpr Color YELLOW(255,255,0);
constexpr Color PURPLE(255,0,255);
constexpr Color CYAN(0,255,255);
constexpr Color ORANGE(255,165,0);
constexpr Color LTGREY(211,211,211);

Color hsv2rgb(double h, double s, double v);
void  rgb2hsv(Color c, double &h, double &s, double &v);

class Event
{
public:
    EvtType evtType;
    int     x;
    int     y;
    ModKey  mods;
    int     arg;
public:
    Vec2d mousePos() const { return Vec2d(x, y); }
    bool hasCtrl() const { return static_cast<int>(mods) & static_cast<int>(ModKey::Ctrl);  }
    bool hasAlt() const { return static_cast<int>(mods) & static_cast<int>(ModKey::Alt);   }
    bool hasShift() const { return static_cast<int>(mods) & static_cast<int>(ModKey::Shift); }
    Key  key() const { return Key(arg); }
    MouseButton  mouseButton() const { return static_cast<MouseButton>(arg); }
};

std::ostream& operator<<(std::ostream& os, const Event& evt);

class Grob;

class ImageInternal {
    NVGcontext* vg;
    int vgImageIdx;
    int width{0};
    int height{0};
    Color* pixels{nullptr};
public:
    ImageInternal(NVGcontext* vg, int idx, int width, int height, Color* cached);
   ~ImageInternal();
private:
    void freeCachedPixels();
    void updatePixels();
    void setPixel(int x, int y, Color c) {
       pixels[y*width+x] = c;
    }
    Color getPixel(int x, int y) {
        return pixels[y*width+x];
    }
    friend class Image;
    friend class Graphics;
};

class Graphics;

class Image
{
private:
    NVGcontext* vg;
    std::shared_ptr<ImageInternal> pixmap;
public:
    Image(mssm::Graphics& g);
    Image(mssm::Graphics& g, int width, int height, Color c, bool cachePixels = false);
    Image(mssm::Graphics& g, const std::string& filename, bool cachePixels = false);
    void set(int width, int height, Color c, bool cachePixels = false);
    void load(const std::string& fileName, bool cachePixels = false);
    void save(const std::string& pngFileName);
    Color* pixels() { return pixmap->pixels; } // changes won't take effect until updatePixels
    void  setPixel(int x, int y, Color c) { pixmap->setPixel(x, y, c); } // won't take effect until updatePixels
    Color getPixel(int x, int y)          { return pixmap->getPixel(x, y); }
    void updatePixels() { pixmap->updatePixels(); }
    int width() const;
    int height() const;
    friend class Graphics;
};

typedef unsigned int SoundHandle ;

#ifdef INCLUDE_SOUND
class SoundInternal
{
private:
    std::string filename;
    SoLoud::AudioSource *source{nullptr};
    SoLoud::Soloud& player;
public:
    SoundInternal(SoLoud::Soloud& player, const std::string& filename, bool stream = false);
   ~SoundInternal();
private:
    SoundHandle play(double volume);
    bool playing(SoundHandle handle);
    void stop(SoundHandle handle);
    friend class Graphics;
    friend class Sound;
};
#else
class SoundInternal
{

};
#endif

class Sound
{
private:
    std::shared_ptr<SoundInternal> sound;
public:
    Sound(mssm::Graphics& g, const std::string& filename);
    friend class Graphics;
};

class TextExtents {
public:
    float fontAscent;
    float fontDescent;
    float fontHeight;
    float textHeight;
    float textWidth;
    float textAdvance;
};

enum class FontFace {
    Roboto,
    RobotoBold,
    RobotoLight,
    User1,
    User2,
    User3,
    User4,
    User5,
    User6,
    User7,
    User8,
    User9,
    User10
};

class FontInfo {
private:
    int  size;
    FontFace face{FontFace::Roboto};
public:
    FontInfo(int size) : size{size} { }
    FontInfo(FontFace face, int size) : size{size}, face{face} { }
    friend class Graphics;
private:
    int getNvgFontFaceId(Graphics& g) const;
public:
    static FontInfo Bold(int size) { return FontInfo{FontFace::RobotoBold, size}; }
    static FontInfo Light(int size) { return FontInfo{FontFace::RobotoLight, size}; }
    static FontInfo Normal(int size) { return FontInfo{FontFace::Roboto, size}; }
};

class Canvas {
public:
    virtual int    width() = 0;
    virtual int    height() = 0;
    virtual void   setBackground(Color c) = 0;
    virtual void   line(Vec2d p1, Vec2d p2, Color c = WHITE) = 0;
    virtual void   ellipse(Vec2d center, double w, double h, Color c = WHITE, Color f = TRANSPARENT) = 0;
    virtual void   arc(Vec2d center, double w, double h, double a, double alen, Color c = WHITE) = 0;
    virtual void   chord(Vec2d center, double w, double h, double a, double alen, Color c = WHITE, Color f = TRANSPARENT) = 0;
    virtual void   pie(Vec2d center, double w, double h, double a, double alen, Color c = WHITE, Color f = TRANSPARENT) = 0;
    virtual void   rect(Vec2d corner, double w, double h, Color c = WHITE, Color f = TRANSPARENT) = 0;
    virtual void   polygon(const std::vector<Vec2d>& points, Color border, Color fill = TRANSPARENT) = 0;
    virtual void   polyline(const std::vector<Vec2d>& points, Color color) = 0;
    virtual void   points(const std::vector<Vec2d>& points, Color c) = 0;
    virtual void   polygon(const Array<Vec2d>& points, Color border, Color fill = TRANSPARENT) = 0;
    virtual void   polyline(const Array<Vec2d>& points, Color color) = 0;
    virtual void   points(const Array<Vec2d>& points, Color c) = 0;
    virtual void   polygon(std::initializer_list<Vec2d> points, Color border, Color fill = TRANSPARENT) = 0;
    virtual void   polyline(std::initializer_list<Vec2d> points, Color color) = 0;
    virtual void   points(std::initializer_list<Vec2d> points, Color c) = 0;
    virtual void   text(Vec2d pos, const FontInfo& sizeAndFace, const std::string& str, Color textColor = WHITE, HAlign hAlign = HAlign::left, VAlign vAlign = VAlign::baseline) = 0;
    virtual void   textExtents(double size, const std::string& str, TextExtents& extents) = 0;
    virtual double textWidth(double size, const std::string& str) = 0;
    virtual void   point(Vec2d pos, Color c) = 0;
    virtual void   image(Vec2d pos, const Image& img) = 0;
    virtual void   image(Vec2d pos, const Image& img, Vec2d src, int srcw, int srch) = 0;
    virtual void   image(Vec2d pos, double w, double h, const Image& img) = 0;
    virtual void   image(Vec2d pos, double w, double h, const Image& img, Vec2d src, int srcw, int srch) = 0;
    virtual void   imageC(Vec2d center, double angle, const Image& img) = 0;
    virtual void   imageC(Vec2d center, double angle, const Image& img, Vec2d src, int srcw, int srch) = 0;
    virtual void   imageC(Vec2d center, double angle, double w, double h, const Image& img) = 0;
    virtual void   imageC(Vec2d center, double angle, double w, double h, const Image& img, Vec2d src, int srcw, int srch) = 0;
};

class Graphics : public Canvas
{
    enum KeyState {
        up,            // still up (2nd and subsequent frames after release)
        justPressed,   // just pressed this frame
        down,          // still down (2nd and subsequent frames after press)
        justReleased,  // just released this frame
    };

private:
    GLFWwindow* window{nullptr};

#ifdef _WIN32
    HDC hdc;
#endif

    NVGcontext* vg{nullptr};

    Rand rnd;
    std::function<void (Graphics&)> mainFunc;

#ifdef INCLUDE_SOUND
    SoLoud::Soloud    soundPlayer;
    SoLoud::WavStream musicStream;
    SoundHandle       musicHandle{0};
#endif

    std::string       musicFile;
    double            musicVolume{-1}; // -1 default (full volume)  0 to 1 otherwise

    int fontRegular;
    int fontBold;
    int fontLight;

    std::vector<Event> _events;
    std::vector<Event> _cachedEvents;
    std::string title;

    bool        closed{false};

    bool        requestToggleFullScreen{false};

    std::chrono::microseconds::rep start_time;
    std::chrono::steady_clock::time_point lastDrawTime;
    std::chrono::microseconds::rep elapsed;

    int         currentWidth{100};
    int         currentHeight{100};

    bool        gotResizeEvent{false};

    int         windowedX{0};
    int         windowedY{0};
    int         windowedWidth;
    int         windowedHeight;

    mssm::Color background;

    int minKeyStateIdx; // minimum idx of keyState that was changed this update cycle
    int maxKeyStateIdx;                  // max idx of keyState that was changed this update cycle
    std::vector<KeyState> keyState;

    int minMouseStateIdx; // minimum idx of keyState that was changed this update cycle
    int maxMouseStateIdx;                  // max idx of keyState that was changed this update cycle
    std::vector<KeyState> mouseState;

    Key pressedKey{Key::None}; // most recent key pressed, but one frame only
    MouseButton pressedButton{MouseButton::None};      // most recent mouse button pressed, but one frame only
    int wheelDelta{0};

    std::string stringOutput;
    std::function<std::string()> getInputText;
    Vec2d       _mousePos; // mouse pos at time of last screen repaint

    void postEvent(int x, int y, EvtType evtType, ModKey mods, int arg);

    std::vector<std::shared_ptr<ImageInternal>> keepImages;

public:
    Graphics(std::string title, int width, int height,
             std::function<void (Graphics&)> mainThreadFunc = nullptr);


    ~Graphics();
public:



    std::stringstream cout;
    std::stringstream cerr;
    std::deque<std::string> cerrLines;

    template<typename... _Args>
    inline void
    print(fmt::format_string<_Args...> __fmt, _Args&&... __args)
    { cout << fmt::vformat(__fmt.get(), fmt::make_format_args(__args...)); }

    template<typename... _Args>
    inline void
    println(fmt::format_string<_Args...> __fmt, _Args&&... __args)
    { cout << fmt::vformat(__fmt.get(), fmt::make_format_args(__args...)) << endl; }

    template<typename... _Args>
    inline void
    printError(fmt::format_string<_Args...> __fmt, _Args&&... __args)
    { cerr << fmt::vformat(__fmt.get(), fmt::make_format_args(__args...)) << endl; }

    NVGcontext* vgContext() { return vg; }

#ifdef INCLUDE_SOUND
    SoLoud::Soloud& getSoundPlayer() { return soundPlayer; }
#endif

    std::chrono::milliseconds::rep time();

    void test();

    void waitUntilClosed();

    double elapsedMs() { return elapsed/1000.0; }

    int    width() override  { return currentWidth; }
    int    height() override { return currentHeight; }

    Vec2d  mousePos();

    void   setBackground(Color c) override { background = c; }

    void   setClip(Vec2d pos, double w, double h);
    void   clearClip();

    void   line(Vec2d p1, Vec2d p2, Color c = WHITE) override;
    void   ellipse(Vec2d center, double w, double h, Color c = WHITE, Color f = TRANSPARENT) override;
    void   arc(Vec2d center, double w, double h, double a, double alen, Color c = WHITE) override;
    void   chord(Vec2d center, double w, double h, double a, double alen, Color c = WHITE, Color f = TRANSPARENT) override;
    void   pie(Vec2d center, double w, double h, double a, double alen, Color c = WHITE, Color f = TRANSPARENT) override;
    void   rect(Vec2d corner, double w, double h, Color c = WHITE, Color f = TRANSPARENT) override;

    void   polygon(const std::vector<Vec2d>& points, Color border, Color fill = TRANSPARENT) override;
    void   polyline(const std::vector<Vec2d>& points, Color color) override;
    void   points(const std::vector<Vec2d>& points, Color c) override;

    void   polygon(const Array<Vec2d>& points, Color border, Color fill = TRANSPARENT) override;
    void   polyline(const Array<Vec2d>& points, Color color) override;
    void   points(const Array<Vec2d>& points, Color c) override;

    void   polygon(std::initializer_list<Vec2d> points, Color border, Color fill = TRANSPARENT) override;
    void   polyline(std::initializer_list<Vec2d> points, Color color) override;
    void   points(std::initializer_list<Vec2d> points, Color c) override;

    void   text(Vec2d pos, const FontInfo& sizeAndFace, const std::string& str, Color textColor = WHITE, HAlign hAlign = HAlign::left, VAlign vAlign = VAlign::baseline) override;

    void   textExtents(double size, const std::string& str, TextExtents& extents) override;
    double textWidth(double size, const std::string& str) override;

    void   point(Vec2d pos, Color c) override;

    void   image(Vec2d pos, const Image& img) override;
    void   image(Vec2d pos, const Image& img, Vec2d src, int srcw, int srch) override;
    void   image(Vec2d pos, double w, double h, const Image& img) override;
    void   image(Vec2d pos, double w, double h, const Image& img, Vec2d src, int srcw, int srch) override;

    void   imageC(Vec2d center, double angle, const Image& img) override;
    void   imageC(Vec2d center, double angle, const Image& img, Vec2d src, int srcw, int srch) override;
    void   imageC(Vec2d center, double angle, double w, double h, const Image& img) override;
    void   imageC(Vec2d center, double angle, double w, double h, const Image& img, Vec2d src, int srcw, int srch) override;

    SoundHandle play(Sound sound, double volume = -1);  // volume 0 to 1
    bool   isPlaying(SoundHandle handle);
    void   music(const std::string& filename, double volume = -1);
    void   stopMusic();
    void   setMusicVolume(double volume);
    bool   isMusicPlaying();

    bool   draw();
    void   drawFromStream(std::stringstream& ss, std::deque<std::string>& lines, Vec2d start, Color c);

    bool   isClosed();

    double timeMs();

    bool   isKeyPressed(Key c) const { return keyState[(int)c] == KeyState::down || keyState[(int)c] == KeyState::justPressed; }
    bool   isKeyPressed(char c) const { return isKeyPressed(Key{std::isalpha(c) ? std::toupper(c) : c}); }
    bool   isKeyPressed(Key::KeyConst c) const { return isKeyPressed(Key{c}); }

    bool   onKeyPress(Key c) const { return keyState[(int)c] == KeyState::justPressed; }
    bool   onKeyPress(char c) const { return onKeyPress(Key{std::isalpha(c) ? std::toupper(c) : c}); }
    bool   onKeyPress(Key::KeyConst c) const { return onKeyPress(Key{c}); }

    bool   onKeyRelease(Key c) const { return keyState[(int)c] == KeyState::justReleased; }
    bool   onKeyRelease(char c) const { return onKeyRelease(Key{std::isalpha(c) ? std::toupper(c) : c}); }
    bool   onKeyRelease(Key::KeyConst c) const { return onKeyRelease(Key{c}); }

    bool   isShiftKeyPressed() const { return isKeyPressed(Key::LeftShift) || isKeyPressed(Key::RightShift); }
    bool   isCtrlKeyPressed() const { return isKeyPressed(Key::LeftCtrl) || isKeyPressed(Key::RightCtrl); }
    bool   isAltKeyPressed() const { return isKeyPressed(Key::LeftAlt) || isKeyPressed(Key::RightAlt); }

    bool   isMousePressed(MouseButton button) const { return mouseState[(int)button] == KeyState::down || mouseState[(int)button] == KeyState::justPressed; }
    bool   onMousePress(MouseButton button) const { return mouseState[(int)button] == KeyState::justPressed; }
    bool   onMouseRelease(MouseButton button) const { return mouseState[(int)button] == KeyState::justReleased; }

    Key    getKeyPressed()   { return pressedKey; }
    MouseButton getMousePressed() { return pressedButton; }
    int    getWheelDelta()   { return wheelDelta; }
    int    getDigitPressed();

    std::vector<Event> events();

    int    randomInt(int minVal, int maxVal) { return rnd.randomInt(minVal, maxVal); }
    double randomDouble(double minVal, double maxVal) { return rnd.randomDouble(minVal, maxVal); }
    bool   randomTrue(double pct) { return rnd.randomTrue(pct); }

    std::string currentPath(const std::string& file = "");
    std::string programName();

private:
  //  void draw(/*QWidget *pd, */QNanoPainter *painter, int width, int height, double uiFuncElapsed);
    std::string getTitle() { return title; }
    void setClosed();


    std::string getOutputText();
    bool appendOutputText(const std::string& txt);
    void setInputTextFunc(std::function<std::string()> func) { getInputText = func; }

    void setMousePos(int x, int y);

    void toggleFullScreen() { requestToggleFullScreen = true; }

    friend void key(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void mousePosCallback(GLFWwindow* window, double x, double y);
    friend void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    friend void scrollWheelCallback(GLFWwindow* window, double /*sx*/, double sy);
    friend void windowSizeCallback(GLFWwindow* window,int width,int height);

    void resetKeyChangeIdx();
    void updateKeyChangeIdx(int idx);

    void resetMouseChangeIdx();
    void updateMouseChangeIdx(int idx);

    template<typename T>
    void t_polygon(T points, Color border, Color fill = TRANSPARENT);

    template<typename T>
    void t_polyline(T points, Color color);

    template<typename T>
    void t_points(T points, Color c);
public:
    friend class FontInfo;
};

// see format_helper.h
auto format_value(const Key& k, auto i) {
    return fmt::format_to(i, "{}", k.toFormatString());
}

auto format_value(const MouseButton& k, auto i) {
    return fmt::format_to(i, "{}", k.toFormatString());
}

auto format_value(const Color& c, auto i) {
    return fmt::format_to(i, "0x{:02X}{:02X}{:02X}", c.r, c.g, c.b);
}

}



#endif // GROBS_H
