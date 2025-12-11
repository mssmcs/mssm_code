#ifndef VULKSWAPCHAIN_H
#define VULKSWAPCHAIN_H

#include <vector>
#include "vulkimageview.h"
#include "vulkimage.h"

class VulkDevice;


class VulkSwapChain : public VulkHandle<VkSwapchainKHR>
{
    struct SwapChainSupportDetails {
        bool hasExtension{false};
        VkSurfaceCapabilitiesKHR capabilities{};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;

        bool supportsSwapchain() { return hasExtension && !formats.empty() && !presentModes.empty(); }
        VkSurfaceFormatKHR chooseSurfaceFormat();
        VkExtent2D chooseSwapExtent(VkExtent2D actualWindowExtent);
        int chooseImageCount();
        VkPresentModeKHR choosePresentMode();
    };
private:
    //GLFWwindow* window;
    std::vector<VkImage> swapChainImages;
    VulkImage depthBuffer;
    VkFormat imageFormat;
    VkExtent2D imageExtent;
public:
    // Note: if window is minimized, swapchain is not created (count() will be 0)
    VulkSwapChain(VulkDevice& device, VkSurfaceKHR surface, VkExtent2D actualWindowExtent, bool createDepthBuffer);
    std::vector<VulkImageView> createImageViews() { return createImageViews(device(), swapChainImages, imageFormat); }
    VulkImageView getDepthBufferImageView() { return depthBuffer.imageView(); }
    VkFormat getImageFormat() const { return imageFormat; }
    VkFormat getDepthBufferFormat() const { return depthBuffer.getFormat(); }
    VkExtent2D getImageExtent() const { return imageExtent; }
    int count() const { return swapChainImages.size(); }
    bool hasSwapChain() const { return handle != VK_NULL_HANDLE; }
private:
    static VkSwapchainKHR createSwapchain(VulkDevice& device, VkSurfaceKHR surface, VkExtent2D actualWindowExtent, VkSwapchainKHR oldSwapChain, VkFormat& imageFormatOut, VkExtent2D& imageExtentOut);
    static std::vector<VkImage> getSwapChainImages(VulkDevice &device, VkSwapchainKHR swapChain);
    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    static std::vector<VulkImageView> createImageViews(VulkDevice& device, const std::vector<VkImage>& images, VkFormat format);
};


#endif // VULKSWAPCHAIN_H
