#include "vulkgraphicsbackend.h"

void VulkanGraphicsBackend::setCursor(mssm::CoreWindowCursor cursor)
{
    window->setCursor(cursor);
}


VulkanGraphicsBackend::VulkanGraphicsBackend(std::string title, int width, int height)
{
    window = new VulkBackendWindow(title, width, height);
    window->update(true); // update actually creates the canvas
    canvas = window->getCanvas();
}

VulkanGraphicsBackend::~VulkanGraphicsBackend()
{
    delete window;
}

mssm::ImageLoader *VulkanGraphicsBackend::getImageLoader()
{
    return window;
}

MeshLoader *VulkanGraphicsBackend::getMeshLoader()
{
    return window;
}
