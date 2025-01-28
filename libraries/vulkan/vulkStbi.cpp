#include "vulkimage.h"
#include "stb_image.h"

VkFormat stbiToVkFormat(int stbiFormat)
{
    switch (stbiFormat) {
    case STBI_grey: return VK_FORMAT_R8_UNORM;
    case STBI_grey_alpha: return VK_FORMAT_R8G8_UNORM;
    case STBI_rgb: return VK_FORMAT_R8G8B8_UNORM;
    case STBI_rgb_alpha: return VK_FORMAT_R8G8B8A8_UNORM;
    default:
        throw std::runtime_error("unsupported stbi format");
    }
}

VulkImageBuffer loadImageIntoBuffer(VulkDevice& device, std::string filename)
{
    int width{};
    int height{};
    int numChannels{};

    auto stbiFormat = STBI_rgb_alpha;
    VkFormat vkFormat = stbiToVkFormat(stbiFormat);

    stbi_uc* pixels = stbi_load(filename.c_str(), &width, &height, &numChannels, stbiFormat);

    VkDeviceSize imageSizePixels = width * height;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    VulkImageBuffer imageBuffer;

    VkExtent3D extent{static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};

    imageBuffer.initialize(device, extent, vkFormat);

    imageBuffer.setPixelsRaw(pixels, imageSizePixels);

    stbi_image_free(pixels);

    return std::move(imageBuffer);
}
