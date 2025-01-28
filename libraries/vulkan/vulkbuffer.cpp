
#include "vulkbuffer.h"

VulkRawBuffer::VulkRawBuffer(VulkDevice &device,
                             VkDeviceSize sizeInBytes,
                             VkBufferUsageFlags usage,
                             VkMemoryPropertyFlags properties)
{
    initializeRaw(device, sizeInBytes, usage, properties);
}

void VulkRawBuffer::initializeRaw(VulkDevice &device,
                                  VkDeviceSize sizeInBytes,
                                  VkBufferUsageFlags usage,
                                  VkMemoryPropertyFlags properties)
{
    this->initDeviceHandle(device);

    try {

        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeInBytes;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VKCALLD(vkCreateBuffer, &bufferInfo, nullptr, &this->handle);

        memory.initMemory(device, this->handle, properties);

        VKCALLD(vkBindBufferMemory, this->handle, memory, 0);
    } catch (...) {
        this->initDeviceHandle(device);
        throw;
    }
}

void VulkRawBuffer::resizeRaw(VkDeviceSize newSizeBytes, VkDeviceSize preserveSizeBytes)
{
    bufferInfo.size = newSizeBytes;

    VkBuffer newHandle;

    VKCALLD(vkCreateBuffer, &bufferInfo, nullptr, &newHandle);

    VkMemoryRequirements memRequirements;

    VKCALLD_void(vkGetBufferMemoryRequirements, newHandle, &memRequirements);

    setHandle(newHandle);

    memory.resize(memRequirements.size, preserveSizeBytes);

    VKCALLD(vkBindBufferMemory, handle, memory, 0);
}
