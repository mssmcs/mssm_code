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

// template <typename CANVAS>
// class HasBackend {
// protected:
//     GraphicsBackend<CANVAS> *backend{};
// public:
//     HasBackend(GraphicsBackend<CANVAS> *backend) : backend(backend)
//     {
//     }
//     virtual ~HasBackend() {
//         delete backend;
//     }
//     mssm::CoreWindow *getWindow() { return backend->getWindow(); }
//     CANVAS *getCanvas() { return backend->getCanvas(); }
// };

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
class GraphicsBase : public SoundHost, public mssm::ImageLoader, public mssm::EvtSourceWrapper
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

    template<typename... _Args>
    inline void print(fmt::format_string<_Args...> __fmt, _Args &&...__args)
    {
        this->cout << fmt::vformat(__fmt.get(), fmt::make_format_args(__args...));
    }

    template<typename... _Args>
    inline void println(fmt::format_string<_Args...> __fmt, _Args &&...__args)
    {
        this->cout << fmt::vformat(__fmt.get(), fmt::make_format_args(__args...)) << std::endl;
    }

    template<typename... _Args>
    inline void printError(fmt::format_string<_Args...> __fmt, _Args &&...__args)
    {
        this->cerr << fmt::vformat(__fmt.get(), fmt::make_format_args(__args...)) << std::endl;
    }

  //  std::chrono::milliseconds::rep time();

    //    void waitUntilClosed();

 //   double elapsedMs() const;

    // int width() override { return canvas->width(); }
    // int height() override { return canvas->height(); }

    Vec2d mousePos() const { return eventSource->mousePos<Vec2d>(); }

    // void setBackground(Color c) override;

    // bool isClipped(Vec2d pos) const override;
    // void setClip(int x, int y, int w, int h) override;
    // void resetClip() override;
    // void pushClip(int x, int y, int w, int h, bool replace) override;
    // void popClip() override;
    // void setViewport(int x, int y, int w, int h) override;
    // void resetViewport() override;

    // 3d

    // void line3d(Vec3d p1, Vec3d p2, mssm::Color c) override { canvas->line3d(p1, p2, c); }
    // void polygon3d(const std::vector<Vec3d> &points, mssm::Color border, mssm::Color fill) override
    // {
    //     canvas->polygon3d(points, border, fill);
    // }

    // void setModelMatrix(mat4x4 &model) override { canvas->setModelMatrix(model); }
    // void resetModelMatrix() override { canvas->resetModelMatrix(); }
    // void setCameraParams(Vec3d eye, Vec3d target, Vec3d up, double near, double far) override
    // {
    //     canvas->setCameraParams(eye, target, up, near, far);
    // }
    // void setLightParams(Vec3d pos, Color color) override
    // {
    //     canvas->setLightParams(pos, color);
    // }

    // TriWriter<Vertex3dUV> getTriangleWriter(uint32_t triCount) override
    // {
    //     return canvas->getTriangleWriter(triCount);
    // }

    // 2d

    // void line(Vec2d p1, Vec2d p2, Color c = WHITE) override
    // {
    //     return canvas->line(p1, p2, c);
    // }

    // void ellipse(Vec2d center, double w, double h, Color c = WHITE, Color f = mssm::TRANS) override
    // {
    //     return canvas->ellipse(center, w, h, c, f);
    // }

    // void arc(Vec2d center, double w, double h, double a, double alen, Color c = WHITE) override
    // {
    //     return canvas->arc(center, w, h, a, alen, c);
    // }


    // void chord(Vec2d center,
    //            double w,
    //            double h,
    //            double a,
    //            double alen,
    //            Color c = WHITE,
    //            Color f = mssm::TRANS) override
    // {
    //     return canvas->chord(center, w, h, a, alen, c, f);
    // }
    // void pie(Vec2d center,
    //          double w,
    //          double h,
    //          double a,
    //          double alen,
    //          Color c = WHITE,
    //          Color f = mssm::TRANS) override
    // {
    //     return canvas->pie(center, w, h, a, alen, c, f);
    // }
    // void rect(Vec2d corner, double w, double h, Color c = WHITE, Color f = mssm::TRANS) override
    // {
    //     return canvas->rect(corner, w, h, c, f);
    // }

    // void polygon(const std::vector<Vec2d> &points, Color border, Color fill = mssm::TRANS) override
    // {
    //     return canvas->polygon(points, border, fill);
    // }
    // void polyline(const std::vector<Vec2d> &points, Color color) override
    // {
    //     return canvas->polyline(points, color);
    // }
    // void points(const std::vector<Vec2d> &points, Color c) override
    // {
    //     return canvas->points(points, c);
    // }

    // void polygon(const Array<Vec2d> &points, Color border, Color fill = mssm::TRANS);
    // void polyline(const Array<Vec2d> &points, Color color);
    // void points(const Array<Vec2d> &points, Color c);

    // void polygon(std::initializer_list<Vec2d> points,
    //              Color border,
    //              Color fill = mssm::TRANS) override
    // {
    //     return canvas->polygon(points, border, fill);
    // }
    // void polyline(std::initializer_list<Vec2d> points, Color color) override
    // {
    //     return canvas->polyline(points, color);
    // }
    // void points(std::initializer_list<Vec2d> points, Color c) override
    // {
    //     return canvas->points(points, c);
    // }

    // void text(Vec2d pos,
    //           const FontInfo &sizeAndFace,
    //           const std::string &str,
    //           Color textColor = WHITE,
    //           HAlign hAlign = HAlign::left,
    //           VAlign vAlign = VAlign::baseline) override
    // {
    //     return canvas->text(pos, sizeAndFace, str, textColor, hAlign, vAlign);
    // }

    // void textExtents(const FontInfo &sizeAndFace,
    //                  const std::string &str,
    //                  TextExtents &extents) override
    // {
    //     return canvas->textExtents(sizeAndFace, str, extents);
    // }
    // double textWidth(const FontInfo &sizeAndFace, const std::string &str) override
    // {
    //     return canvas->textWidth(sizeAndFace, str);
    // }
    // std::vector<double> getCharacterXOffsets(const FontInfo& sizeAndFace, double startX, const std::string& text) override
    // {
    //     return canvas->getCharacterXOffsets(sizeAndFace, startX, text);
    // }

    // void point(Vec2d pos, Color c) override { return canvas->point(pos, c); }

    // void image(Vec2d pos, const Image &img) override { return canvas->image(pos, img); }
    // void image(Vec2d pos, const Image &img, Vec2d src, int srcw, int srch) override
    // {
    //     return canvas->image(pos, img, src, srcw, srch);
    // }
    // void image(Vec2d pos, double w, double h, const Image &img) override
    // {
    //     return canvas->image(pos, w, h, img);
    // }
    // void image(Vec2d pos, double w, double h, const Image &img, Vec2d src, int srcw, int srch) override
    // {
    //     return canvas->image(pos, w, h, img, src, srcw, srch);
    // }

    // void imageC(Vec2d center, double angle, const Image &img) override
    // {
    //     return canvas->imageC(center, angle, img);
    // }
    // void imageC(Vec2d center, double angle, const Image &img, Vec2d src, int srcw, int srch) override
    // {
    //     return canvas->imageC(center, angle, img, src, srcw, srch);
    // }
    // void imageC(Vec2d center, double angle, double w, double h, const Image &img) override
    // {
    //     return canvas->imageC(center, angle, w, h, img);
    // }
    // void imageC(Vec2d center,
    //             double angle,
    //             double w,
    //             double h,
    //             const Image &img,
    //             Vec2d src,
    //             int srcw,
    //             int srch) override
    // {
    //     return canvas->imageC(center, angle, w, h, img, src, srcw, srch);
    // }



    bool draw();
    void drawFromStream(std::stringstream &ss, std::deque<std::string> &lines, Vec2d start, Color c);

    bool isClosed() const;

  //  double timeMs() const;

    // bool isKeyPressed(Key c) const { return eventSource->isKeyPressed(c); }
    // bool isKeyPressed(char c) const { return eventSource->isKeyPressed(Key{c}); }
    // bool isKeyPressed(Key::KeyConst c) const { return eventSource->isKeyPressed(c); }

    // bool onKeyPress(Key c) const { return eventSource->onKeyPress(c); }
    // bool onKeyPress(char c) const { return eventSource->onKeyPress(Key{c}); }
    // bool onKeyPress(Key::KeyConst c) const { return eventSource->onKeyPress(c); }

    // bool onKeyRelease(Key c) const { return eventSource->onKeyRelease(c); }
    // bool onKeyRelease(char c) const { return eventSource->onKeyRelease(Key{c}); }
    // bool onKeyRelease(Key::KeyConst c) const { return eventSource->onKeyRelease(c); }

    // bool isShiftKeyPressed() const { return eventSource->isShiftKeyPressed(); }
    // bool isCtrlKeyPressed() const { return eventSource->isCtrlKeyPressed(); }
    // bool isAltKeyPressed() const { return eventSource->isAltKeyPressed(); }

    // bool isMousePressed(MouseButton button) const
    // {
    //     return eventSource->isMousePressed(button);
    // }
    // bool onMousePress(MouseButton button) const { return eventSource->onMousePress(button); }
    // bool onMouseRelease(MouseButton button) const
    // {
    //     return eventSource->onMouseRelease(button);
    // }

    // Key getKeyPressed() const { return eventSource->getKeyPressed(); }
    // MouseButton getMousePressed() const { return eventSource->getMousePressed(); }
    // int getWheelDelta() const { return eventSource->getWheelDelta(); }
    // int getDigitPressed();

   // double maxDragDistance(MouseButton button) const { return eventSource->maxDragDistance(button); }
    Vec2d mouseDragStart(MouseButton button) const { return eventSource->mouseDragStart<Vec2d>(button); }

    const std::vector<Event>& events() { return eventSource->events(); }

    int randomInt(int minVal, int maxVal) { return ::randomInt(minVal, maxVal); }
    double randomDouble(double minVal, double maxVal) { return ::randomDouble(minVal, maxVal); }
    bool randomTrue(double pct) { return ::randomTrue(pct); }

