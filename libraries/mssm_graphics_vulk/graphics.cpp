#include "graphics.h"
#include "vulkgraphicsbackend.h"

using namespace mssm;

GraphicsBackend<VulkBackendWindow, mssm::Canvas3d> *loadGraphicsBackend3d(std::string title, int width, int height)
{
    return new VulkanGraphicsBackend(title, width, height);
}


Graphics::Graphics(std::string title, int width, int height)
    : GraphicsBase(title, width, height,loadGraphicsBackend3d),
    Canvas2dWrapper(backend->getCanvas())
{
}

