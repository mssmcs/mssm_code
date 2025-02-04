#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "graphicsbase.h"
#include "graphics3d.h"
#include "vulkbackendwindow.h"

namespace mssm {

class Graphics : public mssm::GraphicsBase<VulkBackendWindow, mssm::Canvas3d>, public mssm::Canvas2dWrapper
{
public:
    Graphics(std::string title, int width, int height);
    virtual ~Graphics() {}
};

} // mssm

#endif // GRAPHICS_H
