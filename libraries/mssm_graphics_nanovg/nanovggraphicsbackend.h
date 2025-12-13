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
    virtual mssm::ImageLoader* getImageLoader() override;
    virtual MeshLoader* getMeshLoader() override;
};

#endif // NANOVGGRAPHICSBACKEND_H
