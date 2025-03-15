#include "graphics.h"
#include "vulkgraphicsbackend.h"

using namespace mssm;

GraphicsBackend<VulkBackendWindow, mssm::Canvas3d> *loadGraphicsBackend2d(std::string title, int width, int height)
{
    return new VulkanGraphicsBackend(title, width, height);
}


Graphics::Graphics(std::string title, int width, int height)
    : GraphicsBase(title, width, height,loadGraphicsBackend2d),
    Canvas2dWrapper(backend->getCanvas())
{
}

