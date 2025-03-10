#ifndef CANVAS2D_H
#define CANVAS2D_H

#ifdef SUPPORT_MSSM_ARRAY
#include "array.h"
#endif
#include "color.h"
#include "textinfo.h"
#include "vec2d.h"
#include "vecUtil.h"
#include <vector>

namespace mssm {

class Image;
class Canvas2d;

class GraphicsGroup {
private:
    Canvas2d& g;
public:
    GraphicsGroup(Canvas2d& g, std::string layerName);
    ~GraphicsGroup();
};

template <is2dVector VDest, typename SELF>
class CanvasOverloads {
public:
    template <is2dVector V>
    void lineV(V p1, V p2, Color c = WHITE);
    template <is2dVector V>
    void ellipseV(V center, double w, double h, Color c = WHITE, Color f = mssm::TRANS);
    template <is2dVector V>
    void arcV(V center, double w, double h, double a, double alen, Color c = WHITE);
    template <is2dVector V>
    void chordV(V center, double w, double h, double a, double alen, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT);
    template <is2dVector V>
    void pieV(V center, double w, double h, double a, double alen, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT);
    template <is2dVector V>
    void rectV(V corner, double w, double h, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT);
};

template <is2dVector VDest, typename SELF>
template <is2dVector V>
void CanvasOverloads<VDest, SELF>::arcV(V center, double w, double h, double a, double alen, Color c)
{
    static_cast<SELF*>(this)->arc(cast<VDest>(center), w, h, a, alen, c);
}

template <is2dVector VDest, typename SELF>
template <is2dVector V>
void CanvasOverloads<VDest, SELF>::ellipseV(V center, double w, double h, Color c, Color f)
{
    static_cast<SELF*>(this)->ellipse(cast<VDest>(center), w, h, c, f);
}

template <is2dVector VDest, typename SELF>
template <is2dVector V>
void CanvasOverloads<VDest, SELF>::lineV(V p1, V p2, Color c)
{
    static_cast<SELF*>(this)->line(cast<VDest>(p1), cast<VDest>(p2), c);
}

template <is2dVector VDest, typename SELF>
template <is2dVector V>
void CanvasOverloads<VDest, SELF>::chordV(V center, double w, double h, double a, double alen, Color c, Color f)
{
    static_cast<SELF*>(this)->chord(cast<VDest>(center), w, h, a, alen, c, f);
}

template <is2dVector VDest, typename SELF>
template <is2dVector V>
void CanvasOverloads<VDest, SELF>::pieV(V center, double w, double h, double a, double alen, Color c, Color f)
{
    static_cast<SELF*>(this)->pie(cast<VDest>(center), w, h, a, alen, c, f);
}

template <is2dVector VDest, typename SELF>
template <is2dVector V>
void CanvasOverloads<VDest, SELF>::rectV(V corner, double w, double h, Color c, Color f)
{
    static_cast<SELF*>(this)->rect(cast<VDest>(corner), w, h, c, f);
}

class Canvas2d : public CanvasOverloads<Vec2d, Canvas2d> {
public:
    virtual bool   isDrawable() = 0;
    virtual int    width() = 0;
    virtual int    height() = 0;
    virtual void   setBackground(mssm::Color c) = 0;
    virtual void   line(Vec2d p1, Vec2d p2, mssm::Color c = WHITE) = 0;
    virtual void   ellipse(Vec2d center, double w, double h, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) = 0;
    virtual void   arc(Vec2d center, double w, double h, double a, double alen, mssm::Color c = WHITE) = 0;
    virtual void   chord(Vec2d center, double w, double h, double a, double alen, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) = 0;
    virtual void   pie(Vec2d center, double w, double h, double a, double alen, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) = 0;
    virtual void   rect(Vec2d corner, double w, double h, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) = 0;
    virtual void   polygon(const std::vector<Vec2d>& points, mssm::Color border, mssm::Color fill = mssm::TRANSPARENT) = 0;
    virtual void   polyline(const std::vector<Vec2d>& points, mssm::Color color) = 0;
    virtual void   points(const std::vector<Vec2d>& points, mssm::Color c) = 0;
#ifdef SUPPORT_MSSM_ARRAY
    virtual void   polygon(const Array<Vec2d>& points, mssm::Color border, mssm::Color fill = mssm::TRANSPARENT) = 0;
    virtual void   polyline(const Array<Vec2d>& points, mssm::Color color) = 0;
    virtual void   points(const Array<Vec2d>& points, mssm::Color c) = 0;
#endif // SUPPORT_MSSM_ARRAY
    virtual void   polygon(std::initializer_list<Vec2d> points, mssm::Color border, mssm::Color fill = mssm::TRANSPARENT) = 0;
    virtual void   polyline(std::initializer_list<Vec2d> points, mssm::Color color) = 0;
    virtual void   points(std::initializer_list<Vec2d> points, mssm::Color c) = 0;
    virtual void   text(Vec2d pos, const FontInfo& sizeAndFace, const std::string& str, mssm::Color textcolor = WHITE, HAlign hAlign = HAlign::left, VAlign vAlign = VAlign::baseline) = 0;
    virtual void   textExtents(const FontInfo& sizeAndFace, const std::string& str, TextExtents& extents) = 0;
    virtual double textWidth(const FontInfo& sizeAndFace, const std::string& str) = 0;
    virtual std::vector<double> getCharacterXOffsets(const FontInfo& sizeAndFace, double startX, const std::string& text) = 0;
    virtual void   point(Vec2d pos, mssm::Color c) = 0;
    virtual void   image(Vec2d pos, const Image& img) = 0;
    virtual void   image(Vec2d pos, const Image& img, Vec2d src, int srcw, int srch) = 0;
    virtual void   image(Vec2d pos, double w, double h, const Image& img) = 0;
    virtual void   image(Vec2d pos, double w, double h, const Image& img, Vec2d src, int srcw, int srch) = 0;
    virtual void   imageC(Vec2d center, double angle, const Image& img) = 0;
    virtual void   imageC(Vec2d center, double angle, const Image& img, Vec2d src, int srcw, int srch) = 0;
    virtual void   imageC(Vec2d center, double angle, double w, double h, const Image& img) = 0;
    virtual void   imageC(Vec2d center, double angle, double w, double h, const Image& img, Vec2d src, int srcw, int srch) = 0;

