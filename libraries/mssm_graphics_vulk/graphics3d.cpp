#include "graphics3d.h"

#include "vulkgraphicsbackend.h"

GraphicsBackend<VulkBackendWindow, mssm::Canvas3d> *loadGraphicsBackend3d(std::string title, int width, int height)
{
    return new VulkanGraphicsBackend(title, width, height);
}

mssm::Graphics3d::Graphics3d(std::string title, int width, int height)
    : GraphicsBase(title, width, height,loadGraphicsBackend3d),
    Canvas3dWrapper(backend->getCanvas())
{
    m_meshLoader = dynamic_cast<MeshLoader*>(backend);
}
