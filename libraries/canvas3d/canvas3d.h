#ifndef CANVAS3D_H
#define CANVAS3D_H

#include "canvas2d.h"
#include "color.h"
#include "linmath.h"
#include "triwriter.h"
#include "vec3d.h"
#include "vertextypes3d.h"
#include <vector>

namespace mssm {

class Canvas3d : public Canvas2d {
public:

    // 3d
    virtual void line3d(Vec3d p1, Vec3d p2, mssm::Color c) = 0;
    virtual void polygon3d(const std::vector<Vec3d> &points,
                 mssm::Color border,
                 mssm::Color fill) = 0;
    virtual void setModelMatrix(mat4x4 &model) = 0;
    virtual void resetModelMatrix() = 0;
    virtual void setCameraParams(Vec3d eye, Vec3d target, Vec3d up, double near, double far) = 0;
    virtual void setLightParams(Vec3d pos, Color color) = 0;

    virtual TriWriter<Vertex3dUV> getTriangleWriter(uint32_t triCount) = 0;
};

class Canvas3dWrapper : public Canvas3d {
protected:
    Canvas3d* canvas{};
public:
    Canvas3dWrapper(Canvas3d* canvas) : canvas(canvas) {}
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
    void polygon(std::initializer_list<Vec2d> points, mssm::Color c = WHITE, mssm::Color f = mssm::TRANSPARENT) override { canvas->polygon(points, c, f);}
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
    void point(Vec2d pos, mssm::Color c = WHITE) override { canvas->point(pos, c); }
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

    // Canvas3d interface
public:
    void line3d(Vec3d p1, Vec3d p2, Color c) override { canvas->line3d(p1, p2, c); }
    void polygon3d(const std::vector<Vec3d> &points, mssm::Color c = WHITE, mssm::Color f = TRANS) override { canvas->polygon3d(points, c, f); }
    void setModelMatrix(mat4x4 &model) override { canvas->setModelMatrix(model); }
    void resetModelMatrix() override { canvas->resetModelMatrix(); }
    void setCameraParams(Vec3d eye, Vec3d target, Vec3d up, double near, double far) override { canvas->setCameraParams(eye, target, up, near, far); }
    void setLightParams(Vec3d pos, Color color) override { canvas->setLightParams(pos, color); }
    TriWriter<Vertex3dUV> getTriangleWriter(uint32_t triCount) override { return canvas->getTriangleWriter(triCount); }
};

}

#endif // CANVAS3D_H
