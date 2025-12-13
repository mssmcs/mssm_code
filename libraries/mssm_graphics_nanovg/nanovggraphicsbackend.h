#ifndef NANOVGGRAPHICSBACKEND_H
#define NANOVGGRAPHICSBACKEND_H

#include "canvas2d.h"
#include "graphicsbackend.h"
#include "nanovgwindow.h"

class NanovgGraphicsBackend : public GraphicsBackend<NanovgWindow, mssm::Canvas2d> {
public:
    NanovgGraphicsBackend(std::string title, int width, int height);
    virtual ~NanovgGraphicsBackend();

    // GraphicsBackend interface
public:
    void setCursor(mssm::CoreWindowCursor cursor) override;
    std::shared_ptr<mssm::ImageInternal> loadImg(std::string filename, bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> createImg(int width, int height, mssm::Color c, bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> initImg(int width, int height, mssm::Color* pixels, bool cachePixels) override;
    void saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename) override;
    void queueForDestruction(std::shared_ptr<mssm::ImageInternal> img) override;
};

#endif // NANOVGGRAPHICSBACKEND_H