//    bool isDrawable() override { return canvas->isDrawable(); }

  //  void setCursor(mssm::CoreWindowCursor cursor) { backend->setCursor(cursor); }



public:
    std::shared_ptr<ImageInternal> loadImg(std::string filename, bool cachePixels) override;

    // ImageLoader interface
public:
    std::shared_ptr<ImageInternal> createImg(int width,
                                             int height,
                                             Color c,
                                             bool cachePixels) override;
    std::shared_ptr<ImageInternal> initImg(int width,
                                           int height,
                                           Color *pixels,
                                           bool cachePixels) override;
    void saveImg(std::shared_ptr<ImageInternal> img, std::string filename) override;
};


template <typename WINDOW, typename CANVAS>
mssm::GraphicsBase<WINDOW, CANVAS>::GraphicsBase(std::string title,
                                                 int width, int height,
                                                 std::function<GraphicsBackendBase<WINDOW, CANVAS>*(std::string title, int width, int height)> loadGraphicsBackend)
    : mssm::EvtSourceWrapper{{}}
{
    backend = loadGraphicsBackend(title, width, height);
    canvas = backend->getCanvas();
    window = backend->getWindow();
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

template<typename WINDOW, typename CANVAS>
std::shared_ptr<ImageInternal> GraphicsBase<WINDOW, CANVAS>::loadImg(std::string filename, bool cachePixels)
{
    return backend->loadImg(Paths::findAsset(filename), cachePixels);
}

template<typename WINDOW, typename CANVAS>
std::shared_ptr<ImageInternal> GraphicsBase<WINDOW, CANVAS>::createImg(int width,
                                                                       int height,
                                                                       Color c,
                                                                       bool cachePixels)
{
    return backend->createImg(width, height, c, cachePixels);
}

template<typename WINDOW, typename CANVAS>
std::shared_ptr<ImageInternal> GraphicsBase<WINDOW, CANVAS>::initImg(int width,
                                                                     int height,
                                                                     Color *pixels,
                                                                     bool cachePixels)
{
    return backend->initImg(width, height, pixels, cachePixels);
}

template<typename WINDOW, typename CANVAS>
inline void GraphicsBase<WINDOW, CANVAS>::saveImg(std::shared_ptr<ImageInternal> img,
                                                  std::string filename)
{
    return backend->saveImg(img, filename);
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
