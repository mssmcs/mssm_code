#ifndef GROBS_H
#define GROBS_H

#include <deque>
#include <functional>
#include <sstream>
#include <string>
#include "paths.h"

#include "array.h"
#include "canvas2d.h"
#include "image.h"
#include "rand.h"
#include "soundengine.h"

#include "graphicsbackend.h"
#include "windowevents.h"

void splitRows(std::deque<std::string>& lines, const std::string& str, size_t keepLast);

namespace mssm {
//class GraphicsBase;
class SoundHost;

class Sound
{
private:
    SoundHandle snd;

public:
    Sound(SoundHost &g, const std::string &filename);
    friend class SoundHost;
};

using Key = mssm::Key;
using MouseButton = mssm::MouseButton;
using Event = mssm::Event;
using EvtType = mssm::EvtType;
using ModKey = mssm::ModKey;

class SoundHost {
protected:

    SoundEngine soundEngine;
    SoundHandle musicHandle{0};
public:
    void play(Sound sound, double volume = -1); // volume 0 to 1
    void music(const std::string &filename, double volume = -1);
    void stopMusic();
    void setMusicVolume(double volume);
protected:
    SoundEngine &getSoundPlayer() { return soundEngine; }
    friend class Sound;
};


template <typename WINDOW, typename CANVAS>
class GraphicsBase : public SoundHost, public mssm::EvtSourceWrapper
{
protected:
    GraphicsBackendBase<WINDOW, CANVAS> *backend{};
    WINDOW *window{};
    CANVAS *canvas{};
public:
    GraphicsBase(std::string title, int width, int height, std::function<GraphicsBackendBase<WINDOW, CANVAS> *(std::string title, int width, int height)> loadGraphicsBackend);
    virtual ~GraphicsBase() {}

    mssm::EventSource &evtSource() { return *eventSource; }
    mssm::CoreWindow *getWindow() { return window; }

    std::stringstream cout;
    std::stringstream cerr;
    std::deque<std::string> cerrLines;

// Helper function to format a string with either std or fmt
#if MSSM_HAS_CPP23_PRINT
    template<typename... Args>
    inline std::string format_helper(std::format_string<Args...> fmt_str, Args&&... args) {
        return std::vformat(fmt_str.get(), std::make_format_args(args...));
    }
#else
    template<typename... Args>
    inline std::string format_helper(fmt::format_string<Args...> fmt_str, Args&&... args) {
        return fmt::vformat(fmt_str.get(), fmt::make_format_args(args...));
    }
#endif

    // Common print methods using the helper
    template<typename... Args>
    inline void print(
#if MSSM_HAS_CPP23_PRINT
        std::format_string<Args...> fmt,
#else
        fmt::format_string<Args...> fmt,
#endif
        Args&&... args)
    {
        this->cout << format_helper(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void println(
#if MSSM_HAS_CPP23_PRINT
        std::format_string<Args...> fmt,
#else
        fmt::format_string<Args...> fmt,
#endif
        Args&&... args)
    {
        this->cout << format_helper(fmt, std::forward<Args>(args)...) << std::endl;
    }

    template<typename... Args>
    inline void printError(
#if MSSM_HAS_CPP23_PRINT
        std::format_string<Args...> fmt,
#else
        fmt::format_string<Args...> fmt,
#endif
        Args&&... args)
    {
        std::string formatted = format_helper(fmt, std::forward<Args>(args)...);
        //this->cerr << formatted << std::endl;
        cerrLines.push_back(formatted);
    }

    // Empty format string overload for println
    inline void println()
    {
        this->cout << std::endl;
    }

    Vec2d mousePos() const { return eventSource->mousePos<Vec2d>(); }

    bool draw();
    void drawFromStream(std::stringstream &ss, std::deque<std::string> &lines, Vec2d start, Color c);

    bool isClosed() const;

    Vec2d mouseDragStart(MouseButton button) const { return eventSource->mouseDragStart<Vec2d>(button); }

    const std::vector<Event>& events() { return eventSource->events(); }

    int randomInt(int minVal, int maxVal) { return ::randomInt(minVal, maxVal); }
    double randomDouble(double minVal, double maxVal) { return ::randomDouble(minVal, maxVal); }
    bool randomTrue(double pct) { return ::randomTrue(pct); }

};


template <typename WINDOW, typename CANVAS>
mssm::GraphicsBase<WINDOW, CANVAS>::GraphicsBase(std::string title,
                                                 int width, int height,
                                                 std::function<GraphicsBackendBase<WINDOW, CANVAS>*(std::string title, int width, int height)> loadGraphicsBackend)
    : mssm::EvtSourceWrapper{{}}
{
    backend = loadGraphicsBackend(title, width, height);
    canvas = backend->getCanvas1();
    window = backend->getWindow1();
    eventSource = backend->eventSource();
}

template <typename WINDOW, typename CANVAS>
void mssm::GraphicsBase<WINDOW, CANVAS>::drawFromStream(std::stringstream& ss, std::deque<std::string>& lines, Vec2d start, Color c)
{
    std::string tmp = ss.str();
    if (!tmp.empty()) {
        splitRows(lines, tmp, (canvas->height()-40)/25);
        ss.str("");
    }

    double y = start.y;
    for (const auto& row : lines) {
        canvas->text({start.x, y}, 20, row, c);
        y += 25;
    }
}

template <typename WINDOW, typename CANVAS>
bool mssm::GraphicsBase<WINDOW, CANVAS>::draw()
{
    std::deque<std::string> lines;
    drawFromStream(cout, lines, { 10, 20 }, PURPLE);
    drawFromStream(cerr, cerrLines, { canvas->width()/2, 20 }, RED);
    return window->update(true);
}



} // namespace mssm

#endif // GROBS_H
