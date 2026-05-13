#include "dawngraphicsbackend.h"

mssm::EventSource* DawnGraphicsBackend::eventSource()
{
    return nullptr;
}

mssm::Canvas2d* DawnGraphicsBackend::getCanvas1()
{
    return nullptr;
}

mssm::CoreWindow* DawnGraphicsBackend::getWindow1()
{
    return nullptr;
}

void DawnGraphicsBackend::setCursor(mssm::CoreWindowCursor cursor)
{
    (void)cursor;
}

mssm::ImageLoader* DawnGraphicsBackend::getImageLoader()
{
    return nullptr;
}

MeshLoader* DawnGraphicsBackend::getMeshLoader()
{
    return nullptr;
}

bool DawnGraphicsBackend::initialize(std::string& errorMessage)
{
    return context.initialize(errorMessage);
}

