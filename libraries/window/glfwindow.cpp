#include "glfwindow.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// todo: remove, only for opengl (drop support for opengl?)

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <windows.h>
//#include <minwindef.h>
#endif

using namespace std;
using namespace mssm;

// UGH! global
unsigned int g_CurrentMods = 0;

void errorcb(int error, const char* desc)
{
    printf("GLFW error %d: %s\n", error, desc);
    throw std::runtime_error(std::string("GLFW error ") + to_string(error) + " " + desc);
}


// thanks to: https://stackoverflow.com/users/514130/shmo
namespace mssm {


// get required vulkan extensions for use with glfw
std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}


string Key::toFormatString() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str(); // refactor if performance is an issue.  It's like this because << was implemented before format support
}

string MouseButton::toFormatString() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str(); // refactor if performance is an issue.  It's like this because << was implemented before format support
}


std::ostream& operator<<(std::ostream& os, const Event& evt)
{
    os << "Evt: ";

    switch (evt.evtType) {
    case EvtType::KeyPress:     os << "KeyPress    "; break;
    case EvtType::KeyRelease:   os << "KeyRelease  "; break;
    case EvtType::KeyRepeat:    os << "KeyRepeat   "; break;
    case EvtType::MouseMove:    os << "MouseMove   "; break;
    case EvtType::MousePress:   os << "MousePress  "; break;
    case EvtType::MouseRelease: os << "MouseRelease"; break;
    case EvtType::MouseWheel:   os << "MouseWheel  "; break;
    case EvtType::MusicEvent:   os << "MusicEvent  "; break;
    case EvtType::WindowResize: os << "WindowResize"; break;
    }

    os << " x: " << setw(5) << evt.x << " y: " << setw(5) << evt.y << " arg: " << evt.arg;

    if (static_cast<int>(evt.mods) & static_cast<int>(ModKey::Ctrl))  { os << " <CTRL>";  }
    if (static_cast<int>(evt.mods) & static_cast<int>(ModKey::Alt))   { os << " <ALT>";   }
    if (static_cast<int>(evt.mods) & static_cast<int>(ModKey::Shift)) { os << " <SHIFT>"; }

    //os << " data: " << evt.data << " pluginId: " << evt.pluginId;

    return os;
}

