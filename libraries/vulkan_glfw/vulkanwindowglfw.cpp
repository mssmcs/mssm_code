#include "vulkanwindowglfw.h"

#define GLFW_INCLUDE_VULKAN
#define VK_NO_PROTOTYPES
#include "GLFW/glfw3.h"

VulkanWindowGLFW::VulkanWindowGLFW(std::string title, int width, int height)
: CoreWindowGLFW(title, width, height, false)
{

}

VkExtent2D VulkanWindowGLFW::getExtent()
{
    int width, height;
    glfwGetFramebufferSize(getWindow(), &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    return actualExtent;
}

VkSurfaceKHR VulkanWindowGLFW::createSurface(VkInstance instance)
{
    VkSurfaceKHR surfaceHandle;

    if (glfwCreateWindowSurface(instance, getWindow(), nullptr, &surfaceHandle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

    return surfaceHandle;
}

std::vector<const char *> VulkanWindowGLFW::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}
