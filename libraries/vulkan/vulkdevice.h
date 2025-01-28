#ifndef VULKDEVICE_H
#define VULKDEVICE_H

#include "volk.h"
#include <optional>
#include <vector>

#include <cassert>


void debugVkCall(const char* func, VkResult result);

class QueueIndices
{
public:
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentationFamily;

    bool isComplete(bool needPresentation) {
        return graphicsFamily.has_value() && (!needPresentation || presentationFamily.has_value());
    }
};


class VulkDevice
{
private:
    VkInstance instance;
    VkQueue graphicsQueue{};
    VkQueue presentationQueue{};
    VkPhysicalDevice physicalDevice{};
    VkDevice logicalDevice{};
    int graphicsQueueIndex;
    VkPhysicalDeviceProperties deviceProperties;

public:
    VolkDeviceTable fn;
public:
    VulkDevice(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char *> requiredExtensions); // ok to pass null surface if we're not presenting
   ~VulkDevice();
    VulkDevice(const VulkDevice&) = delete;
    VulkDevice(VulkDevice&& other) = delete;
    VulkDevice& operator=(const VulkDevice&) = delete;
    operator VkDevice() { return logicalDevice; }
    operator VkPhysicalDevice() { return physicalDevice; }
    operator VkInstance() { return instance; }
    int getGraphicsQueueIndex() { return graphicsQueueIndex; }

    VkQueue getGraphicsQueue() { return graphicsQueue; }
    VkQueue getPresentationQueue() { return presentationQueue; }

    static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char *> requiredExtensions);
    static QueueIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
    static VkPhysicalDevice chooosePhysicalDevice(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char *> requiredExtensions);
    static VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const std::vector<const char *> requiredExtensions);
    static bool hasAllExtensions(VkPhysicalDevice device, const std::vector<const char*> requiredExtensions);

    void waitForIdle() { fn.vkDeviceWaitIdle(logicalDevice);}

    VkDeviceSize minUBOffsetAlign() { return deviceProperties.limits.minUniformBufferOffsetAlignment; }

    template <typename F, typename... Args>
    inline auto calld(F func, Args&&... args) // variadic template function with perfect fowarding
    {
        return (fn.*func)(*this, std::forward<Args>(args)...);
    }

    template <typename F, typename... Args>
    inline auto call(F func, Args&&... args) // variadic template function with perfect fowarding
    {
        return (fn.*func)(std::forward<Args>(args)...);
    }
};

#define assertm(exp, msg) assert(((void)msg, exp))

