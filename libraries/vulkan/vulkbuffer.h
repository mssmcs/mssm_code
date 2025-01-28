#ifndef VULKBUFFER_H
#define VULKBUFFER_H

#include "vulkmemory.h"


/*  TODO
   . Driver developers recommend that you also store multiple buffers, like the vertex and index buffer,
into a single VkBuffer and use offsets in commands like vkCmdBindVertexBuffers. The advantage is that
your data is more cache friendly in that case, because it’s closer together. It is even possible to
reuse the same chunk of memory for multiple resources if they are not used during the same render
operations, provided that their data is refreshed, of course.
This is known as aliasing and some Vulkan functions have explicit flags to
specify that you want to do this.
   */

enum class VulkBufferType {
    index,
    vertex,
    other
};

class VulkRawBuffer : public VulkHandle<VkBuffer>
{
protected:
    VulkMemory memory;
    VkBufferCreateInfo bufferInfo{};
    uint32_t lastBindingIndex{};
public:
    VulkRawBuffer() {}
    VulkRawBuffer(VulkDevice &device, VkDeviceSize sizeInBytes, VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    virtual ~VulkRawBuffer() {}
    VulkRawBuffer(VulkRawBuffer& other) = delete;
    VulkRawBuffer& operator=(VulkRawBuffer& other) = delete;

    VulkRawBuffer(VulkRawBuffer&& other) :
        VulkHandle<VkBuffer>(std::move(other)),
        memory(std::move(other.memory))
    {
        bufferInfo = other.bufferInfo;
        other.bufferInfo = {};
    }

    VulkRawBuffer& operator=(VulkRawBuffer&& other)
    {
        VulkHandle<VkBuffer>::operator=(std::move(other));
        memory = std::move(other.memory);
        bufferInfo = other.bufferInfo;
        other.bufferInfo = {};
        return *this;
    }

    void ensureMapped() { memory.map(); }

    template<typename T>
    inline T* mapRaw() { return static_cast<T*>(memory.map()); }

    template<typename T>
    std::span<T> mappedSpanRaw()  {
        return std::span<T>(static_cast<T*>(memory.map()), sizeBytes() / sizeof(T));
    }

    template<typename T>
    void copyFromRaw(std::span<T> source) { memory.copyFrom(source); }

    void initializeRaw(VulkDevice &device, VkDeviceSize sizeInBytes, VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // NOTE: invalidates both VkBuffer and VkDeviceMemory
    void resizeRaw(VkDeviceSize newSizeBytes, VkDeviceSize preserveSizeBytes);

    VkDeviceSize sizeBytes() const { return bufferInfo.size; }

    VkBufferUsageFlags bufferUsage() const { return bufferInfo.usage; }
    VkMemoryPropertyFlags memoryProperties() const { return memory.getProperties(); }

    bool isIndexBuffer() const { return bufferInfo.usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT; }
    bool isVertexBuffer() const { return bufferInfo.usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; }

    const VulkBufferType bufferType() const {
        if (isIndexBuffer()) return VulkBufferType::index;
        if (isVertexBuffer()) return VulkBufferType::vertex;
        return VulkBufferType::other;
    }

    void setLastBindingIndex(uint32_t bindingIndex) { lastBindingIndex = bindingIndex; }
    uint32_t getLastBindingIndex() { return lastBindingIndex; }
};


template<typename T>
class VulkBuffer : public VulkRawBuffer
{
private:
    size_t itemCount{};
public:
    VulkBuffer() {}
    VulkBuffer(VulkDevice &device, VkDeviceSize count, VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VulkBuffer(VulkBuffer& other) = delete;
    VulkBuffer& operator=(VulkBuffer& other) = delete;
    VulkBuffer(VulkBuffer&& other) : VulkRawBuffer(std::move(other))
    {
        itemCount = other.itemCount;
        other.itemCount = 0;
    }
    VulkBuffer& operator=(VulkBuffer&& other) {
        VulkRawBuffer::operator=(std::move(other));
        itemCount = other.itemCount;
        other.itemCount = 0;
        return *this;
    }

    VulkBuffer(VulkRawBuffer&& other) : VulkRawBuffer(std::move(other))
    {
        itemCount = sizeBytes() / sizeof(T);
    }

    VulkBuffer& operator=(VulkRawBuffer&& other) {
        VulkRawBuffer::operator=(std::move(other));
        itemCount = sizeBytes() / sizeof(T);
        return *this;
    }

    void initialize(VulkDevice &device, VkDeviceSize count, VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    void copyFrom(std::span<T> source) { memory.copyFrom(source); }
    void copyFromValue(T& value) { memory.copyFrom(std::span<T>(&value, 1)); }
    size_t count() const { return this->itemCount; }
    size_t sizeBytes() const { return memory.getMemSizeBytes(); }

    inline T* map() { return mapRaw<T>(); }

    std::span<T> mappedSpan()  { return mappedSpanRaw<T>(); }

    void resizeBuffer(VkDeviceSize newItemCount, VkDeviceSize preserveItemCount);
};

template<typename T>
VulkBuffer<T>::VulkBuffer(VulkDevice &device, VkDeviceSize count, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    initialize(device, count, usage, properties);
}

template<typename T>
void VulkBuffer<T>::initialize(VulkDevice &device,
                               VkDeviceSize count,
                               VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags properties)
{
    itemCount = count;
    initializeRaw(device, sizeof(T) * count, usage, properties);
}

template<typename T>
inline void VulkBuffer<T>::resizeBuffer(VkDeviceSize newItemCount, VkDeviceSize preserveItemCount)
{
    resizeRaw(newItemCount * sizeof(T), preserveItemCount * sizeof(T));
    this->itemCount = newItemCount;
}



#endif // VULKBUFFER_H
