#ifndef GRAPHICS3D_H
#define GRAPHICS3D_H

#include "canvas3d.h"
#include "graphics.h"
#include "vulkbackendwindow.h"

namespace mssm {

class Graphics3d : public mssm::GraphicsBase<VulkBackendWindow, mssm::Canvas3d>, public mssm::Canvas3dWrapper
{
public:
    Graphics3d(std::string title, int width, int height);
    virtual ~Graphics3d() {}
};

}

#endif // GRAPHICS3D_H
