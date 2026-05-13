#include "webgpu_glfw_surface.h"

#include <GLFW/glfw3.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include <cstring>
#include <sstream>
#include <string>

namespace mssm {

namespace {

const char* wgpuStatusLabel(WGPUStatus status)
{
    switch (status) {
    case WGPUStatus_Success:
        return "Success";
    case WGPUStatus_Error:
        return "Error";
    default:
        return "unknown";
    }
}

} // namespace

bool webgpuCreateSurfaceFromGlfwWindow(
    WGPUInstance instance,
    GLFWwindow* window,
    WGPUSurface* outSurface,
    std::string& errorMessage)
{
    if (!instance || !window || !outSurface) {
        errorMessage = "Invalid arguments to webgpuCreateSurfaceFromGlfwWindow";
        return false;
    }
    *outSurface = nullptr;

#ifndef _WIN32
    errorMessage = "webgpuCreateSurfaceFromGlfwWindow is implemented for Windows only";
    return false;
#else
    WGPUSurfaceDescriptorFromWindowsHWND chain{};
    chain.chain.next = nullptr;
    chain.chain.sType = WGPUSType_SurfaceDescriptorFromWindowsHWND;
    chain.hinstance = GetModuleHandleW(nullptr);
    chain.hwnd = glfwGetWin32Window(window);
    if (chain.hwnd == nullptr) {
        errorMessage = "glfwGetWin32Window returned null (cannot create Win32 surface); GetLastError=";
        errorMessage += std::to_string(static_cast<unsigned long>(GetLastError()));
        return false;
    }

    WGPUSurfaceDescriptor desc{};
    desc.nextInChain = &chain.chain;
    desc.label = nullptr;

    WGPUSurface surface = wgpuInstanceCreateSurface(instance, &desc);
    if (!surface) {
        errorMessage = "wgpuInstanceCreateSurface failed";
        return false;
    }
    *outSurface = surface;
    errorMessage.clear();
    return true;
#endif
}

bool webgpuConfigureSurfaceForDevice(WGPUSurface surface,
                                     WGPUAdapter adapter,
                                     WGPUDevice device,
                                     uint32_t width,
                                     uint32_t height,
                                     WGPUTextureFormat* inOutFormat,
                                     std::string& errorMessage)
{
    if (!surface || !adapter || !device || !inOutFormat) {
        errorMessage = "Invalid arguments to webgpuConfigureSurfaceForDevice";
        return false;
    }
    if (width == 0 || height == 0) {
        errorMessage = "Surface configure requires non-zero width and height";
        return false;
    }

    WGPUSurfaceCapabilities caps{};
    const WGPUStatus capStatus = wgpuSurfaceGetCapabilities(surface, adapter, &caps);
    if (capStatus != WGPUStatus_Success) {
        std::ostringstream oss;
        oss << "wgpuSurfaceGetCapabilities failed: status=" << wgpuStatusLabel(capStatus) << " ("
            << static_cast<unsigned>(capStatus) << ") size=" << width << "x" << height
            << " surface=" << static_cast<void*>(surface) << " adapter=" << static_cast<void*>(adapter)
            << ". The C API does not return a reason string; common causes are an incompatible "
               "surface/adapter pair (request the adapter with compatibleSurface set) or a bad Win32 "
               "HWND chain. Enable Dawn validation/logging for implementation-defined details.";
        errorMessage = oss.str();
        return false;
    }

    WGPUTextureFormat format = *inOutFormat;
    if (format == WGPUTextureFormat_Undefined) {
        for (size_t i = 0; i < caps.formatCount; ++i) {
            if (caps.formats[i] == WGPUTextureFormat_BGRA8Unorm) {
                format = caps.formats[i];
                break;
            }
        }
        if (format == WGPUTextureFormat_Undefined && caps.formatCount > 0) {
            format = caps.formats[0];
        }
    }
    wgpuSurfaceCapabilitiesFreeMembers(caps);

    if (format == WGPUTextureFormat_Undefined) {
        errorMessage = "No suitable surface texture format reported by adapter";
        return false;
    }

    WGPUSurfaceConfiguration config{};
    config.nextInChain = nullptr;
    config.device = device;
    config.format = format;
    config.usage = WGPUTextureUsage_RenderAttachment;
    config.viewFormatCount = 0;
    config.viewFormats = nullptr;
    config.alphaMode = WGPUCompositeAlphaMode_Auto;
    config.width = width;
    config.height = height;
    config.presentMode = WGPUPresentMode_Fifo;

    wgpuSurfaceConfigure(surface, &config);
    *inOutFormat = format;
    errorMessage.clear();
    return true;
}

bool webgpuRenderSurfaceFrame(WGPUSurface surface,
                              WGPUDevice device,
                              WGPUQueue queue,
                              const WGPUColor& clearColor,
                              std::string& errorMessage)
{
    if (!surface || !device || !queue) {
        errorMessage = "Invalid arguments to webgpuRenderSurfaceFrame";
        return false;
    }

    WGPUSurfaceTexture surfaceTexture{};
    wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);
    if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
        errorMessage = "wgpuSurfaceGetCurrentTexture did not return Success";
        return false;
    }
    if (!surfaceTexture.texture) {
        errorMessage = "wgpuSurfaceGetCurrentTexture returned null texture";
        return false;
    }

    WGPUTextureViewDescriptor viewDesc{};
    viewDesc.nextInChain = nullptr;
    viewDesc.label = nullptr;
    viewDesc.format = WGPUTextureFormat_Undefined;
    viewDesc.dimension = WGPUTextureViewDimension_2D;
    viewDesc.baseMipLevel = 0;
    viewDesc.mipLevelCount = 1;
    viewDesc.baseArrayLayer = 0;
    viewDesc.arrayLayerCount = 1;
    viewDesc.aspect = WGPUTextureAspect_All;

    WGPUTextureView textureView = wgpuTextureCreateView(surfaceTexture.texture, &viewDesc);
    if (!textureView) {
        wgpuTextureRelease(surfaceTexture.texture);
        errorMessage = "wgpuTextureCreateView failed for surface texture";
        return false;
    }

    WGPURenderPassColorAttachment colorAttachment{};
    colorAttachment.nextInChain = nullptr;
    colorAttachment.view = textureView;
    colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    colorAttachment.resolveTarget = nullptr;
    colorAttachment.loadOp = WGPULoadOp_Clear;
    colorAttachment.storeOp = WGPUStoreOp_Store;
    colorAttachment.clearValue = clearColor;

    WGPURenderPassDescriptor renderPassDesc{};
    renderPassDesc.nextInChain = nullptr;
    renderPassDesc.label = nullptr;
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &colorAttachment;
    renderPassDesc.depthStencilAttachment = nullptr;
    renderPassDesc.occlusionQuerySet = nullptr;
    renderPassDesc.timestampWrites = nullptr;

    WGPUCommandEncoderDescriptor encoderDesc{};
    encoderDesc.nextInChain = nullptr;
    encoderDesc.label = nullptr;

    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);
    if (!encoder) {
        wgpuTextureViewRelease(textureView);
        wgpuTextureRelease(surfaceTexture.texture);
        errorMessage = "wgpuDeviceCreateCommandEncoder failed";
        return false;
    }

    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
    if (!pass) {
        wgpuCommandEncoderRelease(encoder);
        wgpuTextureViewRelease(textureView);
        wgpuTextureRelease(surfaceTexture.texture);
        errorMessage = "wgpuCommandEncoderBeginRenderPass failed";
        return false;
    }

    wgpuRenderPassEncoderEnd(pass);
    wgpuRenderPassEncoderRelease(pass);

    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(encoder, nullptr);
    wgpuCommandEncoderRelease(encoder);

    if (!commandBuffer) {
        wgpuTextureViewRelease(textureView);
        wgpuTextureRelease(surfaceTexture.texture);
        errorMessage = "wgpuCommandEncoderFinish failed";
        return false;
    }

    wgpuQueueSubmit(queue, 1, &commandBuffer);
    wgpuCommandBufferRelease(commandBuffer);

    wgpuTextureViewRelease(textureView);

    wgpuSurfacePresent(surface);
    wgpuTextureRelease(surfaceTexture.texture);

    errorMessage.clear();
    return true;
}

} // namespace mssm
