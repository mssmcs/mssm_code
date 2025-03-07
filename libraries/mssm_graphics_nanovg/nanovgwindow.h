#ifndef NANOVGWINDOW_H
#define NANOVGWINDOW_H

#include "canvas2d.h"
#include "color.h"
#include "glfwindow.h"
#include "image.h"

#include "nanovg.h"

class ImageInternalVG : public mssm::ImageInternal {
    NVGcontext* vg;
    int vgImageIdx;
public:
    ImageInternalVG(NVGcontext* vg, int idx, int width, int height, mssm::Color* cached);
    ~ImageInternalVG();
    uint32_t textureIndex() const override { return vgImageIdx; };
private:
    void freeCachedPixels();
    void updatePixels() override;
    // void setPixel(int x, int y, mssm::Color c = mssm::WHITE) {
    //     pixels[y*w+x] = c;
    // }
    // mssm::Color getPixel(int x, int y) {
    //     return pixels[y*w+x];
    // }
    friend class Image;
    //friend class mssm::GraphicsBase;
};


class NanovgWindow : public mssm::CoreWindowGLFW, public mssm::ImageLoader, public mssm::Canvas2d
{
protected:
    struct Scissor {
        int x;
        int y;
        int w;
        int h;
    };
protected:
    NVGcontext* vg{nullptr};
    int fontRegular;
    int fontBold;
    int fontLight;
    std::vector<std::shared_ptr<mssm::ImageInternal>> keepImages;
    std::vector<Scissor> clipRects;

public:
    NanovgWindow(std::string title, int width, int height);
    virtual ~NanovgWindow();
    // CoreWindow interface
protected:
    void configure() override;
    void beginDrawing(bool wasResized) override;
    void endDrawing() override;

    // Canvas2d interface
public:
    bool isDrawable() override;
    int width() override { return mssm::CoreWindowGLFW::width(); }
    int height() override { return mssm::CoreWindowGLFW::height(); }
    void setBackground(mssm::Color c = mssm::WHITE) override;
    void line(Vec2d p1, Vec2d p2, mssm::Color c = mssm::WHITE) override;
    void ellipse(Vec2d center, double w, double h, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANS) override;
    void arc(Vec2d center, double w, double h, double a, double alen, mssm::Color c = mssm::WHITE) override;
    void chord(Vec2d center, double w, double h, double a, double alen, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANS)
        override;
    void pie(Vec2d center, double w, double h, double a, double alen, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANS)
        override;
    void rect(Vec2d corner, double w, double h, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANS) override;
    void polygon(const std::vector<Vec2d> &points, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANS) override;
    void polyline(const std::vector<Vec2d> &points, mssm::Color color) override;
    void points(const std::vector<Vec2d> &points, mssm::Color c = mssm::WHITE) override;
    void polygon(std::initializer_list<Vec2d> points, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANS) override;
    void polyline(std::initializer_list<Vec2d> points, mssm::Color c = mssm::WHITE) override;
    void points(std::initializer_list<Vec2d> points, mssm::Color c = mssm::WHITE) override;
    void text(Vec2d pos,
              const FontInfo &sizeAndFace,
              const std::string &str,
              mssm::Color textcolor = mssm::WHITE, HAlign hAlign = HAlign::left, VAlign vAlign = VAlign::baseline) override;
    void textExtents(const FontInfo &sizeAndFace,
                     const std::string &str,
                     TextExtents &extents) override;
    double textWidth(const FontInfo &sizeAndFace, const std::string &str) override;
    std::vector<double> getCharacterXOffsets(const FontInfo &sizeAndFace,
                                             double startX,
                                             const std::string &text) override;
    void point(Vec2d pos, mssm::Color c = mssm::WHITE) override;
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

    template<typename T>
    void t_polygon(T points, mssm::Color border, mssm::Color fill);

    template<typename T>
    void t_polyline(T points, mssm::Color color, bool closed);

    template<typename T>
    void t_points(T points, mssm::Color c);

    // ImageLoader interface
public:
    std::shared_ptr<mssm::ImageInternal> loadImg(std::string filename, bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> createImg(int width,
                                                   int height,
                                                   mssm::Color c,
                                                   bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> initImg(int width,
                                                 int height,
                                                 mssm::Color *pixels,
                                                 bool cachePixels) override;
    void saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename) override;

    void pushGroup(std::string layerName) override {}
    void popGroup() override {}
    void polygonPattern(const std::vector<Vec2d> &points, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANSPARENT) override;
    void polygonPattern(std::initializer_list<Vec2d> points, mssm::Color c = mssm::WHITE, mssm::Color f = mssm::TRANSPARENT) override;
};

#endif // NANOVGWINDOW_H
