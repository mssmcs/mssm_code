#ifndef CANVASEXTENT_H
#define CANVASEXTENT_H

#include "canvas2d.h"

class ExtentWrapper : public mssm::Canvas2d
{
private:
    mssm::Canvas2d& g;
public:
    Vec2d minExtent;
    Vec2d maxExtent;
public:
    ExtentWrapper(mssm::Canvas2d& g) : g{g} {}
    void   resetExtents();
    void   extend(Vec2d p);

    // Canvas2d interface
public:
    bool isDrawable() override;
    int width() override;
    int height() override;
    void setBackground(mssm::Color c) override;
    void line(Vec2d p1, Vec2d p2, mssm::Color c) override;
    void ellipse(Vec2d center, double w, double h, mssm::Color c, mssm::Color f) override;
    void arc(Vec2d center, double w, double h, double a, double alen, mssm::Color c) override;
    void chord(Vec2d center, double w, double h, double a, double alen, mssm::Color c, mssm::Color f)
        override;
    void pie(Vec2d center, double w, double h, double a, double alen, mssm::Color c, mssm::Color f)
        override;
    void rect(Vec2d corner, double w, double h, mssm::Color c, mssm::Color f) override;
    void polygon(const std::vector<Vec2d> &points, mssm::Color border, mssm::Color fill) override;
    void polyline(const std::vector<Vec2d> &points, mssm::Color color) override;
    void points(const std::vector<Vec2d> &points, mssm::Color c) override;
    void polygon(std::initializer_list<Vec2d> points, mssm::Color border, mssm::Color fill) override;
    void polyline(std::initializer_list<Vec2d> points, mssm::Color color) override;
    void points(std::initializer_list<Vec2d> points, mssm::Color c) override;
    void text(Vec2d pos,
              const FontInfo &sizeAndFace,
              const std::string &str,
              mssm::Color textcolor,
              HAlign hAlign,
              VAlign vAlign) override;
    void textExtents(const FontInfo &sizeAndFace,
                     const std::string &str,
                     TextExtents &extents) override;
    double textWidth(const FontInfo &sizeAndFace, const std::string &str) override;
    std::vector<double> getCharacterXOffsets(const FontInfo &sizeAndFace,
                                             double startX,
                                             const std::string &text) override;
    void point(Vec2d pos, mssm::Color c) override;
    void image(Vec2d pos, const mssm::Image &img) override;
    void image(Vec2d pos, const mssm::Image &img, Vec2d src, int srcw, int srch) override;
    void image(Vec2d pos, double w, double h, const mssm::Image &img) override;
    void image(Vec2d pos, double w, double h, const mssm::Image &img, Vec2d src, int srcw, int srch)
        override;
    void imageC(Vec2d center, double angle, const mssm::Image &img) override;
    void imageC(
        Vec2d center, double angle, const mssm::Image &img, Vec2d src, int srcw, int srch) override;
    void imageC(Vec2d center, double angle, double w, double h, const mssm::Image &img) override;
    void imageC(Vec2d center,
                double angle,
                double w,
                double h,
                const mssm::Image &img,
                Vec2d src,
                int srcw,
                int srch) override;
    bool isClipped(Vec2d pos) const override;
    void pushClip(int x, int y, int w, int h, bool replace) override;
    void popClip() override;
    void setClip(int x, int y, int w, int h) override;
    void resetClip() override;
    void setViewport(int x, int y, int w, int h) override;
    void resetViewport() override;
    void pushGroup(std::string groupName) override;
    void popGroup() override;
    void polygonPattern(const std::vector<Vec2d> &points, mssm::Color c, mssm::Color f) override;
    void polygonPattern(std::initializer_list<Vec2d> points, mssm::Color c, mssm::Color f) override;
};

#endif // CANVASEXTENT_H
