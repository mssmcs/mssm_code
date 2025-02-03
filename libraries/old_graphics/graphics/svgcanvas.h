#ifndef SVGCANVAS_H
#define SVGCANVAS_H

#include "graphics.h"
#include <fstream>

namespace mssm {

class SVGCanvas : public mssm::Canvas
{
    int w;
    int h;
     ofstream svgFile;
public:
    SVGCanvas(string filename, int width, int height);
    ~SVGCanvas();
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
    virtual void polygon(const Array<Vec2d> &points, Color border, Color fill) override;
    virtual void polyline(const Array<Vec2d> &points, Color color) override;
    virtual void points(const Array<Vec2d> &points, Color c) override;
    virtual void polygon(std::initializer_list<Vec2d> points, Color border, Color fill) override;
    virtual void polyline(std::initializer_list<Vec2d> points, Color color) override;
    virtual void points(std::initializer_list<Vec2d> points, Color c) override;
    virtual void text(Vec2d pos, const FontInfo &sizeAndFace, const string &str, Color textColor, HAlign hAlign, VAlign vAlign) override;
    virtual void textExtents(double size, const string &str, TextExtents &extents) override;
    virtual double textWidth(double size, const string &str) override;
    virtual void point(Vec2d pos, Color c) override;
    virtual void image(Vec2d pos, const Image &img) override;
    virtual void image(Vec2d pos, const Image &img, Vec2d src, int srcw, int srch) override;
    virtual void image(Vec2d pos, double w, double h, const Image &img) override;
    virtual void image(Vec2d pos, double w, double h, const Image &img, Vec2d src, int srcw, int srch) override;
    virtual void imageC(Vec2d center, double angle, const Image &img) override;
    virtual void imageC(Vec2d center, double angle, const Image &img, Vec2d src, int srcw, int srch) override;
    virtual void imageC(Vec2d center, double angle, double w, double h, const Image &img) override;
    virtual void imageC(Vec2d center, double angle, double w, double h, const Image &img, Vec2d src, int srcw, int srch) override;

    string toString(mssm::Color color) const;

};
}

#endif // SVGCANVAS_H
