#ifndef GROBS_H
#define GROBS_H

#include <deque>
#include <sstream>
#include <string>

#include "array.h"
#include "canvas.h"
#include "corewindow.h"
#include "image.h"
#include "rand.h"
#include "soundengine.h"
#include "vec3d.h"

class GraphicsBackend
{
protected:
    gjh::CoreWindow *window;
    mssm::Canvas *canvas;
public:
    gjh::EventSource* eventSource() { return window; }
    mssm::Canvas* getCanvas() { return canvas; }
    virtual ~GraphicsBackend() {};
    virtual void postConfigure() = 0;
    virtual void setCursor(gjh::CoreWindowCursor cursor) = 0;
};

namespace mssm {
class Graphics;

class Sound
{
private:
    SoundHandle snd;

public:
    Sound(mssm::Graphics &g, const std::string &filename);
    friend class Graphics;
};

using Key = gjh::Key;
using MouseButton = gjh::MouseButton;
using Event = gjh::Event;
using EvtType = gjh::EvtType;
using ModKey = gjh::ModKey;

class Graphics : public mssm::Canvas, public mssm::ImageLoader
{
protected:
    GraphicsBackend *backend{};

    SoundEngine soundEngine;
    SoundHandle musicHandle{0};

public:
    Graphics(std::string title, int width, int height);
    virtual ~Graphics();

    gjh::EventSource &evtSource() { return *backend->eventSource(); }

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

    std::chrono::milliseconds::rep time();

    //    void waitUntilClosed();

    double elapsedMs() const;

    int width() override { return backend->getCanvas()->width(); }
    int height() override { return backend->getCanvas()->height(); }

    Vec2d mousePos() const;

    void setBackground(Color c) override;

    bool isClipped(Vec2d pos) const override;
    void setClip(int x, int y, int w, int h) override;
    void resetClip() override;
    void pushClip(int x, int y, int w, int h, bool replace) override;
    void popClip() override;
    void setViewport(int x, int y, int w, int h) override;
    void resetViewport() override;

    // 3d

    void line(Vec3d p1, Vec3d p2, mssm::Color c) override { backend->getCanvas()->line(p1, p2, c); }
    void polygon(const std::vector<Vec3d> &points, mssm::Color border, mssm::Color fill) override
    {
        backend->getCanvas()->polygon(points, border, fill);
    }

    void setModelMatrix(mat4x4 &model) override { backend->getCanvas()->setModelMatrix(model); }
    void resetModelMatrix() override { backend->getCanvas()->resetModelMatrix(); }
    void setCameraParams(Vec3d eye, Vec3d target, Vec3d up, double near, double far) override
    {
        backend->getCanvas()->setCameraParams(eye, target, up, near, far);
    }
    void setLightParams(Vec3d pos, Color color) override
    {
        backend->getCanvas()->setLightParams(pos, color);
    }

    TriWriter<Vertex3dUV> getTriangleWriter(uint32_t triCount) override
    {
        return backend->getCanvas()->getTriangleWriter(triCount);
    }

    // 2d

    void line(Vec2d p1, Vec2d p2, Color c = WHITE) override
    {
        return backend->getCanvas()->line(p1, p2, c);
    }

    void ellipse(Vec2d center, double w, double h, Color c = WHITE, Color f = mssm::TRANS) override
    {
        return backend->getCanvas()->ellipse(center, w, h, c, f);
    }

    void arc(Vec2d center, double w, double h, double a, double alen, Color c = WHITE) override
    {
        return backend->getCanvas()->arc(center, w, h, a, alen, c);
    }


    void chord(Vec2d center,
               double w,
               double h,
               double a,
               double alen,
               Color c = WHITE,
               Color f = mssm::TRANS) override
    {
        return backend->getCanvas()->chord(center, w, h, a, alen, c, f);
    }
    void pie(Vec2d center,
             double w,
             double h,
             double a,
             double alen,
             Color c = WHITE,
             Color f = mssm::TRANS) override
    {
        return backend->getCanvas()->pie(center, w, h, a, alen, c, f);
    }
    void rect(Vec2d corner, double w, double h, Color c = WHITE, Color f = mssm::TRANS) override
    {
        return backend->getCanvas()->rect(corner, w, h, c, f);
    }

    void polygon(const std::vector<Vec2d> &points, Color border, Color fill = mssm::TRANS) override
    {
        return backend->getCanvas()->polygon(points, border, fill);
    }
    void polyline(const std::vector<Vec2d> &points, Color color) override
    {
        return backend->getCanvas()->polyline(points, color);
    }
    void points(const std::vector<Vec2d> &points, Color c) override
    {
        return backend->getCanvas()->points(points, c);
    }

    void polygon(const Array<Vec2d> &points, Color border, Color fill = mssm::TRANS);
    void polyline(const Array<Vec2d> &points, Color color);
    void points(const Array<Vec2d> &points, Color c);

    void polygon(std::initializer_list<Vec2d> points,
                 Color border,
                 Color fill = mssm::TRANS) override
    {
        return backend->getCanvas()->polygon(points, border, fill);
    }
    void polyline(std::initializer_list<Vec2d> points, Color color) override
    {
        return backend->getCanvas()->polyline(points, color);
    }
    void points(std::initializer_list<Vec2d> points, Color c) override
    {
        return backend->getCanvas()->points(points, c);
    }