std::ostream& operator<<(std::ostream& os, const Key& k)
{
    switch (k) {
    case Key::None:       os <<  "Key::None"; break;
    case Key::Left:       os <<  "Key::Left"; break;
    case Key::Right:      os <<  "Key::Right"; break;
    case Key::Up:         os <<  "Key::Up"; break;
    case Key::Down:       os <<  "Key::Down"; break;
    case Key::LeftShift:  os <<  "Key::LeftShift"; break;
    case Key::LeftCtrl:   os <<  "Key::LeftCtrl"; break;
    case Key::LeftAlt:    os <<  "Key::LeftAlt"; break;
    case Key::RightShift: os <<  "Key::RightShift"; break;
    case Key::RightCtrl:  os <<  "Key::RightCtrl"; break;
    case Key::RightAlt:   os <<  "Key::RightAlt"; break;
    case Key::ESC:        os <<  "Key::ESC"; break;
    case Key::ENTER:      os <<  "Key::ENTER"; break;
    case Key::PageUp:     os <<  "Key::PageUp"; break;
    case Key::PageDown:   os <<  "Key::PageDown"; break;
    case Key::Space:      os <<  "Key::Space"; break;
    case Key::Tab:        os <<  "Key::Tab"; break;
    case Key::Backspace:  os <<  "Key::Backspace"; break;
    case Key::Delete:     os <<  "Key::Delete"; break;
    case Key::Insert:     os <<  "Key::Insert"; break;

    default:
    {
        if (k.keyCode() <= 255 && std::isprint((int)k)) {
            os << (char)k;
        }
        else {
            os <<  "Key::UNKNOWN(" << k.keyCode() << ")";
        }
        break;
    }
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const MouseButton& k)
{
    switch (k) {
    case MouseButton::None:       os <<  "MouseButton::None"; break;
    case MouseButton::Left:       os <<  "MouseButton::Left"; break;
    case MouseButton::Right:      os <<  "MouseButton::Right"; break;
    case MouseButton::Middle:     os <<  "MouseButton::Middle"; break;
    default:                      os <<  "MouseButton::" << ((int)k + 1); break;
    }
    return os;
}


GLFWmonitor* get_current_monitor(GLFWwindow *window)
{
    int nmonitors, i;
    int wx, wy, ww, wh;
    int mx, my, mw, mh;
    int overlap, bestoverlap;
    GLFWmonitor *bestmonitor;
    GLFWmonitor **monitors;
    const GLFWvidmode *mode;

    bestoverlap = 0;
    bestmonitor = NULL;

    glfwGetWindowPos(window, &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);
    monitors = glfwGetMonitors(&nmonitors);

    for (i = 0; i < nmonitors; i++) {
        mode = glfwGetVideoMode(monitors[i]);
        glfwGetMonitorPos(monitors[i], &mx, &my);
        mw = mode->width;
        mh = mode->height;

        overlap =
            max(0, min(wx + ww, mx + mw) - max(wx, mx)) *
            max(0, min(wy + wh, my + mh) - max(wy, my));

        if (bestoverlap < overlap) {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }

    if (!bestmonitor) {
        // not sure that this could even happen, but let's be safe
        bestmonitor = glfwGetPrimaryMonitor();
    }

    return bestmonitor;
}


ModKey cvtMods(int glfwMods)
{
    ModKey mods = static_cast<ModKey>(
        ((glfwMods & GLFW_MOD_CONTROL) ? (int)ModKey::Ctrl  : 0) |
        ((glfwMods & GLFW_MOD_ALT)     ? (int)ModKey::Alt   : 0) |
        ((glfwMods & GLFW_MOD_SHIFT)   ? (int)ModKey::Shift : 0));

    return mods;
}


void key(GLFWwindow* window, int key, int /*scancode*/, int action, int mods)
{
    g_CurrentMods = mods;

    WindowEventSink* g = reinterpret_cast<WindowEventSink*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ENTER &&
        mods & GLFW_MOD_ALT &&
        action == GLFW_PRESS) {
        g->toggleFullScreen();
    }

    switch (action) {
    case GLFW_PRESS:
        g->postEvent(g->currMousePosX(), g->currMousePosY(), EvtType::KeyPress, cvtMods(mods), key);
        break;
    case GLFW_RELEASE:
        g->postEvent(g->currMousePosX(), g->currMousePosY(), EvtType::KeyRelease, cvtMods(mods), key);
        break;
    case GLFW_REPEAT:
        g->postEvent(g->currMousePosX(), g->currMousePosY(), EvtType::KeyRepeat, cvtMods(mods), key);
        break;
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    WindowEventSink* g = reinterpret_cast<WindowEventSink*>(glfwGetWindowUserPointer(window));

    switch (action) {
    case GLFW_PRESS:
        //cout << "Click: " << mp.x << " " << mp.y << endl;
        g->postEvent(g->currMousePosX(), g->currMousePosY(), EvtType::MousePress, cvtMods(mods), button);
        break;
    case GLFW_RELEASE:
        g->postEvent(g->currMousePosX(), g->currMousePosY(), EvtType::MouseRelease, cvtMods(mods), button);
        break;
    }
}

void mousePosCallback(GLFWwindow* window, double x, double y)
{
    WindowEventSink* g = reinterpret_cast<WindowEventSink*>(glfwGetWindowUserPointer(window));

    g->setMousePos(x,y);

    int buttons =
        (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ? 1 : 0) +
        (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) ? 2 : 0) +
        (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) ? 4 : 0);

    if (buttons > 0)
    {
        g->postEvent(x, y, EvtType::MouseMove, cvtMods(g_CurrentMods), buttons);
    }
}

