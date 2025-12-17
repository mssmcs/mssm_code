#include "graphics3d.h"

#include "vulkgraphicsbackend.h"

GraphicsBackend<VulkBackendWindow, mssm::Canvas3d> *loadGraphicsBackend3d(std::string title, int width, int height)
{
    return new VulkanGraphicsBackend(title, width, height);
}

mssm::Graphics3d::Graphics3d(std::string title, int width, int height)
    : GraphicsBase(title, width, height,loadGraphicsBackend3d),
    Canvas3dWrapper(backend->getCanvas1())
{
    m_meshLoader = backend->getMeshLoader();
    m_imageLoader = backend->getImageLoader();
}

std::shared_ptr<mssm::ImageInternal> mssm::Graphics3d::loadImg(std::string filename, bool cachePixels)
{
    return m_imageLoader->loadImg(Paths::findAsset(filename), cachePixels);
}

std::shared_ptr<mssm::ImageInternal> mssm::Graphics3d::createImg(int width,
                                                           int height,
                                                           Color c,
                                                           bool cachePixels)
{
    return m_imageLoader->createImg(width, height, c, cachePixels);
}

std::shared_ptr<mssm::ImageInternal> mssm::Graphics3d::initImg(int width,
                                                         int height,
                                                         Color *pixels,
                                                         bool cachePixels)
{
    return m_imageLoader->initImg(width, height, pixels, cachePixels);
}

void mssm::Graphics3d::saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename)
{
    return m_imageLoader->saveImg(img, filename);
}

void mssm::Graphics3d::queueForDestruction(std::shared_ptr<mssm::ImageInternal> img)
{
    return m_imageLoader->queueForDestruction(img);
}

std::shared_ptr<StaticMeshInternal> mssm::Graphics3d::createMesh(
    const TriangularMesh<Vertex3dUV>& triMesh)
{
    return m_meshLoader->createMesh(triMesh);
}

std::shared_ptr<StaticMeshInternal> mssm::Graphics3d::loadMesh(const std::string &filepath)
{
    return m_meshLoader->loadMesh(filepath);
}

void mssm::Graphics3d::queueForDestruction(std::shared_ptr<StaticMeshInternal> mesh)
{
    return m_meshLoader->queueForDestruction(mesh);
}


std::shared_ptr<StaticMeshInternal> mssm::Graphics3d::createMesh(const TriangularMesh<Vertex3dUV>& triMesh, const Image &texture)
{
    return m_meshLoader->createMesh(triMesh, texture);
}
