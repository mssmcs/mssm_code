#ifndef VULKGRAPHICSBACKEND_H
#define VULKGRAPHICSBACKEND_H

#include "graphicsbackend.h"
#include "vulkbackendwindow.h"

class VulkanGraphicsBackend : public GraphicsBackend<VulkBackendWindow, mssm::Canvas3d>{
public:
    VulkanGraphicsBackend(std::string title, int width, int height);
    virtual ~VulkanGraphicsBackend();

    // GraphicsBackend interface
public:
    void setCursor(mssm::CoreWindowCursor cursor) override;

    // GraphicsBackendBase interface
public:
    mssm::ImageLoader *getImageLoader() override;
    MeshLoader *getMeshLoader() override;
};

#endif // VULKGRAPHICSBACKEND_H
