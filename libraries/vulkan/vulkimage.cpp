#include "vulkimage.h"
#include "vulkcommandbuffers.h"


VkFormat findSupportedFormat(VulkDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;

        vkGetPhysicalDeviceFormatProperties(device, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat findDepthFormat(VulkDevice& device) {
    return findSupportedFormat(device,
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
}

bool hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}



VulkImage::VulkImage(VulkCommandPool &commandPool, std::string filename, bool retainBuffer)
{
    load(commandPool, filename, retainBuffer);
}

VulkImage::VulkImage(VulkCommandPool &commandPool, int width, int height, VkFormat format, bool retainBuffer)
{
    create(commandPool, width, height, format, retainBuffer);
}

void VulkImage::load(VulkCommandPool& commandPool, std::string filename, bool retainBuffer)
{
    VulkImageBuffer imageBuffer = loadImageIntoBuffer(commandPool.device(), filename);

    initializeImage(commandPool.device(), imageBuffer.width(), imageBuffer.height(),
                   imageBuffer.getFormat(),
                   VK_IMAGE_TILING_OPTIMAL,
                   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    loadBufferIntoImage(commandPool, imageBuffer);

    if (retainBuffer) {
        stagingBuffer = std::move(imageBuffer);
    }
}

void VulkImage::create(VulkCommandPool &commandPool, int width, int height, VkFormat format, bool retainBuffer)
{
    this->format = format;

    initializeImage(commandPool.device(), width, height,
                    format,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    transitionImageLayout(commandPool, handle, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    transitionImageLayout(commandPool, handle, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (retainBuffer) {
        stagingBuffer.initialize(commandPool.device(), extent, format);
    }
}

void VulkImage::createDepthBuffer(VulkDevice &device, int width, int height)
{
    VkFormat depthFormat = findDepthFormat(device);

    initializeImage(device, width, height,
                    depthFormat,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void IterateFormats(VkPhysicalDevice device, VkFormat min, VkFormat max)
{
    for(int fmt = min; fmt <= max; ++fmt)
    {
        VkFormatProperties properties{};
        VkFormat vkFmt = static_cast<VkFormat>(fmt);
        vkGetPhysicalDeviceFormatProperties(device, vkFmt, &properties);
        if (FormatByteSize(vkFmt) == 1 &&
            (properties.linearTilingFeatures &  VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) &&
            (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)){
            std::cout << properties.linearTilingFeatures << std::endl;
        }
        //SaveFormat(fmt, properties);
    }
}

void IterateFormatGroups(VkPhysicalDevice device)
{
    // VK_VERSION_1_0
    IterateFormats(device, VK_FORMAT_R4G4_UNORM_PACK8, VK_FORMAT_B10G11R11_UFLOAT_PACK32); // VK_FORMAT_ASTC_12x12_SRGB_BLOCK);

}
void VulkImage::initializeImage(VulkDevice& device,
                       int width,
                       int height,
                       VkFormat format,
                       VkImageTiling tiling,
                       VkImageUsageFlags usage,
                       VkMemoryPropertyFlags properties)
{
    this->format = format;

   // IterateFormatGroups(commandPool.device());

    initDeviceHandle(device);

    extent.width = width;
    extent.height = height;
    extent.depth = 1;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;

    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional

    VKCALLD(vkCreateImage, &imageInfo, nullptr, &handle);

    VkMemoryRequirements memRequirements;
    VKCALLD_void(vkGetImageMemoryRequirements, handle, &memRequirements);

    textureImageMemory.initMemory(device, memRequirements.size, memRequirements.memoryTypeBits, properties);

    VKCALLD(vkBindImageMemory, handle, textureImageMemory, 0);

    VkImageAspectFlags aspectMask{};

    if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    else {
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;  // default assume wants color info
    }

    view = VulkImageView(device, handle, format, aspectMask);
}
// https://stackoverflow.com/questions/40574668/how-to-update-texture-for-every-frame-in-vulkan
//         https://www.asawicki.info/news_1698_vulkan_sparse_binding_-_a_quick_overview

//                 https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_EXT_external_memory_host.html
//                         https://www.reddit.com/r/vulkan/comments/fv8xks/is_going_through_mapped_memory_the_onlybest_way/


void VulkImage::loadBufferIntoImage(VulkCommandPool& commandPool, VulkImageBuffer &imageBuffer)
{
    transitionImageLayout(commandPool, handle, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(commandPool, imageBuffer, handle, imageBuffer.width(), imageBuffer.height());
    transitionImageLayout(commandPool, handle, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void copyBuffer(VulkCommandPool& commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VulkCommandBuffer::oneTimeCommmand(commandPool, [srcBuffer, dstBuffer, size](VulkDevice& device, VkCommandBuffer cmdBuffer) {
        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        device.fn.vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    });
}

void transitionImageLayout(VulkCommandPool& commandPool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VulkCommandBuffer::oneTimeCommmand(commandPool, [image, format, oldLayout, newLayout](VulkDevice& device, VkCommandBuffer cmdBuffer) {

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL &&
                   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_GENERAL) {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
                   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        device.fn.vkCmdPipelineBarrier(
            cmdBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
            );
    });
}

void copyBufferToImage(VulkCommandPool& commandPool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VulkCommandBuffer::oneTimeCommmand(commandPool, [buffer, image, width, height](VulkDevice& device, VkCommandBuffer cmdBuffer) {

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        device.fn.vkCmdCopyBufferToImage(
            cmdBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
            );
    });
}

VulkSampler::VulkSampler(VulkDevice& device)
    : VulkHandle<VkSampler>(device)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(device, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VKCALLD(vkCreateSampler, &samplerInfo, nullptr, &handle);
}


VulkImageBuffer::VulkImageBuffer(VulkDevice &device, VkExtent3D extent, VkFormat format)
{
    initialize(device, extent, format);
}


void VulkImageBuffer::initialize(VulkDevice &device, VkExtent3D extent, VkFormat format)
{
    bytesPerPixel = FormatByteSize(format);
    this->extent = extent;
    this->format = format;
    VkDeviceSize numBytes = extent.width * extent.height * extent.depth * bytesPerPixel;
    initializeRaw(device, numBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    ensureMapped();
}

size_t FormatByteSize(VkFormat fmt)
{
    switch (fmt) {
    case VK_FORMAT_R4G4_UNORM_PACK8: return 1;
    case VK_FORMAT_R8_UNORM: return 1;
    case VK_FORMAT_R8_SNORM: return 1;
    case VK_FORMAT_R8_USCALED: return 1;
    case VK_FORMAT_R8_SSCALED: return 1;
    case VK_FORMAT_R8_UINT: return 1;
    case VK_FORMAT_R8_SINT: return 1;
    case VK_FORMAT_R8_SRGB: return 1;
    case VK_FORMAT_A8_UNORM_KHR: return 1;

    case VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR: return 2;
    case VK_FORMAT_R10X6_UNORM_PACK16: return 2;
    case VK_FORMAT_R12X4_UNORM_PACK16: return 2;
    case VK_FORMAT_A4R4G4B4_UNORM_PACK16: return 2;
    case VK_FORMAT_A4B4G4R4_UNORM_PACK16: return 2;
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return 2;
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return 2;
    case VK_FORMAT_R5G6B5_UNORM_PACK16: return 2;
    case VK_FORMAT_B5G6R5_UNORM_PACK16: return 2;
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return 2;
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return 2;
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return 2;
    case VK_FORMAT_R8G8_UNORM: return 2;
    case VK_FORMAT_R8G8_SNORM: return 2;
    case VK_FORMAT_R8G8_USCALED: return 2;
    case VK_FORMAT_R8G8_SSCALED: return 2;
    case VK_FORMAT_R8G8_UINT: return 2;
    case VK_FORMAT_R8G8_SINT: return 2;
    case VK_FORMAT_R8G8_SRGB: return 2;
    case VK_FORMAT_R16_UNORM: return 2;
    case VK_FORMAT_R16_SNORM: return 2;
    case VK_FORMAT_R16_USCALED: return 2;
    case VK_FORMAT_R16_SSCALED: return 2;
    case VK_FORMAT_R16_UINT: return 2;
    case VK_FORMAT_R16_SINT: return 2;
    case VK_FORMAT_R16_SFLOAT: return 2;


              // 24-bit
              //     Block size 3 byte
              // 1x1x1 block extent
              // 1 texel/block

    case VK_FORMAT_R8G8B8_UNORM: return 3;
    case VK_FORMAT_R8G8B8_SNORM: return 3;
    case VK_FORMAT_R8G8B8_USCALED: return 3;
    case VK_FORMAT_R8G8B8_SSCALED: return 3;
    case VK_FORMAT_R8G8B8_UINT: return 3;
    case VK_FORMAT_R8G8B8_SINT: return 3;
    case VK_FORMAT_R8G8B8_SRGB: return 3;
    case VK_FORMAT_B8G8R8_UNORM: return 3;
    case VK_FORMAT_B8G8R8_SNORM: return 3;
    case VK_FORMAT_B8G8R8_USCALED: return 3;
    case VK_FORMAT_B8G8R8_SSCALED: return 3;
    case VK_FORMAT_B8G8R8_UINT: return 3;
    case VK_FORMAT_B8G8R8_SINT: return 3;
    case VK_FORMAT_B8G8R8_SRGB: return 3;

        // 32-bit
        //           Block size 4 byte
        //       1x1x1 block extent
        //       1 texel/block

    case VK_FORMAT_R10X6G10X6_UNORM_2PACK16: return 4;
    case VK_FORMAT_R12X4G12X4_UNORM_2PACK16: return 4;
    case VK_FORMAT_R16G16_SFIXED5_NV: return 4;
    case VK_FORMAT_R8G8B8A8_UNORM: return 4;
    case VK_FORMAT_R8G8B8A8_SNORM: return 4;
    case VK_FORMAT_R8G8B8A8_USCALED: return 4;
    case VK_FORMAT_R8G8B8A8_SSCALED: return 4;
    case VK_FORMAT_R8G8B8A8_UINT: return 4;
    case VK_FORMAT_R8G8B8A8_SINT: return 4;
    case VK_FORMAT_R8G8B8A8_SRGB: return 4;
    case VK_FORMAT_B8G8R8A8_UNORM: return 4;
    case VK_FORMAT_B8G8R8A8_SNORM: return 4;
    case VK_FORMAT_B8G8R8A8_USCALED: return 4;
    case VK_FORMAT_B8G8R8A8_SSCALED: return 4;
    case VK_FORMAT_B8G8R8A8_UINT: return 4;
    case VK_FORMAT_B8G8R8A8_SINT: return 4;
    case VK_FORMAT_B8G8R8A8_SRGB: return 4;
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32: return 4;
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32: return 4;
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32: return 4;
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: return 4;
    case VK_FORMAT_A8B8G8R8_UINT_PACK32: return 4;
    case VK_FORMAT_A8B8G8R8_SINT_PACK32: return 4;
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32: return 4;
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return 4;
    case VK_FORMAT_A2R10G10B10_SNORM_PACK32: return 4;
    case VK_FORMAT_A2R10G10B10_USCALED_PACK32: return 4;
    case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: return 4;
    case VK_FORMAT_A2R10G10B10_UINT_PACK32: return 4;
    case VK_FORMAT_A2R10G10B10_SINT_PACK32: return 4;
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return 4;
    case VK_FORMAT_A2B10G10R10_SNORM_PACK32: return 4;
    case VK_FORMAT_A2B10G10R10_USCALED_PACK32: return 4;
    case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: return 4;
    case VK_FORMAT_A2B10G10R10_UINT_PACK32: return 4;
    case VK_FORMAT_A2B10G10R10_SINT_PACK32: return 4;
    case VK_FORMAT_R16G16_UNORM: return 4;
    case VK_FORMAT_R16G16_SNORM: return 4;
    case VK_FORMAT_R16G16_USCALED: return 4;
    case VK_FORMAT_R16G16_SSCALED: return 4;
    case VK_FORMAT_R16G16_UINT: return 4;
    case VK_FORMAT_R16G16_SINT: return 4;
    case VK_FORMAT_R16G16_SFLOAT: return 4;
    case VK_FORMAT_R32_UINT: return 4;
    case VK_FORMAT_R32_SINT: return 4;
    case VK_FORMAT_R32_SFLOAT: return 4;
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return 4;
    case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: return 4;

    default:
        throw std::runtime_error("unsupported format: add a case for this");
    }
}
