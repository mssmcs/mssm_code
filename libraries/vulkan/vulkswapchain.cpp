#include "vulkswapchain.h"
#include "vulkdevice.h"
#include "vulkutil.h"
#include "vulkimage.h"
#include <algorithm>
#include <limits>

VulkSwapChain::VulkSwapChain(VulkDevice &device, VkSurfaceKHR surface, VkExtent2D actualWindowExtent, bool createDepthBuffer)
    : VulkHandle<VkSwapchainKHR>(device) //, window(window)
{
    handle = createSwapchain(device, surface, actualWindowExtent, VK_NULL_HANDLE, imageFormat, imageExtent);
    if (handle) {
        swapChainImages = getSwapChainImages(device, handle);
        if (createDepthBuffer) {
            depthBuffer.createDepthBuffer(device, imageExtent.width, imageExtent.height);
        }
    }
    else {
        // if window is minimized, swapchain is not created
    }
}


VkSwapchainKHR VulkSwapChain::createSwapchain(VulkDevice &device,
                                              VkSurfaceKHR surface,
                                              VkExtent2D actualWindowExtent,
                                              VkSwapchainKHR oldSwapChain,
                                              VkFormat &imageFormatOut,
                                              VkExtent2D &imageExtentOut)
{
    auto supportDetails = querySwapChainSupport(device, surface);

    if (!supportDetails.supportsSwapchain()) {
        throw std::runtime_error("surface doesn't support swapchain!");
    }

    QueueIndices queueIndices = VulkDevice::findQueueFamilies(device, surface);

    VkSurfaceFormatKHR surfaceFormat = supportDetails.chooseSurfaceFormat();

    imageFormatOut = surfaceFormat.format;
    imageExtentOut= supportDetails.chooseSwapExtent(actualWindowExtent);

    if (imageExtentOut.width == 0 || imageExtentOut.height == 0) {
        return {}; // window minimized
    }


    int imageCount = supportDetails.chooseImageCount();

    // // https://stackoverflow.com/questions/64150186/lets-get-swapchains-image-count-straight

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = imageFormatOut;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = imageExtentOut;
    createInfo.imageArrayLayers = 1; // always 1 unless stereoscopic 3D application
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT for post-processing

    uint32_t queueFamilyIndices[] = {queueIndices.graphicsFamily.value(), queueIndices.presentationFamily.value()};

    if (queueIndices.graphicsFamily != queueIndices.presentationFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = supportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = supportDetails.choosePresentMode();

    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = oldSwapChain;

    VkSwapchainKHR newSwapChain;

    if (device.fn.vkCreateSwapchainKHR(device, &createInfo, nullptr, &newSwapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    if (oldSwapChain) {
        device.fn.vkDestroySwapchainKHR(device, oldSwapChain, nullptr);
    }

    return newSwapChain;
}

std::vector<VkImage> VulkSwapChain::getSwapChainImages(VulkDevice& device, VkSwapchainKHR swapChain)
{
    return getVulkArray<VulkDevice&, VkSwapchainKHR, VkImage>(device, swapChain, device.fn.vkGetSwapchainImagesKHR);
}

VulkSwapChain::SwapChainSupportDetails VulkSwapChain::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    VulkSwapChain::SwapChainSupportDetails details;

    details.hasExtension = VulkDevice::hasAllExtensions(device, {VK_KHR_SWAPCHAIN_EXTENSION_NAME});

    if (!details.hasExtension) {
        return details;
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    details.formats = getVulkArray<VkPhysicalDevice, VkSurfaceKHR, VkSurfaceFormatKHR>(device, surface, vkGetPhysicalDeviceSurfaceFormatsKHR);
    details.presentModes = getVulkArray<VkPhysicalDevice, VkSurfaceKHR, VkPresentModeKHR>(device, surface, vkGetPhysicalDeviceSurfacePresentModesKHR);

    return details;
}

std::vector<VulkImageView> VulkSwapChain::createImageViews(VulkDevice &device, const std::vector<VkImage> &images, VkFormat format)
{
    std::vector<VulkImageView> views;

    for (size_t i = 0; i < images.size(); i++) {
        VulkImageView view(device, images[i], format, VK_IMAGE_ASPECT_COLOR_BIT);
        views.push_back(view);
    }

    return views;
}

VkSurfaceFormatKHR VulkSwapChain::SwapChainSupportDetails::chooseSurfaceFormat()
{
    if (formats.empty()) {
        throw std::runtime_error("no formats available in chooseSwapSurfaceFormat");
    }

    for (const auto &availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return formats[0];
}



VkExtent2D VulkSwapChain::SwapChainSupportDetails::chooseSwapExtent(VkExtent2D actualWindowExtent)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }



    actualWindowExtent.width = std::clamp(actualWindowExtent.width,
                                          capabilities.minImageExtent.width,
                                          capabilities.maxImageExtent.width);
    actualWindowExtent.height = std::clamp(actualWindowExtent.height,
                                           capabilities.minImageExtent.height,
                                           capabilities.maxImageExtent.height);

    return actualWindowExtent;
}

int VulkSwapChain::SwapChainSupportDetails::chooseImageCount()
{
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }
    return imageCount;
}

VkPresentModeKHR VulkSwapChain::SwapChainSupportDetails::choosePresentMode()
{
    //return VK_PRESENT_MODE_IMMEDIATE_KHR;

    for (const auto &availablePresentMode : presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}
