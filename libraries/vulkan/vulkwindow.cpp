#include "vulkwindow.h"
#include <iostream>
#include <memory>
#include "image.h"

// #define GLFW_INCLUDE_VULKAN
// #define VK_NO_PROTOTYPES
// #include "GLFW/glfw3.h"

using namespace std;
using namespace mssm;

// std::vector<const char*> getRequiredExtensions() {
//     uint32_t glfwExtensionCount = 0;
//     const char** glfwExtensions;
//     glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

//     std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

//     return extensions;
// }

// VkExtent2D VulkWindowGLFW::getExtent()
// {
//     int width, height;
//     glfwGetFramebufferSize(window, &width, &height);

//     VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

//     return actualExtent;
// }

// VkSurfaceKHR VulkWindowGLFW::createSurface(VkInstance instance)
// {
//     VkSurfaceKHR surfaceHandle;

//     if (glfwCreateWindowSurface(instance, window, nullptr, &surfaceHandle) != VK_SUCCESS) {
//         throw std::runtime_error("failed to create window surface!");
//     }

//     return surfaceHandle;
// }

