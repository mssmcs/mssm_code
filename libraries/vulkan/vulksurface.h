#ifndef VULKSURFACE_H
#define VULKSURFACE_H

#define VK_USE_64_BIT_PTR_DEFINES 1
#include "volk.h"

class VulkSurface
{
private:
    VkInstance instance;
    //GLFWwindow *window;
    VkSurfaceKHR surface{};
public:
    VulkSurface(VkInstance instance, VkSurfaceKHR surfaceHandle);
   // VulkSurface(VkInstance instance, GLFWwindow *window);
   ~VulkSurface();
    VulkSurface(const VulkSurface&) = delete;
    VulkSurface& operator=(const VulkSurface&) = delete;
    VulkSurface(VulkSurface&& other);
    VulkSurface& operator=(VulkSurface&& other);

    operator VkSurfaceKHR() { return surface; }
    VkInstance getInstance() { return instance; }
    //GLFWwindow* getWindow() { return window; }

    void release() { surface = nullptr; }  // release handle to prevent double deletion

};

#endif // VULKSURFACE_H
