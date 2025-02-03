#ifndef VULKGRAPHICSBACKEND_H
#define VULKGRAPHICSBACKEND_H

#include "graphicsbackend.h"
#include "vulkbackendwindow.h"

class VulkanGraphicsBackend : public GraphicsBackend<VulkBackendWindow, mssm::Canvas3d> {
public:
    VulkanGraphicsBackend(std::string title, int width, int height);
    virtual ~VulkanGraphicsBackend();

    // GraphicsBackend interface
public:
    void setCursor(gjh::CoreWindowCursor cursor) override;
    std::shared_ptr<mssm::ImageInternal> loadImg(std::string filename, bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> createImg(int width, int height, mssm::Color c, bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> initImg(int width, int height, mssm::Color* pixels, bool cachePixels) override;
    void saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename) override;
};

#endif // VULKGRAPHICSBACKEND_H
