#include "webgpu_context.h"

#include <chrono>
#include <thread>

namespace mssm {

WebGpuContext::~WebGpuContext()
{
    if (device) {
        wgpuDeviceRelease(device);
        device = nullptr;
    }
    if (adapter) {
        wgpuAdapterRelease(adapter);
        adapter = nullptr;
    }
    if (instance) {
        wgpuInstanceRelease(instance);
        instance = nullptr;
    }
}

bool WebGpuContext::initialize(std::string& errorMessage)
{
    WGPUInstanceDescriptor descriptor{};
    instance = wgpuCreateInstance(&descriptor);
    if (instance == nullptr) {
        initialized = false;
        errorMessage = "WebGPU instance creation failed";
        return false;
    }

    initialized = true;
    errorMessage.clear();
    return true;
}

namespace {

struct AdapterRequestState {
    bool done{false};
    bool success{false};
    WGPUAdapter adapter{nullptr};
    std::string error;
};

struct DeviceRequestState {
    bool done{false};
    bool success{false};
    WGPUDevice device{nullptr};
    std::string error;
};

void onAdapterRequestEnded(WGPURequestAdapterStatus status,
                           WGPUAdapter adapter,
                           char const* message,
                           void* userdata)
{
    auto* state = static_cast<AdapterRequestState*>(userdata);
    state->done = true;
    state->success = (status == WGPURequestAdapterStatus_Success && adapter != nullptr);
    state->adapter = adapter;
    if (!state->success) {
        state->error = message ? message : "Adapter request failed";
    }
}

void onDeviceRequestEnded(WGPURequestDeviceStatus status,
                          WGPUDevice device,
                          char const* message,
                          void* userdata)
{
    auto* state = static_cast<DeviceRequestState*>(userdata);
    state->done = true;
    state->success = (status == WGPURequestDeviceStatus_Success && device != nullptr);
    state->device = device;
    if (!state->success) {
        state->error = message ? message : "Device request failed";
    }
}

} // namespace

bool WebGpuContext::requestAdapterAndDevice(std::string& errorMessage, uint32_t timeoutMs)
{
    return requestAdapterAndDevice(nullptr, errorMessage, timeoutMs);
}

bool WebGpuContext::requestAdapterAndDevice(WGPUSurface compatibleSurface,
                                            std::string& errorMessage,
                                            uint32_t timeoutMs)
{
    if (!instance) {
        errorMessage = "WebGPU instance is not initialized";
        return false;
    }

    AdapterRequestState adapterState;
    WGPURequestAdapterOptions adapterOptions{};
    adapterOptions.nextInChain = nullptr;
    adapterOptions.compatibleSurface = compatibleSurface;
    adapterOptions.powerPreference = WGPUPowerPreference_Undefined;
    adapterOptions.backendType = WGPUBackendType_Undefined;
    adapterOptions.forceFallbackAdapter = false;
    adapterOptions.compatibilityMode = false;
    wgpuInstanceRequestAdapter(instance, &adapterOptions, onAdapterRequestEnded, &adapterState);

    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
    while (!adapterState.done && std::chrono::steady_clock::now() < deadline) {
        wgpuInstanceProcessEvents(instance);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (!adapterState.success) {
        errorMessage = adapterState.done ? adapterState.error : "Timed out waiting for adapter";
        return false;
    }

    if (adapter) {
        wgpuAdapterRelease(adapter);
    }
    adapter = adapterState.adapter;

    DeviceRequestState deviceState;
    WGPUDeviceDescriptor deviceDescriptor{};
    wgpuAdapterRequestDevice(adapter, &deviceDescriptor, onDeviceRequestEnded, &deviceState);

    deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
    while (!deviceState.done && std::chrono::steady_clock::now() < deadline) {
        wgpuInstanceProcessEvents(instance);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (!deviceState.success) {
        errorMessage = deviceState.done ? deviceState.error : "Timed out waiting for device";
        return false;
    }

    if (device) {
        wgpuDeviceRelease(device);
    }
    device = deviceState.device;
    errorMessage.clear();
    return true;
}

} // namespace mssm

