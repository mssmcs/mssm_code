#include "vulkdevice.h"
#include "vulkutil.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <optional>
#include <ostream>
#include <set>
#include <stdexcept>
#include <vector>
#include "vk_enum_string_helper.h"
#include <vulkinstance.h>

using namespace std;


VulkDevice::VulkDevice(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char *> requiredExtensions)
: instance(instance)
{
    physicalDevice = chooosePhysicalDevice(instance, surface, requiredExtensions);

    logicalDevice = createLogicalDevice(physicalDevice, surface, requiredExtensions);

    try {
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);


        QueueIndices indices = findQueueFamilies(physicalDevice, surface);

        volkLoadDeviceTable(&fn, logicalDevice);

        graphicsQueueIndex = indices.graphicsFamily.value();

        fn.vkGetDeviceQueue(logicalDevice, graphicsQueueIndex, 0, &graphicsQueue);
        if (indices.presentationFamily.has_value()) {
            fn.vkGetDeviceQueue(logicalDevice, indices.presentationFamily.value(), 0, &presentationQueue);
        }

    } catch (...) {
        fn.vkDestroyDevice(logicalDevice, nullptr);
        throw;
    }
}

VulkDevice::~VulkDevice()
{
    fn.vkDestroyDevice(logicalDevice, nullptr);
}


VkPhysicalDevice VulkDevice::chooosePhysicalDevice(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*> requiredExtensions)
{
    auto devices = getVulkArray<VkInstance, VkPhysicalDevice>(instance, vkEnumeratePhysicalDevices);

    for (const auto& device : devices) {
        if (isDeviceSuitable(device, surface, requiredExtensions)) {
            return device;
        }
    }

    throw std::runtime_error("failed to find a suitable GPU!");
}

VkDevice VulkDevice::createLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const std::vector<const char *> requiredExtensions)
{
    QueueIndices indices = findQueueFamilies(physicalDevice, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies;

    if (indices.graphicsFamily.has_value()) {
        uniqueQueueFamilies.insert(indices.graphicsFamily.value());
    }

    if (indices.presentationFamily.has_value()) {
        uniqueQueueFamilies.insert(indices.presentationFamily.value());
    }

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    deviceFeatures.samplerAnisotropy = VK_TRUE;  // TODO: only need this if we're using the feature (textures)

    VkPhysicalDeviceVulkan12Features features12{};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &features12; // Chain the new features struct

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (validationLayersEnabled()) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(getValidationLayers().size());
        createInfo.ppEnabledLayerNames = getValidationLayers().data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkDevice device;

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    return device;
}


QueueIndices VulkDevice::findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    QueueIndices indices;

    auto queueFamilies = getVulkArray<VkPhysicalDevice, VkQueueFamilyProperties>(physicalDevice, vkGetPhysicalDeviceQueueFamilyProperties);

    for (int i = 0; i < queueFamilies.size(); i++) {
        auto &family = queueFamilies[i];
        bool supportsGraphics = (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) ? true : false;

        VkBool32 presentSupport = false;

        if (surface) {
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        }

        if (supportsGraphics && presentSupport) {
            indices.graphicsFamily = i;
            indices.presentationFamily = i;
            break;
        } else if (supportsGraphics) {
            indices.graphicsFamily = i;
        }
        else if (presentSupport) {
            indices.presentationFamily = i;
        }
    }

    return indices;
}



bool VulkDevice::hasAllExtensions(VkPhysicalDevice device, const std::vector<const char*> requiredExtensions) {

    if (requiredExtensions.empty()) {
        return true;
    }

    auto availableExtensions = getVulkArray<VkPhysicalDevice, const char*, VkExtensionProperties>(device, nullptr, vkEnumerateDeviceExtensionProperties);

    for (auto ext : requiredExtensions) {
        if (std::find_if(availableExtensions.begin(), availableExtensions.end(), [&ext](VkExtensionProperties props) {
            return strcmp(props.extensionName, ext) == 0;
        }) == availableExtensions.end()) {
            // failed to find extension ext
            return false;
        }
    }

    return true;
}

bool VulkDevice::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char*> requiredExtensions)
{
    QueueIndices indices = findQueueFamilies(device, surface);

    bool extensionsSupported = hasAllExtensions(device, requiredExtensions);

    // see https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/03_Physical_devices_and_queue_families.html
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    VkPhysicalDeviceVulkan12Features supportedFeatures12{};
    supportedFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

    VkPhysicalDeviceFeatures2 deviceFeatures2{};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &supportedFeatures12;

    vkGetPhysicalDeviceFeatures2(device, &deviceFeatures2);

    bool nonUniformIndexingSupported = supportedFeatures12.shaderSampledImageArrayNonUniformIndexing;


    return indices.isComplete(surface != VK_NULL_HANDLE) && extensionsSupported && deviceFeatures.samplerAnisotropy && nonUniformIndexingSupported;



  //  return true; // for now just accept anything
}

void debugVkCall(const char *func, VkResult result)
{
    if (result != VK_SUCCESS) {
        std::string resString = string_VkResult(result);
        std::cerr << func << " failed with error code " << resString << std::endl;
        throw std::runtime_error(std::string(func) + " failed with error code " + resString);
    }
}
