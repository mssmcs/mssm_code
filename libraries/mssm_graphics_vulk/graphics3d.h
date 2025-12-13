#ifndef GRAPHICS3D_H
#define GRAPHICS3D_H

#include "canvas3d.h"
#include "graphicsbase.h"
#include "vulkbackendwindow.h"

namespace mssm {

class Graphics3d
    : public mssm::GraphicsBase<VulkBackendWindow, mssm::Canvas3d>,
      public mssm::Canvas3dWrapper,
      public MeshLoader,
      public mssm::ImageLoader
{
private:
    MeshLoader* m_meshLoader;
    mssm::ImageLoader* m_imageLoader;
public:
    Graphics3d(std::string title, int width, int height);
    virtual ~Graphics3d() {}

    // ImageLoader interface
public:
    std::shared_ptr<ImageInternal> loadImg(std::string filename, bool cachePixels) override;
    std::shared_ptr<ImageInternal> createImg(int width,
                                             int height,
                                             Color c,
                                             bool cachePixels) override;
    std::shared_ptr<ImageInternal> initImg(int width,
                                           int height,
                                           Color *pixels,
                                           bool cachePixels) override;
    void saveImg(std::shared_ptr<ImageInternal> img, std::string filename) override;
    void queueForDestruction(std::shared_ptr<ImageInternal> img) override;

    // MeshLoader interface
public:
    std::shared_ptr<StaticMeshInternal> createMesh(
        const Mesh<EdgeData, VertexData, FaceData> &mesh) override;
    std::shared_ptr<StaticMeshInternal> loadMesh(const std::string &filepath) override;
    void queueForDestruction(std::shared_ptr<StaticMeshInternal> mesh) override;
};
}

#endif // GRAPHICS3D_H
