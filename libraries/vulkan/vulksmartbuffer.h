#ifndef VULKSMARTBUFFER_H
#define VULKSMARTBUFFER_H

#include "vulkbufferpool.h"
#include "vulkcommandbuffers.h"
#include "vulksynchronization.h"
#include <span>

class VulkFramebufferSynchronization;

class VulkSmartBufferBase
{
    VulkFramebufferSynchronization& sync;
public:
    VulkSmartBufferBase(VulkFramebufferSynchronization& sync);
    virtual ~VulkSmartBufferBase();
    int frameInFlight();
    virtual void prepare() = 0;
    virtual void submit() = 0;
    virtual VkBuffer bufferToBind() = 0;
    virtual size_t elementSize() const = 0;
    virtual VulkRawBuffer* getRawBuffer() = 0;
    inline VulkCommandBuffer& activeCommandBuffer() const { return sync.activeCommandBuffer(); }
};


template <typename T>
class VulkSmartBuffer : public VulkSmartBufferBase
{
    VulkBufferPool* bufferPool{};
    std::vector<VulkBuffer<T>> buffers;
    std::vector<VulkBuffer<T>> toReturnToPool;
    size_t writeIdx{};
    std::span<T> mappedSpan;
public:
    VulkSmartBuffer() {}
    VulkSmartBuffer(VulkBufferPool& bufferPool, VulkFramebufferSynchronization& sync, VulkDevice& device, VkDeviceSize vertexCount, int bufferCount, VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VulkSmartBuffer(VulkSmartBuffer& other) = delete;
    VulkSmartBuffer& operator=(VulkSmartBuffer& other) = delete;

    VulkBuffer<T>& buffer() { return buffers[frameInFlight()]; }

     VulkRawBuffer* getRawBuffer() override {
        return &buffers[frameInFlight()];
     }

    void prepare() override;

    void submit() override;
    T* mapped() { return buffers[frameInFlight()].map(); }
    VkDeviceSize size() { return buffers[frameInFlight()].size(); }
    virtual VkBuffer bufferToBind() override { return buffers[frameInFlight()]; }
    void copyFromVector(const std::vector<T> &vertices);

    std::span<T> &data() {
        return mappedSpan;
    }

    void ensureSpace(VkDeviceSize count) {

        if (writeIdx + count >= mappedSpan.size()) {
            VulkBuffer<T>& oldBuffer = buffer();
            auto lastBindingIndex = oldBuffer.getLastBindingIndex();
            VulkBuffer<T> newBufferTmp = bufferPool->getTypedBuffer<T>((writeIdx + count)*2, oldBuffer.bufferUsage(), oldBuffer.memoryProperties());
            toReturnToPool.push_back(std::move(oldBuffer));
            VulkBuffer<T>& newBuffer = buffers[frameInFlight()] = std::move(newBufferTmp);
            mappedSpan = newBuffer.mappedSpan();
            writeIdx = 0;
            switch (newBuffer.bufferType()) {
            case VulkBufferType::index:
                activeCommandBuffer().bindIndexBuffer(newBuffer, 0);
                break;
            case VulkBufferType::vertex:
                activeCommandBuffer().bindVertexBuffer(newBuffer, lastBindingIndex, 0);
                break;
            default:
                throw std::runtime_error("unexpected buffer type");
            }
        }
    }

    inline bool hasCapacity(size_t count) const {
        return writeIdx + count <= mappedSpan.size();
    }

    // T& pushElement(const T& element) {
    //     assertm(hasCapacity(1), "VulkSmartBuffer::pushElement out of space");
    //     return *(mappedSpan.data() + writeIdx++) = element;
    // }

    size_t elementSize() const override { return sizeof(T);}

    T& elementAt(size_t idx)
    {
        return *(mappedSpan.data() + idx);
    }

    template <typename... Args>
    inline uint32_t push(Args&&... args) // variadic template function with perfect fowarding
    {
        assertm(hasCapacity(1), "VulkSmartBuffer::pushElementI out of space");
        *(mappedSpan.data() + writeIdx) = T{std::forward<Args>(args)...};
        return writeIdx++;
    }

    // inline size_t pushElementI(const T& element) {
    //     assertm(hasCapacity(1), "VulkSmartBuffer::pushElementI out of space");
    //     *(mappedSpan.data() + writeIdx) = element;
    //     return writeIdx++;
    // }

    void copyElements(T* ptr, size_t count) {
        assertm(hasCapacity(count), "VulkSmartBuffer::copyElements out of space");
        memcpy(mappedSpan.data()+writeIdx, ptr, sizeof(T) * count);
        writeIdx += count;
    }

    int writtenCount() const { return writeIdx; }
    uint32_t nextVertIdx() const { return writeIdx; }
};

// creates an array of buffers, all sharing the same memory
// NOTE: memory is assumed to be (and must be) uninitialized
template<typename T>
std::vector<VulkBuffer<T>> createBufferArray(VulkBufferPool* bufferPool, VulkDevice& device, int bufferCount, VkDeviceSize vertexCount, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    std::vector<VulkBuffer<T>> buffers;

    // , properties  do something about this
    for (int i = 0; i < bufferCount; i++) {
        buffers.push_back(VulkBuffer<T>(device, vertexCount, usage, properties));
    }

    return buffers;
}

template<typename T>
VulkSmartBuffer<T>::VulkSmartBuffer(VulkBufferPool& bufferPoolRef, VulkFramebufferSynchronization& sync, VulkDevice& device, VkDeviceSize vertexCount, int bufferCount, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    : VulkSmartBufferBase(sync), bufferPool{&bufferPoolRef}
{
    buffers = createBufferArray<T>(bufferPool, device, bufferCount, vertexCount, usage, properties);
}

template<typename T>
inline void VulkSmartBuffer<T>::prepare()
{
    auto& buffer = buffers[frameInFlight()];
    mappedSpan = buffer.mappedSpan();
    writeIdx = 0;
    for (auto& b : toReturnToPool) {
        bufferPool->returnBuffer(std::move(b));
    }
    toReturnToPool.clear();
}

template<typename T>
void VulkSmartBuffer<T>::submit()
{
    // buffers[frameInFlight()].unmap();  // maybe not necessary?
}

template<typename T>
inline void VulkSmartBuffer<T>::copyFromVector(const std::vector<T> &vertices)
{
    return buffers[frameInFlight()].copyFromVector(vertices);
}




#endif // VULKSMARTBUFFER_H
