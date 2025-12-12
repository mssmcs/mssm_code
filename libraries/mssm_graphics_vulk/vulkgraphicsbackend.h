#ifndef VULKGRAPHICSBACKEND_H
#define VULKGRAPHICSBACKEND_H

#include "graphicsbackend.h"
#include "vulkbackendwindow.h"

class VulkanGraphicsBackend : public GraphicsBackend<VulkBackendWindow, mssm::Canvas3d>, public MeshLoader {
public:
    VulkanGraphicsBackend(std::string title, int width, int height);
    virtual ~VulkanGraphicsBackend();

    // GraphicsBackend interface
public:
    void setCursor(mssm::CoreWindowCursor cursor) override;
    std::shared_ptr<mssm::ImageInternal> loadImg(std::string filename, bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> createImg(int width, int height, mssm::Color c, bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> initImg(int width, int height, mssm::Color* pixels, bool cachePixels) override;
    void saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename) override;

    std::shared_ptr<StaticMeshInternal> createMesh(const Mesh<EdgeData, VertexData, FaceData>& mesh) override;
    std::shared_ptr<StaticMeshInternal> loadMesh(const std::string& filepath) override;
};

#endif // VULKGRAPHICSBACKEND_H