    void text(Vec2d pos,
              const FontInfo &sizeAndFace,
              const std::string &str,
              Color textColor = WHITE,
              HAlign hAlign = HAlign::left,
              VAlign vAlign = VAlign::baseline) override
    {
        return backend->getCanvas()->text(pos, sizeAndFace, str, textColor, hAlign, vAlign);
    }

    void textExtents(const FontInfo &sizeAndFace,
                     const std::string &str,
                     TextExtents &extents) override
    {
        return backend->getCanvas()->textExtents(sizeAndFace, str, extents);
    }
    double textWidth(const FontInfo &sizeAndFace, const std::string &str) override
    {
        return backend->getCanvas()->textWidth(sizeAndFace, str);
    }
    std::vector<double> getCharacterXOffsets(const FontInfo& sizeAndFace, double startX, const std::string& text) override
    {
        return backend->getCanvas()->getCharacterXOffsets(sizeAndFace, startX, text);
    }

    void point(Vec2d pos, Color c) override { return backend->getCanvas()->point(pos, c); }

    void image(Vec2d pos, const Image &img) override { return backend->getCanvas()->image(pos, img); }
    void image(Vec2d pos, const Image &img, Vec2d src, int srcw, int srch) override
    {
        return backend->getCanvas()->image(pos, img, src, srcw, srch);
    }
    void image(Vec2d pos, double w, double h, const Image &img) override
    {
        return backend->getCanvas()->image(pos, w, h, img);
    }
    void image(Vec2d pos, double w, double h, const Image &img, Vec2d src, int srcw, int srch) override
    {
        return backend->getCanvas()->image(pos, w, h, img, src, srcw, srch);
    }

    void imageC(Vec2d center, double angle, const Image &img) override
    {
        return backend->getCanvas()->imageC(center, angle, img);
    }
    void imageC(Vec2d center, double angle, const Image &img, Vec2d src, int srcw, int srch) override
    {
        return backend->getCanvas()->imageC(center, angle, img, src, srcw, srch);
    }
    void imageC(Vec2d center, double angle, double w, double h, const Image &img) override
    {
        return backend->getCanvas()->imageC(center, angle, w, h, img);
    }
    void imageC(Vec2d center,
                double angle,
                double w,
                double h,
                const Image &img,
                Vec2d src,
                int srcw,
                int srch) override
    {
        return backend->getCanvas()->imageC(center, angle, w, h, img, src, srcw, srch);
    }

    void play(Sound sound, double volume = -1); // volume 0 to 1
    void music(const std::string &filename, double volume = -1);
    void stopMusic();
    void setMusicVolume(double volume);

    bool draw();
    void drawFromStream(std::stringstream &ss, std::deque<std::string> &lines, Vec2d start, Color c);

    bool isClosed() const;

    double timeMs() const;

    bool isKeyPressed(Key c) const { return backend->eventSource()->isKeyPressed(c); }
    bool isKeyPressed(char c) const { return backend->eventSource()->isKeyPressed(Key{c}); }
    bool isKeyPressed(Key::KeyConst c) const { return backend->eventSource()->isKeyPressed(c); }

    bool onKeyPress(Key c) const { return backend->eventSource()->onKeyPress(c); }
    bool onKeyPress(char c) const { return backend->eventSource()->onKeyPress(Key{c}); }
    bool onKeyPress(Key::KeyConst c) const { return backend->eventSource()->onKeyPress(c); }

    bool onKeyRelease(Key c) const { return backend->eventSource()->onKeyRelease(c); }
    bool onKeyRelease(char c) const { return backend->eventSource()->onKeyRelease(Key{c}); }
    bool onKeyRelease(Key::KeyConst c) const { return backend->eventSource()->onKeyRelease(c); }

    bool isShiftKeyPressed() const { return backend->eventSource()->isShiftKeyPressed(); }
    bool isCtrlKeyPressed() const { return backend->eventSource()->isCtrlKeyPressed(); }
    bool isAltKeyPressed() const { return backend->eventSource()->isAltKeyPressed(); }

    bool isMousePressed(MouseButton button) const
    {
        return backend->eventSource()->isMousePressed(button);
    }
    bool onMousePress(MouseButton button) const { return backend->eventSource()->onMousePress(button); }
    bool onMouseRelease(MouseButton button) const
    {
        return backend->eventSource()->onMouseRelease(button);
    }

    Key getKeyPressed() const { return backend->eventSource()->getKeyPressed(); }
    MouseButton getMousePressed() const { return backend->eventSource()->getMousePressed(); }
    int getWheelDelta() const { return backend->eventSource()->getWheelDelta(); }
    int getDigitPressed();

    double maxDragDistance(MouseButton button) const { return backend->eventSource()->maxDragDistance(button); }
    Vec2d mouseDragStart(MouseButton button) const { return backend->eventSource()->mouseDragStart<Vec2d>(button); }

    const std::vector<Event>& events() { return backend->eventSource()->events(); }

    int randomInt(int minVal, int maxVal) { return ::randomInt(minVal, maxVal); }
    double randomDouble(double minVal, double maxVal) { return ::randomDouble(minVal, maxVal); }
    bool randomTrue(double pct) { return ::randomTrue(pct); }
    bool isDrawable() override { return backend->getCanvas()->isDrawable(); }

    void setCursor(gjh::CoreWindowCursor cursor) { backend->setCursor(cursor); }

protected:
    SoundEngine &getSoundPlayer() { return soundEngine; }
    friend class Sound;

    // ImageLoader interface
public:
    std::shared_ptr<ImageInternal> loadImg(std::string filename) override;
};

} // namespace mssm

#endif // GROBS_H
