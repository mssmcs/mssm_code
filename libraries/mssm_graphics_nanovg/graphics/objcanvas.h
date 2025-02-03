#ifndef OBJCANVAS_H
#define OBJCANVAS_H

#include "canvas2d.h"

namespace mssm {

    class ObjCanvas : public mssm::Canvas2d
    {
        int w;
        int h;
    public:
        ObjCanvas(std::string filename, int width, int height);

        // Canvas interface
    public:
        virtual int width() override;
        virtual int height() override;
        virtual void setBackground(Color c) override;
        virtual void line(Vec2d p1, Vec2d p2, Color c) override;
        virtual void ellipse(Vec2d center, double w, double h, Color c, Color f) override;
        virtual void arc(Vec2d center, double w, double h, double a, double alen, Color c) override;
        virtual void chord(Vec2d center, double w, double h, double a, double alen, Color c, Color f) override;
        virtual void pie(Vec2d center, double w, double h, double a, double alen, Color c, Color f) override;
        virtual void rect(Vec2d corner, double w, double h, Color c, Color f) override;
        virtual void polygon(const std::vector<Vec2d> &points, Color border, Color fill) override;
        virtual void polyline(const std::vector<Vec2d> &points, Color color) override;
        virtual void points(const std::vector<Vec2d> &points, Color c) override;
#ifdef SUPPORT_MSSM_ARRAY
        virtual void polygon(const Array<Vec2d> &points, Color border, Color fill) override;
        virtual void polyline(const Array<Vec2d> &points, Color color) override;
        virtual void points(const Array<Vec2d> &points, Color c) override;
#endif
        virtual void polygon(std::initializer_list<Vec2d> points, Color border, Color fill) override;
        virtual void polyline(std::initializer_list<Vec2d> points, Color color) override;
        virtual void points(std::initializer_list<Vec2d> points, Color c) override;
        virtual void text(Vec2d pos, const FontInfo &sizeAndFace, const std::string &str, Color textColor, HAlign hAlign, VAlign vAlign) override;
        virtual void textExtents(const FontInfo &sizeAndFace, const std::string &str, TextExtents &extents) override;
        virtual double textWidth(const FontInfo &sizeAndFace, const std::string &str) override;
        virtual void point(Vec2d pos, Color c) override;
        virtual void image(Vec2d pos, const Image &img) override;
        virtual void image(Vec2d pos, const Image &img, Vec2d src, int srcw, int srch) override;
        virtual void image(Vec2d pos, double w, double h, const Image &img) override;
        virtual void image(Vec2d pos, double w, double h, const Image &img, Vec2d src, int srcw, int srch) override;
        virtual void imageC(Vec2d center, double angle, const Image &img) override;
        virtual void imageC(Vec2d center, double angle, const Image &img, Vec2d src, int srcw, int srch) override;
        virtual void imageC(Vec2d center, double angle, double w, double h, const Image &img) override;
        virtual void imageC(Vec2d center, double angle, double w, double h, const Image &img, Vec2d src, int srcw, int srch) override;
    };
}

#endif // OBJCANVAS_H
