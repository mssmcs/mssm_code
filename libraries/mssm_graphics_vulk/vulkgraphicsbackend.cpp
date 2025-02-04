#include "vulkgraphicsbackend.h"

void VulkanGraphicsBackend::setCursor(gjh::CoreWindowCursor cursor)
{
    window->setCursor(cursor);
}

std::shared_ptr<mssm::ImageInternal> VulkanGraphicsBackend::loadImg(std::string filename, bool cachePixels)
{
    return window->loadImg(filename, cachePixels);
}

std::shared_ptr<mssm::ImageInternal> VulkanGraphicsBackend::createImg(int width, int height, mssm::Color c, bool cachePixels)
{
    return window->createImg(width, height, c, cachePixels);
}

std::shared_ptr<mssm::ImageInternal> VulkanGraphicsBackend::initImg(int width, int height, mssm::Color* pixels, bool cachePixels)
{
    return window->initImg(width, height, pixels, cachePixels);
}

void VulkanGraphicsBackend::saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename)
{
    window->saveImg(img, filename);
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
