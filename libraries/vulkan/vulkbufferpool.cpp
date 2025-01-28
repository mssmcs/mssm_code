#include "vulkbufferpool.h"
#include <limits>

VulkBufferPool::VulkBufferPool(VulkDevice &device) : VulkHasDev(device)
{
}

void VulkBufferPool::initialize(VulkDevice &deviceRef)
{
    initDeviceHandle(deviceRef);
}

VulkRawBuffer VulkBufferPool::getBuffer(VkDeviceSize sizeInBytes,
                                        VkBufferUsageFlags usage,
                                        VkMemoryPropertyFlags properties)
{
    assertm(hasDeviceHandle(), "VulkBufferPool::getBuffer called before initialization");

    int bestIdx = -1;
    int bestSize = std::numeric_limits<int>::max();

    // try to find an existing buffer
    for (int i = 0; i < buffers.size(); i++) {
        VulkRawBuffer &buffer = buffers[i];
        if (buffer.sizeBytes() >= sizeInBytes && buffer.bufferUsage() == usage
            && buffer.memoryProperties() == properties && buffer.sizeBytes() < bestSize) {
            bestSize = buffer.sizeBytes();
            bestIdx = i;
        }
    }

    if (bestIdx >= 0) {
        VulkRawBuffer ret = std::move(buffers[bestIdx]);
        buffers.erase(buffers.begin() + bestIdx);
        assertm(ret.isHandleValid(), "VulkBufferPool::getBuffer found buffer with invalid handle");
        return ret;
    }

    return VulkRawBuffer(device(), sizeInBytes, usage, properties);
}

void VulkBufferPool::returnBuffer(VulkRawBuffer &&buffer)
{
    buffers.push_back(std::move(buffer));
}
