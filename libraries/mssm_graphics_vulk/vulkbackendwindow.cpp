#include "vulkbackendwindow.h"

#include "vulkwindow.h"
#include "vulkanwindowglfw.h"
#include "vulkcanvas.h"
#include "paths.h"

#include "graphics.h"




//#include <print>

using namespace std;
using namespace gjh;

std::string findVulkanShader(const std::string &filename)
{
    return Paths::findAsset("vulkan\\" + filename);
}


unique_ptr<VulkCanvas> createCanvas(VulkSurfaceRenderManager &renderManager)
{
    return make_unique<VulkCanvas>(renderManager);
}

VulkBackendWindow::VulkBackendWindow(std::string title, int width, int height)
    : VulkanGraphicsWindow<VulkanWindowGLFW, VulkCanvas>(title, width, height, ::createCanvas)
{}

void VulkBackendWindow::beginDrawing(bool wasResized)
{
    VulkanGraphicsWindow<VulkanWindowGLFW, VulkCanvas>::beginDrawing(wasResized);
}

void VulkBackendWindow::endDrawing()
{
    VulkanGraphicsWindow<VulkanWindowGLFW, VulkCanvas>::endDrawing();
}


