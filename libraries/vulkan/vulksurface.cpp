#include "vulksurface.h"
#define VK_USE_64_BIT_PTR_DEFINES 1
#include "volk.h"
#include <stdexcept>
#include <utility>

using namespace std;

VulkSurface::VulkSurface(VkInstance instance, VkSurfaceKHR surfaceHandle)
    : instance{instance}, surface{surfaceHandle}
{
}

// VulkSurface::VulkSurface(VkInstance instance, GLFWwindow *window)
//     : instance(instance)
// {
//     if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
//         throw std::runtime_error("failed to create window surface!");
//     }
// }

VulkSurface::VulkSurface(VulkSurface &&other)
{
    *this = std::move(other);
}

VulkSurface &VulkSurface::operator=(VulkSurface &&other)
{
    instance = other.instance;
    //window = other.window;
    surface = other.surface;
    other.surface = VK_NULL_HANDLE;
    //other.window = nullptr;
    other.instance = VK_NULL_HANDLE;
    return *this;
}

VulkSurface::~VulkSurface()
{
    if (surface) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }
}

