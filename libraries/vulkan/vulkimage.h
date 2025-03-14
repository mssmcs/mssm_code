#ifndef VULKIMAGE_H
#define VULKIMAGE_H

#include "vulkbuffer.h"
#include "vulkcommandbuffers.h"
#include "vulkimageview.h"
#include <cstring>

size_t FormatByteSize(VkFormat fmt);
void copyBuffer(VulkCommandPool& commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void copyBufferToImage(VulkCommandPool& commandPool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
void transitionImageLayout(VulkCommandPool& commandPool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);



class VulkImageBuffer : public VulkRawBuffer
{
    VkExtent3D extent{};
    VkFormat format{};
    size_t bytesPerPixel{};
public:
    VulkImageBuffer() {}
    VulkImageBuffer(VulkDevice& device, VkExtent3D extent, VkFormat format);
    VulkImageBuffer(VulkImageBuffer& other) = delete;
    VulkImageBuffer& operator=(VulkImageBuffer& other) = delete;

    VulkImageBuffer(VulkImageBuffer &&other)
        : VulkRawBuffer(std::move(other))
        , extent(other.extent)
        , format(other.format)
        , bytesPerPixel(other.bytesPerPixel)
    {
    }
    VulkImageBuffer& operator=(VulkImageBuffer&& other) {
        VulkRawBuffer::operator=(std::move(other));
        extent = other.extent;
        format = other.format;
        bytesPerPixel = other.bytesPerPixel;
        return *this;
    }

    uint32_t width() const { return extent.width; }
    uint32_t height() const { return extent.height; }
    uint32_t depth() const { return extent.depth; }
    VkFormat getFormat() const { return format; }
    const VkExtent3D& getExtent() const { return extent; }

    template <typename PIXEL>
    PIXEL* data() { return mapRaw<PIXEL>(); }

    inline uint32_t numPixels() const { return this->sizeBytes()/bytesPerPixel; }

    VkRect2D getRect2d() const {
        return {{0, 0}, {extent.width, extent.height}};
    }

    // set a rectangle of pixels.  The span of pixels must be the same size as the rectangle
    template <typename PIXEL>
    void setPixels(VkRect2D rect, std::span<const PIXEL> pixels) {
        assertm(bytesPerPixel == sizeof(PIXEL), "VulkImageBuffer::setPixels() called with wrong pixel type");
        assertm(rect.extent.width * rect.extent.height == pixels.size(), "VulkImageBuffer::setPixels() called with span of wrong size");
        auto source = pixels.data();
        auto destination = data<PIXEL>() + rect.offset.y * extent.width + rect.offset.x;
        for (int y = 0; y < rect.extent.height; y++) {
            memcpy(destination, source, rect.extent.width * sizeof(PIXEL));
            source += rect.extent.width;
            destination += extent.width;
        }
    }

    // set a sub-rectangle of pixels.  The span of pixels must be the same size as the entire image
    template <typename PIXEL>
    void updatePixels(VkRect2D rect, std::span<const PIXEL> fullSizePixels)
    {
        assertm(bytesPerPixel == sizeof(PIXEL), "VulkImageBuffer::updatePixels() called with wrong pixel type");
        auto source = fullSizePixels.data() + rect.offset.y * extent.width + rect.offset.x;
        auto destination = data<PIXEL>() + rect.offset.y * extent.width + rect.offset.x;

        for (int y = 0; y < rect.extent.height; y++) {
            memcpy(destination, source, rect.extent.width * sizeof(PIXEL));
            source += extent.width;
            destination += extent.width;
        }
    }

    void setPixelsRaw(void* pixels, size_t numPixels) {
        memcpy(mapRaw<uint8_t>(), pixels, numPixels * bytesPerPixel);
    }

    template <typename PIXEL>
    void setPixels(std::span<const PIXEL> pixels) {
        assertm(bytesPerPixel == sizeof(PIXEL), "VulkImageBuffer::setPixels() called with wrong pixel type");
        memcpy(data<PIXEL>(), pixels.data(), pixels.size() * sizeof(PIXEL));
    }

    template <typename PIXEL>
    void setColor(VkRect2D rect, PIXEL pixel) {
        assertm(bytesPerPixel == sizeof(PIXEL), "VulkImageBuffer::setColor() called with wrong pixel type");
        auto destination = data<PIXEL>() + rect.offset.y * extent.width + rect.offset.x;
        for (int y = 0; y < rect.extent.height; y++) {
            for (int x = 0; x < rect.extent.width; x++) {
                *destination = pixel;
                destination++;
            }
            destination += extent.width - rect.extent.width;
        }
    }

    void initialize(VulkDevice& device, VkExtent3D extent, VkFormat format);
};



class VulkImage : public VulkHandle<VkImage>
{
    VulkImageBuffer stagingBuffer;
    VulkMemory textureImageMemory;
    VulkImageView view;
    VkExtent3D extent{};
    VkFormat format{VK_FORMAT_UNDEFINED};
public:
    VulkImage() {}
    VulkImage(VulkCommandPool &commandPool, std::string filename, bool retainBuffer = false);
    VulkImage(VulkCommandPool& commandPool, int width, int height, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, bool retainBuffer = true);
    VulkImage(VulkImage& other) = delete;
    VulkImage& operator=(VulkImage& other) = delete;

    VulkImage(VulkImage&& other)
        : VulkHandle<VkImage>(std::move(other)),
        stagingBuffer(std::move(other.stagingBuffer)),
        textureImageMemory(std::move(other.textureImageMemory)),
        view(std::move(other.view)),
        extent(other.extent),
        format(other.format)
    {
    }

    VulkImage& operator=(VulkImage&& other) {
        VulkHandle<VkImage>::operator=(std::move(other));
        stagingBuffer = std::move(other.stagingBuffer);
        textureImageMemory = std::move(other.textureImageMemory);
        view = std::move(other.view);
        extent = other.extent;
        format = other.format;
        return *this;
    }

    void load(VulkCommandPool &commandPool, std::string filename, bool retainBuffer);
    void create(VulkCommandPool &commandPool, int width, int height, VkFormat format, bool retainBuffer);
    void createDepthBuffer(VulkDevice &device, int width, int height);


    template <typename PIXEL>
    PIXEL* data() { return stagingBuffer.data<PIXEL>(); }

    template <typename PIXEL>
    void setPixels(VulkCommandPool &commandPool, VkRect2D rect, std::span<const PIXEL> pixels);

    template <typename PIXEL>
    void updatePixels(VulkCommandPool &commandPool, VkRect2D rect, std::span<const PIXEL> fullSizePixels);

    template <typename PIXEL>
    void setPixels(VulkCommandPool &commandPool,  std::span<const PIXEL> pixels);

    const VulkImageView& imageView() const { return view; }

    uint32_t width() const { return extent.width; }
    uint32_t height() const { return extent.height; }
    uint32_t depth() const { return extent.depth; }
    const VkExtent3D& getExtent() const { return extent; }
    VkFormat getFormat() const { return format; }
protected:
    void initializeImage(VulkDevice &device, int width, int height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void loadBufferIntoImage(VulkCommandPool& commandPool, VulkImageBuffer &buffer);
};

template<typename PIXEL>
void VulkImage::updatePixels(VulkCommandPool &commandPool, VkRect2D rect, std::span<const PIXEL> fullSizePixels)
{
    if (!stagingBuffer.isHandleValid()) {
        throw std::runtime_error("staging buffer not initialized");
    }

    stagingBuffer.updatePixels(rect, fullSizePixels);

    transitionImageLayout(commandPool, handle, format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(commandPool, stagingBuffer, handle, width(), height());
    transitionImageLayout(commandPool, handle, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}

template<typename PIXEL>
void VulkImage::setPixels(VulkCommandPool &commandPool,  VkRect2D rect, std::span<const PIXEL> pixels)
{
    if (!stagingBuffer.isHandleValid()) {
        throw std::runtime_error("staging buffer not initialized");
    }

    stagingBuffer.setPixels(rect, pixels);

    transitionImageLayout(commandPool, handle, format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(commandPool, stagingBuffer, handle, width(), height());
    transitionImageLayout(commandPool, handle, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

template <typename PIXEL>
void VulkImage::setPixels(VulkCommandPool &commandPool,  std::span<const PIXEL> pixels)
{
    if (!stagingBuffer.isHandleValid()) {
        throw std::runtime_error("staging buffer not initialized");
    }

    stagingBuffer.setPixels(pixels);

    transitionImageLayout(commandPool, handle, format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(commandPool, stagingBuffer, handle, width(), height());
    transitionImageLayout(commandPool, handle, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

class VulkSampler : public VulkHandle<VkSampler> {
public:
    VulkSampler() {}
    VulkSampler(VulkDevice &device);
    VulkSampler(VulkSampler& other) = delete;
    VulkSampler& operator=(VulkSampler& other) = delete;
    VulkSampler(VulkSampler&& other)
        : VulkHandle<VkSampler>(std::move(other))
    {
    }
    VulkSampler& operator=(VulkSampler&& other) {
        VulkHandle<VkSampler>::operator=(std::move(other));
        return *this;
    }
};

VulkImageBuffer loadImageIntoBuffer(VulkDevice& device, std::string filename);

#endif // VULKIMAGE_H
