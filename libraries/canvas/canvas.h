#ifndef CANVAS_H
#define CANVAS_H

#include "color.h"
#include "linmath.h"
#include "textinfo.h"
#include "triwriter.h"
#include "vec2d.h"
#include "vec3d.h"
#include "vertextypes.h"
#include <vector>

namespace mssm {

class Image;

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

class Canvas : public CanvasOverloads<Vec2d, Canvas> {
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

    // 3d
    virtual void line(Vec3d p1, Vec3d p2, mssm::Color c) = 0;
    virtual void polygon(const std::vector<Vec3d> &points,
                 mssm::Color border,
                 mssm::Color fill) = 0;
    virtual void setModelMatrix(mat4x4 &model) = 0;
    virtual void resetModelMatrix() = 0;
    virtual void setCameraParams(Vec3d eye, Vec3d target, Vec3d up, double near, double far) = 0;
    virtual void setLightParams(Vec3d pos, Color color) = 0;

    virtual TriWriter<Vertex3dUV> getTriangleWriter(uint32_t triCount) = 0;
};

}

#endif // CANVAS_H
