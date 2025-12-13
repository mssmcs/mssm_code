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

mssm::ImageLoader *NanovgGraphicsBackend::getImageLoader()
{
    return window;
}

MeshLoader *NanovgGraphicsBackend::getMeshLoader()
{
    return nullptr;
}

NanovgGraphicsBackend::~NanovgGraphicsBackend()
{
    delete window;
}