void scrollWheelCallback(GLFWwindow* window, double /*sx*/, double sy)
{
    WindowEventSink* g = reinterpret_cast<WindowEventSink*>(glfwGetWindowUserPointer(window));
    //cout << "Scroll: " << sy << endl;

    g->postEvent(g->currMousePosX(), g->currMousePosY(), EvtType::MouseWheel, cvtMods(0), sy < 0 ? -1 : 1);
}

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

void windowPosCallback(GLFWwindow* window,int x,int y)
{
    //EventPoster* g = reinterpret_cast<EventPoster*>(glfwGetWindowUserPointer(window));
    //::cout << "windowPosCallback " << x << " " << y << endl;
}

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    //cout << "windowSizeCallback: " << width << " " << height << endl;
    WindowEventSink* g = reinterpret_cast<WindowEventSink*>(glfwGetWindowUserPointer(window));
    g->setGotResize(width, height);
}

void windowCloseCallback(GLFWwindow* window)
{
    //EventPoster* g = reinterpret_cast<EventPoster*>(glfwGetWindowUserPointer(window));
    //::cout << "windowCloseCallback" << endl;
}

void windowRefreshCallback(GLFWwindow* window)
{
    //EventPoster* g = reinterpret_cast<EventPoster*>(glfwGetWindowUserPointer(window));
    //::cout << "windowRefreshCallback" << endl;
}


void windowFocusCallback(GLFWwindow* window, int hasFocus)
{
    //EventPoster* g = reinterpret_cast<EventPoster*>(glfwGetWindowUserPointer(window));
    //::cout << "windowFocusCallback " << hasFocus << endl;
}

void windowIconifyCallback(GLFWwindow* window, int isIconified)
{
    //EventPoster* g = reinterpret_cast<EventPoster*>(glfwGetWindowUserPointer(window));
    //::cout << "windowIconifyCallback " << isIconified << endl;
}

void windowMaximizeCallback(GLFWwindow* window, int maxArg)
{
    //EventPoster* g = reinterpret_cast<EventPoster*>(glfwGetWindowUserPointer(window));
    //::cout << "windowMaximizeCallback " << maxArg << endl;
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    //EventPoster* g = reinterpret_cast<EventPoster*>(glfwGetWindowUserPointer(window));
    //::cout << "frameBufferSizeCallback " << width << " " << height << endl;
}

void windowContentScaleCallback(GLFWwindow* window, float xArg, float yArg)
{
    //EventPoster* g = reinterpret_cast<EventPoster*>(glfwGetWindowUserPointer(window));
    //::cout << "windowContentScaleCallback " << xArg << " " << yArg << endl;
}

GLFWwindow *buildWindow(WindowEventSink* owner, int width, int height, string title, bool isOpenGl)
{


    if (!isOpenGl) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
    else {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

#ifndef _WIN32 // don't require this on win32, and works with more cards
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        
        #if defined(__linux__) && defined(__WSLERNEL__)
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
        #else
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #endif
#endif    

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE );
        //   glfwWindowHint(GLFW_SAMPLES, 2);
    }



    //    window =   glfwCreateWindow(glfwGetVideoMode(glfwGetPrimaryMonitor())->width,
    //                                               glfwGetVideoMode(glfwGetPrimaryMonitor())->height, "My Title",
    //                                               glfwGetPrimaryMonitor(), nullptr);

    GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    //	window = glfwCreateWindow(1000, 600, "NanoVG", glfwGetPrimaryMonitor(), NULL);
    if (!window) {
        std::cerr << "Window creation failed: " << glfwGetError(nullptr) << std::endl;
        return nullptr;
    }

    // #ifdef _WIN32
    //     hdc = GetDC(glfwGetWin32Window(window));
    // #endif

    //glfwSetWindowIcon(window, count, icons);

    glfwSetWindowUserPointer(window, owner);

    glfwSetKeyCallback(window, key);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetScrollCallback(window, scrollWheelCallback);

    glfwSetWindowPosCallback (window, windowPosCallback);
    glfwSetWindowSizeCallback (window, windowSizeCallback);
    glfwSetWindowCloseCallback (window, windowCloseCallback);
    glfwSetWindowRefreshCallback (window, windowRefreshCallback);
    glfwSetWindowFocusCallback (window, windowFocusCallback);
    glfwSetWindowIconifyCallback (window, windowIconifyCallback);
    glfwSetWindowMaximizeCallback (window, windowMaximizeCallback);
    glfwSetFramebufferSizeCallback (window, frameBufferSizeCallback);
    glfwSetWindowContentScaleCallback (window, windowContentScaleCallback);

    if (isOpenGl) {
        glfwMakeContextCurrent(window);

        glfwSwapInterval(1); // supportTear ? -1 : 1);

#ifdef _WIN32
        // Turn on vertical screen sync under Windows.
        // (I.e. it uses the WGL_EXT_swap_control extension)
        typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
        wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(glfwGetProcAddress("wglSwapIntervalEXT"));
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
        if(wglSwapIntervalEXT) {
            wglSwapIntervalEXT(1);
        }
#endif
    }


    //	initGPUTimer(&gpuTimer);

    glfwSetTime(0);

    return window;
}

