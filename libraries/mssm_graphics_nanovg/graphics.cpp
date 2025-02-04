#include "graphics.h"
#include "nanovggraphicsbackend.h"

using namespace mssm;

GraphicsBackendBase<NanovgWindow, mssm::Canvas2d> *loadGraphicsBackend2d(std::string title,
                                                                         int width,
                                                                         int height)
{
    return new NanovgGraphicsBackend(title, width, height);
}


Graphics::Graphics(std::string title, int width, int height)
    : GraphicsBase(title, width, height,loadGraphicsBackend2d),
    Canvas2dWrapper(backend->getCanvas())
{
}

