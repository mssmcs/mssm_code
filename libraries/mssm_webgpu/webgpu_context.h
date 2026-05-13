#ifndef WEBGPU_CONTEXT_H
#define WEBGPU_CONTEXT_H

#include <string>
#include <dawn/webgpu.h>

namespace mssm {

class WebGpuContext {
private:
    WGPUInstance instance{nullptr};
    WGPUAdapter adapter{nullptr};
    WGPUDevice device{nullptr};
    bool initialized{false};
public:
    ~WebGpuContext();
    bool initialize(std::string& errorMessage);
    bool requestAdapterAndDevice(std::string& errorMessage, uint32_t timeoutMs = 5000);
    // Pass the swapchain surface so RequestAdapterOptions.compatibleSurface is set (required for
    // wgpuSurfaceGetCapabilities / present on many backends).
    bool requestAdapterAndDevice(WGPUSurface compatibleSurface,
                                 std::string& errorMessage,
                                 uint32_t timeoutMs = 5000);
    bool isInitialized() const { return initialized; }
    WGPUInstance getInstance() const { return instance; }
    WGPUAdapter getAdapter() const { return adapter; }
    WGPUDevice getDevice() const { return device; }
};

} // namespace mssm

#endif // WEBGPU_CONTEXT_H

