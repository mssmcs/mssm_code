#ifndef VULKANWINDOWGLFW_H
#define VULKANWINDOWGLFW_H

#include "vulkabstractwindow.h"
#include "glfwindow.h"

class VulkanWindowGLFW : public gjh::CoreWindowGLFW, public VulkAbstractWindow
{
public:
    VulkanWindowGLFW(std::string title, int width, int height);
    virtual VkExtent2D getExtent() override;
    virtual VkSurfaceKHR createSurface(VkInstance instance) override;

    static std::vector<const char*> getRequiredExtensions();
};

#endif // VULKANWINDOWGLFW_H
