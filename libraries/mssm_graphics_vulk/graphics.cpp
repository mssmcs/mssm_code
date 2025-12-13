#include "graphics.h"
#include "vulkgraphicsbackend.h"

using namespace mssm;

GraphicsBackend<VulkBackendWindow, mssm::Canvas3d> *loadGraphicsBackend2d(std::string title, int width, int height)
{
    return new VulkanGraphicsBackend(title, width, height);
}

Graphics::Graphics(std::string title, int width, int height)
    : GraphicsBase(title, width, height,loadGraphicsBackend2d),
    Canvas2dWrapper(backend->getCanvas1())
{
    m_imageLoader = backend->getImageLoader();
}

std::shared_ptr<mssm::ImageInternal> mssm::Graphics::loadImg(std::string filename, bool cachePixels)
{
    return m_imageLoader->loadImg(Paths::findAsset(filename), cachePixels);
}

std::shared_ptr<mssm::ImageInternal> mssm::Graphics::createImg(int width,
                                                         int height,
                                                         Color c,
                                                         bool cachePixels)
{
    return m_imageLoader->createImg(width, height, c, cachePixels);
}

std::shared_ptr<mssm::ImageInternal> mssm::Graphics::initImg(int width,
                                                       int height,
                                                       Color *pixels,
                                                       bool cachePixels)
{
    return m_imageLoader->initImg(width, height, pixels, cachePixels);
}

void mssm::Graphics::saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename)
{
    m_imageLoader->saveImg(img, filename);
}

void mssm::Graphics::queueForDestruction(std::shared_ptr<mssm::ImageInternal> img)
{
    m_imageLoader->queueForDestruction(img);
}
