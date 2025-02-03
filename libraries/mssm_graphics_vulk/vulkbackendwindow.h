#ifndef VULKBACKENDWINDOW_H
#define VULKBACKENDWINDOW_H

#include "vulkanwindowglfw.h"
#include "vulkcanvas.h"
#include "vulkwindow.h"
#include <string>

class VulkBackendWindow : public VulkanGraphicsWindow<VulkanWindowGLFW, VulkCanvas>
{
public:
    VulkBackendWindow(std::string title, int width, int height);
protected:
    void beginDrawing(bool wasResized) override;
    void endDrawing() override;
};

#endif // VULKBACKENDWINDOW_H
