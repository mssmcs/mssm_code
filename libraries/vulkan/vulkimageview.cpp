#include "vulkimageview.h"
#include <stdexcept>

//VK_IMAGE_ASPECT_DEPTH_BIT
//VK_IMAGE_ASPECT_COLOR_BIT
std::shared_ptr<VulkHandle<VkImageView>> createImageView(VulkDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspectMask)
{    
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;

    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = aspectMask;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;

    if (device.fn.vkCreateImageView(device, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image views!");
    }

    std::shared_ptr<VulkHandle<VkImageView>> view(new VulkHandle<VkImageView>(device, imageView));

    return view;
//    return std::make_shared<VulkHandle<VkImageView>>(device, imageView);
}


VulkImageView::VulkImageView() {}

VulkImageView::VulkImageView(VulkDevice &device, VkImageView handle)
{
    view = std::make_shared<VulkHandle<VkImageView>>(device, handle);
}

VulkImageView::VulkImageView(VulkDevice &device, VkImage image, VkFormat format, VkImageAspectFlags aspectMask)
{
    view = createImageView(device, image, format, aspectMask);
}


