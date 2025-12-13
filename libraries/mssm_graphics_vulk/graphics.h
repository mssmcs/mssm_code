#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "graphicsbase.h"
#include "graphics3d.h"
#include "vulkbackendwindow.h"

namespace mssm {

class Graphics
    : public mssm::GraphicsBase<VulkBackendWindow, mssm::Canvas3d>,
      public mssm::Canvas2dWrapper,
      public mssm::ImageLoader
{
protected:
    mssm::ImageLoader* m_imageLoader;
public:
    Graphics(std::string title, int width, int height);
    virtual ~Graphics() {}

    // ImageLoader interface
public:
    std::shared_ptr<mssm::ImageInternal> loadImg(std::string filename, bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> createImg(int width,
                                             int height,
                                             Color c,
                                             bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> initImg(int width,
                                           int height,
                                           Color *pixels,
                                           bool cachePixels) override;
    void saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename) override;
    void queueForDestruction(std::shared_ptr<mssm::ImageInternal> img) override;
};

} // mssm

#endif // GRAPHICS_H