    virtual bool isClipped(Vec2d pos) const = 0;
    virtual void pushClip(int x, int y, int w, int h, bool replace) = 0;
    virtual void popClip() = 0;
    virtual void setClip(int x, int y, int w, int h) = 0;
    virtual void resetClip() = 0;
    virtual void setViewport(int x, int y, int w, int h) = 0;
    virtual void resetViewport() = 0;

    virtual void pushGroup(std::string groupName) = 0;
    virtual void popGroup() = 0;
    virtual void polygonPattern(const std::vector<Vec2d> &points, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) = 0;
    virtual void polygonPattern(std::initializer_list<Vec2d> points, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) = 0;

};

class Canvas2dWrapper : public Canvas2d {
protected:
    Canvas2d* canvas{};
public:
    Canvas2dWrapper(Canvas2d* canvas) : canvas(canvas) {}
    bool isDrawable() override { return canvas->isDrawable(); }
    int width() override { return canvas->width(); }
    int height() override { return canvas->height(); }
    void setBackground(Color c) override { canvas->setBackground(c); }
    void line(Vec2d p1, Vec2d p2, mssm::Color c = WHITE) override { canvas->line(p1, p2, c); }
    void ellipse(Vec2d center, double w, double h, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) override { canvas->ellipse(center, w, h, c, f); }
    void arc(Vec2d center, double w, double h, double a, double alen, mssm::Color c = WHITE) override { canvas->arc(center, w, h, a, alen, c); }
    void chord(Vec2d center, double w, double h, double a, double alen, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) override
    { canvas->chord(center, w, h, a, alen, c, f); }
    void pie(Vec2d center, double w, double h, double a, double alen, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) override { canvas->pie(center, w, h, a, alen, c, f); }
    void rect(Vec2d corner, double w, double h, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) override { canvas->rect(corner, w, h, c, f); }
    void polygon(const std::vector<Vec2d> &points, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) override { canvas->polygon(points, c, f); }
    void polyline(const std::vector<Vec2d> &points, mssm::Color c = WHITE) override { canvas->polyline(points, c); }
    void points(const std::vector<Vec2d> &points, mssm::Color c = WHITE) override { canvas->points(points, c); }
    void polygon(std::initializer_list<Vec2d> points, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) override { canvas->polygon(points, c, f); }
    void polyline(std::initializer_list<Vec2d> points, mssm::Color c = WHITE) override { canvas->polyline(points, c); }
    void points(std::initializer_list<Vec2d> points, mssm::Color c = WHITE) override { canvas->points(points, c); }
    void text(Vec2d pos,
              const FontInfo &sizeAndFace,
              const std::string &str,
              mssm::Color textcolor = WHITE, HAlign hAlign = HAlign::left, VAlign vAlign = VAlign::baseline) override
    { canvas->text(pos, sizeAndFace, str, textcolor, hAlign, vAlign); }
    void textExtents(const FontInfo &sizeAndFace,
                     const std::string &str,
                     TextExtents &extents) override
    { canvas->textExtents(sizeAndFace, str, extents); }
    double textWidth(const FontInfo &sizeAndFace, const std::string &str) override { return canvas->textWidth(sizeAndFace, str); }
    std::vector<double> getCharacterXOffsets(const FontInfo &sizeAndFace,
                                             double startX,
                                             const std::string &text) override
    { return canvas->getCharacterXOffsets(sizeAndFace, startX, text); }
    void point(Vec2d pos, Color c) override { canvas->point(pos, c); }
    void image(Vec2d pos, const Image &img) override { canvas->image(pos, img); }
    void image(Vec2d pos, const Image &img, Vec2d src, int srcw, int srch) override { canvas->image(pos, img, src, srcw, srch); }
    void image(Vec2d pos, double w, double h, const Image &img) override { canvas->image(pos, w, h, img); }
    void image(Vec2d pos, double w, double h, const Image &img, Vec2d src, int srcw, int srch) override
    { canvas->image(pos, w, h, img, src, srcw, srch); }
    void imageC(Vec2d center, double angle, const Image &img) override { canvas->imageC(center, angle, img); }
    void imageC(Vec2d center, double angle, const Image &img, Vec2d src, int srcw, int srch) override
    { canvas->imageC(center, angle, img, src, srcw, srch); }
    void imageC(Vec2d center, double angle, double w, double h, const Image &img) override { canvas->imageC(center, angle, w, h, img); }
    void imageC(Vec2d center,
                double angle,
                double w,
                double h,
                const Image &img,
                Vec2d src,
                int srcw,
                int srch) override
    { canvas->imageC(center, angle, w, h, img, src, srcw, srch); }
    bool isClipped(Vec2d pos) const override { return canvas->isClipped(pos); }
    void pushClip(int x, int y, int w, int h, bool replace) override { canvas->pushClip(x, y, w, h, replace); }
    void popClip() override { canvas->popClip(); }
    void setClip(int x, int y, int w, int h) override { canvas->setClip(x, y, w, h); }
    void resetClip() override { canvas->resetClip(); }
    void setViewport(int x, int y, int w, int h) override { canvas->setViewport(x, y, w, h); }
    void resetViewport() override { canvas->resetViewport(); }
#ifdef SUPPORT_MSSM_ARRAY
    void   polygon(const Array<Vec2d>& points, mssm::Color border, mssm::Color fill = mssm::TRANSPARENT) { canvas->polygon(points, border, fill); }
    void   polyline(const Array<Vec2d>& points, mssm::Color color) { canvas->polyline(points, color); }
    void   points(const Array<Vec2d>& points, mssm::Color c) { canvas->points(points, c); }
#endif // SUPPORT_MSSM_ARRAY
    void pushGroup(std::string groupName) override { canvas->pushGroup(groupName); }
    void popGroup() override { canvas->popGroup(); }
    void polygonPattern(const std::vector<Vec2d> &points, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) override { canvas->polygon(points, c, f); }
    void polygonPattern(std::initializer_list<Vec2d> points, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) override { canvas->polygon(points, c, f); }
};

inline GraphicsGroup::GraphicsGroup(Canvas2d &g, std::string layerName) : g{g}
{
    g.pushGroup(layerName);
}

inline GraphicsGroup::~GraphicsGroup()
{
    g.popGroup();
}

}

#endif // CANVAS2D_H
