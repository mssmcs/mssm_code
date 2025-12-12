#ifndef GRAPHICS3D_H
#define GRAPHICS3D_H

#include "canvas3d.h"
#include "graphicsbase.h"
#include "vulkbackendwindow.h"

namespace mssm {

class Graphics3d : public mssm::GraphicsBase<VulkBackendWindow, mssm::Canvas3d>, public mssm::Canvas3dWrapper
{
private:
    MeshLoader* m_meshLoader;
public:
    Graphics3d(std::string title, int width, int height);
    virtual ~Graphics3d() {}

    MeshLoader& meshLoader() {
        if (!m_meshLoader) {
            throw std::runtime_error("Mesh loading not supported by this backend.");
        }
        return *m_meshLoader;
    }
};

}

#endif // GRAPHICS3D_H
