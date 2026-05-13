#ifndef WEBGPU_GLFW_SURFACE_H
#define WEBGPU_GLFW_SURFACE_H

#include <dawn/webgpu.h>

#include <string>

struct GLFWwindow;

namespace mssm {

// Windows: HWND via GLFW native. Other platforms currently unsupported here.
bool webgpuCreateSurfaceFromGlfwWindow(
    WGPUInstance instance,
    GLFWwindow* window,
    WGPUSurface* outSurface,
    std::string& errorMessage);

// On first call (*inOutFormat == WGPUTextureFormat_Undefined), picks a supported format.
// On later calls (e.g. resize), reuses *inOutFormat for wgpuSurfaceConfigure.
bool webgpuConfigureSurfaceForDevice(WGPUSurface surface,
                                     WGPUAdapter adapter,
                                     WGPUDevice device,
                                     uint32_t width,
                                     uint32_t height,
                                     WGPUTextureFormat* inOutFormat,
                                     std::string& errorMessage);

// Clears the swapchain texture to clearColor, submits work, and presents.
bool webgpuRenderSurfaceFrame(WGPUSurface surface,
                              WGPUDevice device,
                              WGPUQueue queue,
                              const WGPUColor& clearColor,
                              std::string& errorMessage);

} // namespace mssm

#endif // WEBGPU_GLFW_SURFACE_H
