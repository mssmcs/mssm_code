#ifndef VULKCOMMANDBUFFERS_H
#define VULKCOMMANDBUFFERS_H

#include "vulkbuffer.h"
#include "vulkdevice.h"
#include <functional>

class VulkCommandPool : public VulkHandle<VkCommandPool>
{
    VkQueue queue;
public:
    VulkCommandPool() {}
    VulkCommandPool(VulkDevice &device);
    VulkCommandPool(const VulkCommandPool&) = delete;
    VulkCommandPool& operator=(const VulkCommandPool&) = delete;
    VulkCommandPool(VulkCommandPool&& other) : VulkHandle<VkCommandPool>{std::move(other)}, queue{other.queue} {}
    VulkCommandPool& operator=(VulkCommandPool&& other) {
        VulkHandle<VkCommandPool>::operator=(std::move(other));
        queue = other.queue;
        return *this;
    }

    void initialize(VulkDevice &device);

    VkQueue getQueue() { return queue; }
};

class VulkCommandBuffer : public VulkHasDev
{
    VulkCommandPool* commandPool{};
    VkCommandBuffer cmdBuffer{};
    bool hasBegun{false};
public:
    bool submitted{false};
public:
    VulkCommandBuffer() {}
    VulkCommandBuffer(VulkCommandPool& commandPool);
    ~VulkCommandBuffer();
    VulkCommandBuffer(const VulkCommandBuffer&) = delete;
    VulkCommandBuffer& operator=(const VulkCommandBuffer&) = delete;
    VulkCommandBuffer(VulkCommandBuffer&& other) : commandPool{other.commandPool}, cmdBuffer{other.cmdBuffer} {
        other.cmdBuffer = nullptr;
    }
    VulkCommandBuffer& operator= (VulkCommandBuffer&& other) {
        commandPool = other.commandPool;
        cmdBuffer = other.cmdBuffer;
        other.commandPool = nullptr;
        other.cmdBuffer = nullptr;
        return *this;
    }

    bool getHasBegun() const { return hasBegun; }

    void initialize(VulkCommandPool& commandPool);
    void initialize(VulkCommandPool& commandPool, VkCommandBuffer buffer);

    operator VkCommandBuffer() { return cmdBuffer; }
    VkCommandBuffer* ptr() { return &cmdBuffer; }

    void begin(VkCommandBufferUsageFlags flags);
    void end();
    void submit(bool waitIdle);

    template <typename T>
    void bindIndexBuffer(VulkBuffer<T>& buffer, VkDeviceSize offset);

    void bindIndexBuffer(VulkRawBuffer& buffer, VkDeviceSize offset, VkIndexType indexType);
    void bindVertexBuffer(VulkRawBuffer& buffer, uint32_t bindingIndex, VkDeviceSize offset);
    void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
    void copyBuffer(VulkRawBuffer& srcBuffer, VulkRawBuffer& dstBuffer, VkDeviceSize size);
    void oneTimeCommand(std::function<void (VulkDevice &, VulkCommandBuffer &)> func);
    void setScissor(VkRect2D scissor);
    void setViewport(VkRect2D viewport);

    static void oneTimeCommmand(VulkCommandPool& commandPool, std::function<void (VulkDevice&, VulkCommandBuffer&)> func);

    VkQueue getQueue() { return commandPool->getQueue(); }
};

template<typename T>
inline void VulkCommandBuffer::bindIndexBuffer(VulkBuffer<T> &buffer, VkDeviceSize offset)
{
    VkIndexType indexType;
    if (sizeof(T) == sizeof(uint16_t)) {
        indexType = VK_INDEX_TYPE_UINT16;
    } else if (sizeof(T) == sizeof(uint32_t)) {
        indexType = VK_INDEX_TYPE_UINT32;
    }
    else {
        throw std::runtime_error("Invalid index buffer element size");
    }
    bindIndexBuffer(buffer, offset, indexType);
}

class VulkCommandBuffers : public VulkHasDev
{
    VulkCommandPool& commandPool;
    std::vector<VulkCommandBuffer> buffers;
public:
    VulkCommandBuffers(VulkCommandPool& commandPool, int count);
    ~VulkCommandBuffers();
    VulkCommandBuffers(const VulkCommandBuffers&) = delete;
    VulkCommandBuffers& operator=(const VulkCommandBuffers&) = delete;

    VulkCommandBuffer& operator[](int index) { return buffers[index]; }
    VkCommandBuffer* ptr(int index) { return buffers[index].ptr(); }

    int size() const { return buffers.size(); }
    bool isEmpty() const { return buffers.empty(); }
};

class VulkCommandSequence
{
    VulkCommandPool& commandPool;
    VulkCommandBuffer commandBuffer;
public:
    VulkCommandSequence(VulkCommandPool& commandPool);
    void add(std::function<void (VulkDevice&, VulkCommandBuffer&)> func);
    VulkCommandBuffer& buffer() { return commandBuffer; }
    void execute(bool waitIdle);
};


#endif // VULKCOMMANDBUFFERS_H
