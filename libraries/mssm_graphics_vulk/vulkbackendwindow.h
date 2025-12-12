#ifndef VULKBACKENDWINDOW_H
#define VULKBACKENDWINDOW_H

#include "vulkanwindowglfw.h"
#include "vulkcanvas.h"
#include "vulkwindow.h"
#include "staticmesh.h"
#include <string>

class VulkBackendWindow : public VulkanGraphicsWindow<VulkanWindowGLFW, VulkCanvas>
{
public:
    VulkBackendWindow(std::string title, int width, int height);

    std::shared_ptr<StaticMeshInternal> createMesh(const Mesh<EdgeData, VertexData, FaceData>& mesh);
    std::shared_ptr<StaticMeshInternal> loadMesh(const std::string& filepath);

protected:
    void beginDrawing(bool wasResized) override;
    void endDrawing() override;
};

#endif // VULKBACKENDWINDOW_H
