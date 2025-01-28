#ifndef VULKBUFFERPOOL_H
#define VULKBUFFERPOOL_H

#include "vulkbuffer.h"

class VulkBufferPool : public VulkHasDev
{
    std::vector<VulkRawBuffer> buffers;
public:
    VulkBufferPool() = default;
    VulkBufferPool(VulkDevice& device);
    VulkBufferPool(VulkBufferPool& other) = delete;
    VulkBufferPool& operator=(VulkBufferPool& other) = delete;

    void initialize(VulkDevice& device);

    VulkRawBuffer getBuffer(VkDeviceSize sizeInBytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    template <typename T>
    VulkBuffer<T> getTypedBuffer(VkDeviceSize count, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    {
        VulkRawBuffer raw = getBuffer(sizeof(T) * count, usage, properties);
        return VulkBuffer<T>(std::move(raw));
    }

    void returnBuffer(VulkRawBuffer&& buffer);
    void clear();
};


#endif // VULKBUFFERPOOL_H