inline void deleteVk(VulkDevice& device, VkDeviceMemory obj) { device.fn.vkFreeMemory(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkBuffer obj) { device.fn.vkDestroyBuffer(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkImage obj) { device.fn.vkDestroyImage(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkImageView obj) { device.fn.vkDestroyImageView(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkShaderModule obj) { device.fn.vkDestroyShaderModule(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkPipeline obj) { device.fn.vkDestroyPipeline(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkPipelineLayout obj) { device.fn.vkDestroyPipelineLayout(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkRenderPass obj) { device.fn.vkDestroyRenderPass(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkFramebuffer obj) { device.fn.vkDestroyFramebuffer(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkDescriptorSetLayout obj) { device.fn.vkDestroyDescriptorSetLayout(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkSampler obj) { device.fn.vkDestroySampler(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkDescriptorPool obj) { device.fn.vkDestroyDescriptorPool(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkCommandPool obj) { device.fn.vkDestroyCommandPool(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkFence obj) { device.fn.vkDestroyFence(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkSemaphore obj) { device.fn.vkDestroySemaphore(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkQueryPool obj) { device.fn.vkDestroyQueryPool(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkBufferView obj) { device.fn.vkDestroyBufferView(device, obj, nullptr); }
inline void deleteVk(VulkDevice& device, VkSwapchainKHR obj) { device.fn.vkDestroySwapchainKHR(device, obj, nullptr); }

template <typename D>
class VulkHasDevIndirect {
public:
    VulkDevice& getDevice()
    {
        D* derivedClass = static_cast<D*>(this);
        return *derivedClass->devPtr;
    }
    VolkDeviceTable* getFn()
    {
        D* derivedClass = static_cast<D*>(this);
        return &derivedClass->devPtr->fn;
    }

    template <typename F, typename... Args>
    auto call_d(F func, Args&&... args) // variadic template function with perfect fowarding
    {
        D* derivedClass = static_cast<D*>(this);
        assertm(derivedClass->devPtr != nullptr, "VulkHasDev::call() called when no device set.  Did you forget to set device (maybe call initDeviceHandle(device)");
        return ((derivedClass->devPtr->fn).*func)(*derivedClass->devPtr, std::forward<Args>(args)...);
    }

    template <typename F, typename... Args>
    auto call_(F func, Args&&... args) // variadic template function with perfect fowarding
    {
        D* derivedClass = static_cast<D*>(this);
        assertm(derivedClass->devPtr != nullptr, "VulkHasDev::call() called when no device set.  Did you forget to set device (maybe call initDeviceHandle(device)");
        return ((derivedClass->devPtr->fn).*func)(std::forward<Args>(args)...);
    }
};

class VulkHasDev  {
private:
    VulkDevice* devPtr;
public:
    VulkHasDev() : devPtr{} {}
    VulkHasDev(VulkDevice& device) : devPtr{&device} {}

    inline bool hasDeviceHandle() const { return devPtr != nullptr; }
    inline VulkDevice& device() const { return *devPtr; }
    inline VolkDeviceTable* fn() const { return &devPtr->fn; }

    template <typename F, typename... Args>
    inline auto calld(F func, Args&&... args)  const // variadic template function with perfect fowarding
    {
        assertm(devPtr != nullptr, "VulkHandle::call() called when no device set.  Did you forget to set device (maybe call initDeviceHandle(device)");
        return ((devPtr->fn).*func)(*devPtr, std::forward<Args>(args)...);
    }

    template <typename F, typename... Args>
    inline auto call(F func, Args&&... args)  const  // variadic template function with perfect fowarding
    {
        assertm(devPtr != nullptr, "VulkHandle::call() called when no device set.  Did you forget to set device (maybe call initDeviceHandle(device)");
        return ((devPtr->fn).*func)(std::forward<Args>(args)...);
    }

    template <typename D>
    friend class VulkHasDevIndirect;

protected:

    void initDeviceHandle(VulkDevice& device)
    {
        assertm(devPtr == nullptr || devPtr == &device, "VulkHandle::initDeviceHandle called when device already set");
        devPtr = &device;
    }

    void resetDeviceHandle(VulkDevice& device)
    {
        devPtr = &device;
    }

    void clearDeviceHandle()
    {
        devPtr = nullptr;
    }
};

template <typename T>
class VulkHandle : public VulkHasDev {
protected:
    T handle;
public:
    VulkHandle() : handle{} {}
    VulkHandle(VulkDevice& device) : VulkHasDev{device}, handle{} {}
    VulkHandle(VulkDevice& device, T handle) : VulkHasDev{device}, handle{handle} {}
    VulkHandle(const VulkHandle<T>&) = delete;
    VulkHandle<T>& operator=(const VulkHandle<T>&) = delete;
    VulkHandle(VulkHandle<T>&& other)
        : VulkHasDev{other.device()}, handle{other.handle}
    {
        other.handle = VK_NULL_HANDLE;
    }

    VulkHandle<T>& operator=(VulkHandle<T>&& other)
    {
        deleteHandle();
        resetDeviceHandle(other.device());
        other.clearDeviceHandle();
        handle = other.handle;
        other.handle = VK_NULL_HANDLE;
        return *this;
    }

    ~VulkHandle() { deleteHandle(); }

protected:
    void deleteHandle()
    {
        if (handle)
        {
            deleteVk(device(), handle);
            handle = nullptr;
        }
    }
public:
    void setHandle(T newHandle)
    {
        assertm(hasDeviceHandle(), "VulkHandle::setHandle() called when no device set");
        deleteHandle();
        handle = newHandle;
    }

    void setHandle(VulkDevice& device, T newHandle)
    {
        deleteHandle();
        resetDeviceHandle(device);
        handle = newHandle;
    }

    inline operator const T() { return handle; }

    bool isHandleValid() const { return handle != VK_NULL_HANDLE; }
};

// #define VKCALLD(func, ...) \
// this->call(&VolkDeviceTable:: func __VA_OPT__(,) __VA_ARGS__)

// use this if derived from VulkHandle and returns a result
#define VKCALLD(func, ...) \
{ \
    VkResult result = this->calld(&VolkDeviceTable:: func __VA_OPT__(,) __VA_ARGS__); \
    if (result != VK_SUCCESS) { \
        debugVkCall(#func, result); \
    } \
}

// use this if derived from VulkHandle and returns void
#define VKCALLD_void(func, ...) \
(this->calld(&VolkDeviceTable:: func __VA_OPT__(,) __VA_ARGS__))

// use this if derived from VulkHandle and returns a result
#define VKCALL(func, ...) \
if (this->call(&VolkDeviceTable:: func __VA_OPT__(,) __VA_ARGS__) != VK_SUCCESS) { \
        throw std::runtime_error("Vulkan call to " #func " failed"); \
}

// use this if derived from VulkHandle and returns void
#define VKCALL_void(func, ...) \
(this->call(&VolkDeviceTable:: func __VA_OPT__(,) __VA_ARGS__))

// use this if derived from VulkHasDevIndirect and returns a result
#define VKCALL_D(func, ...) \
if (this->call_d(&VolkDeviceTable:: func __VA_OPT__(,) __VA_ARGS__) != VK_SUCCESS) { \
    throw std::runtime_error("Vulkan call to " #func " failed"); \
}

// use this if derived from VulkHasDevIndirect and returns void
#define VKCALL_D_void(func, ...) \
(this->call_d(&VolkDeviceTable:: func __VA_OPT__(,) __VA_ARGS__))

// use this if derived from VulkHasDevIndirect and returns a result
#define VKCALL_(func, ...) \
if (this->call_(&VolkDeviceTable:: func __VA_OPT__(,) __VA_ARGS__) != VK_SUCCESS) { \
        throw std::runtime_error("Vulkan call to " #func " failed"); \
}

// use this if derived from VulkHasDevIndirect and returns void
#define VKCALL__void(func, ...) \
(this->call_(&VolkDeviceTable:: func __VA_OPT__(,) __VA_ARGS__))

// this handle allows sharing of the device pointer
// CRTP is used to get the device pointer from the derived class
template <typename T, class D>
class VulkSecondaryHandle {
protected:
    T secondaryHandle;
public:
    VulkSecondaryHandle() : secondaryHandle{} {}
    VulkSecondaryHandle(T handle) : secondaryHandle{handle} {}
    VulkSecondaryHandle(const VulkSecondaryHandle&) = delete;
    VulkSecondaryHandle<T,D>& operator=(const VulkSecondaryHandle<T,D>&) = delete;
    VulkSecondaryHandle(VulkSecondaryHandle<T,D>&& other)
        : secondaryHandle{other.secondaryHandle}
    {
        other.secondaryHandle = VK_NULL_HANDLE;
    }
    VulkSecondaryHandle<T,D>& operator=(VulkSecondaryHandle<T,D>&& other)
    {
        clearSecondaryHandle();
        secondaryHandle = other.secondaryHandle;
        other.secondaryHandle = VK_NULL_HANDLE;
        return *this;
    }

    void deleteSecondary() {
        D* derivedClass = static_cast<D*>(this);
        if (secondaryHandle)
        {
            deleteVk(derivedClass->device(), secondaryHandle);
        }
    }

    ~VulkSecondaryHandle()
    {
        deleteSecondary();
    }
    void setSecondaryHandle(T newHandle)
    {
        deleteSecondary();
        secondaryHandle = newHandle;
    }
    void clearSecondaryHandle()
    {
        deleteSecondary();
        secondaryHandle = VK_NULL_HANDLE;
    }
    operator const T() { return secondaryHandle; }
    T* ptr2() { return &secondaryHandle; }
};



#endif // VULKDEVICE_H