CoreWindowGLFW::CoreWindowGLFW(std::string title, int width, int height, bool isOpenGL)
    : CoreWindow(title, width, height), isOpenGL{isOpenGL}
{
    windowedWidth = width;
    windowedHeight = height;

    glfwSetErrorCallback(errorcb);

    if (!glfwInit()) {
        throw std::runtime_error("Failed to init GLFW.");
    }
}


CoreWindowGLFW::~CoreWindowGLFW()
{
    if (window) {
        glfwDestroyWindow(window);
    }

    for (auto& c : cursors) {
        glfwDestroyCursor(c.second);
    }
}

void CoreWindowGLFW::configure()
{
    CoreWindow::configure(); // CALL BASE CLASS CONFIGURE!
    window = buildWindow(this, width(), height(), getTitle(), isOpenGL);
}


bool CoreWindowGLFW::shouldClose() const
{
    return glfwWindowShouldClose(window);
}

void CoreWindowGLFW::handleToggleFullScreen()
{
    if (glfwGetWindowMonitor(window)) {
        // it was full screen (otherwise glfwGetWindowMonitor would have returned null)
        // switch to windowed mode
        //::cout << "Window Mode: " << windowedX << " " << windowedY << " " << currentWidth << " " << currentHeight << endl;
        glfwSetWindowMonitor(window, NULL, windowedX, windowedX, width(), height(), 0);
    }
    else
    {
        // we were windowed and are switching to full screen
        GLFWmonitor* monitor = get_current_monitor(window);  // get the monitor the window is most on

        // save the windowed confuration for later if we go out of full screen
        glfwGetWindowPos(window, &windowedX, &windowedY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        //::cout << "Leaving windowed Mode: " << windowedX << " " << windowedY << " " << windowedWidth << " " << windowedHeight << endl;

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

        //::cout << "FullScreen Mode: " << mode->width << " " << mode->height << endl;
        //::cout << "Refresh: " << mode->refreshRate << endl;
    }
}

void CoreWindowGLFW::pollEvents()
{
    if (isOpenGL) {
        glfwSwapBuffers(window);
    }
    glfwPollEvents();
}

} // namespace mssm


void mssm::CoreWindowGLFW::setCursor(CoreWindowCursor cursor)
{
    if (cursor == activeCursor) {
        return;
    }
    auto c = cursors[cursor];
    if (c == nullptr) {
        switch (cursor) {
        case mssm::CoreWindowCursor::arrow:
            c = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
            break;
        case mssm::CoreWindowCursor::iBeam:
            c = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
            break;
        case mssm::CoreWindowCursor::crosshair:
            c = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
            break;
        case mssm::CoreWindowCursor::hand:
            c = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
            break;
        case mssm::CoreWindowCursor::hresize:
            c = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
            break;
        case mssm::CoreWindowCursor::vresize:
            c = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
            break;
        }
        cursors[cursor] = c;
    }

    glfwSetCursor(window, c);    
    activeCursor = cursor;
}
