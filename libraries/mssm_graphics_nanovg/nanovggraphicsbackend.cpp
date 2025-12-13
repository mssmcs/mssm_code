#include "nanovggraphicsbackend.h"



NanovgGraphicsBackend::NanovgGraphicsBackend(std::string title, int width, int height)
{
    window = new NanovgWindow(title, width, height);
    window->update(true);
    canvas = window;
}

void NanovgGraphicsBackend::setCursor(mssm::CoreWindowCursor cursor)
{
    window->setCursor(cursor);
}

std::shared_ptr<mssm::ImageInternal> NanovgGraphicsBackend::loadImg(std::string filename,
                                                                    bool cachePixels)
{
    return window->loadImg(filename, cachePixels);
}

std::shared_ptr<mssm::ImageInternal> NanovgGraphicsBackend::createImg(int width,
                                                                      int height,
                                                                      mssm::Color c,
                                                                      bool cachePixels)
{
    return window->createImg(width, height, c, cachePixels);
}

std::shared_ptr<mssm::ImageInternal> NanovgGraphicsBackend::initImg(int width,
                                                                    int height,
                                                                    mssm::Color *pixels,
                                                                    bool cachePixels)
{
    return window->initImg(width, height, pixels, cachePixels);
}

void NanovgGraphicsBackend::saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename)
{
    return window->saveImg(img, filename);
}

void NanovgGraphicsBackend::queueForDestruction(std::shared_ptr<mssm::ImageInternal> img)
{
    // No-op for NanoVG backend
}

NanovgGraphicsBackend::~NanovgGraphicsBackend()
{
    delete window;
}
