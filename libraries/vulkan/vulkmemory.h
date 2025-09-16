#ifndef VULKMEMORY_H
#define VULKMEMORY_H

#include "vulkdevice.h"

#include <iostream>
#include <ostream>
#include <span>
#include <cstring> // Added to declare memcpy

uint32_t findMemoryType(VulkDevice &device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

template <typename D>
class VulkMemoryBase : public VulkHasDevIndirect<D> {
protected:
    VkMemoryAllocateInfo allocInfo{};
    VkDeviceSize memSizeBytes{};
public:
    void resize(VkDeviceSize newSizeBytes, VkDeviceSize preserveSizeBytes);
};

template<typename D>
inline void VulkMemoryBase<D>::resize(VkDeviceSize newSizeBytes, VkDeviceSize preserveSizeBytes)
{
    D* derivedClass = static_cast<D*>(this);

    std::cout << "resizing to " << newSizeBytes << std::endl;

    auto writeIdxBackup = derivedClass->writeIdx;
    bool wasMapped = derivedClass->isMapped();

    VkDeviceSize oldSize = this->memSizeBytes;

    void* oldPtr = derivedClass->map();

    assertm(oldSize >= preserveSizeBytes, "Asked to preserve more memory than exists in old buffer");
    assertm(newSizeBytes >= preserveSizeBytes, "Asked to preserve more memory than exists in new buffer");

    this->allocInfo.allocationSize = newSizeBytes;

    VkDeviceMemory newHandle;

    VKCALL_D(vkAllocateMemory, &this->allocInfo, nullptr, &newHandle);

    void* newPtr;

    VKCALL_D(vkMapMemory, newHandle, 0, newSizeBytes, 0, &newPtr);

    memcpy(newPtr, oldPtr, preserveSizeBytes); // memcpy is now declared

    derivedClass->unmap(); // unmap old memory
    derivedClass->setHandle(newHandle);  // deletes old memory

    derivedClass->updateMapping(newPtr, writeIdxBackup);

    this->memSizeBytes = newSizeBytes;

    if (!wasMapped) {
        derivedClass->unmap();
    }
}

template <typename D>
class VulkMappable : public VulkMemoryBase<D> {
protected:
    size_t writeIdx{};
private:
    void* mappedPtr{};
public:
    void* map() {
        if (!mappedPtr) {
            D* derivedClass = static_cast<D*>(this);
            auto sz = derivedClass->getMemSizeBytes();
            if (sz == 0) {
                throw std::runtime_error("VulkBuffer::map() called on zero-sized buffer");
            }
            VKCALL_D(vkMapMemory, derivedClass->memHandle(), 0, derivedClass->getMemSizeBytes(), 0, &mappedPtr);
            writeIdx = 0;
        }
        return mappedPtr;
    }
    void unmap() {
        D* derivedClass = static_cast<D*>(this);
        assertm(mappedPtr != nullptr, "VulkBuffer::unmap() called when not mapped");
        VKCALL_D_void(vkUnmapMemory, derivedClass->memHandle());
        mappedPtr = nullptr;
    }
    bool isMapped() const { return mappedPtr != nullptr; }
    void* memPtr() const { return mappedPtr; }
protected:
    void updateMapping(void* newMappedPtr, size_t newWriteIdx) {
        mappedPtr = newMappedPtr;
        writeIdx = newWriteIdx;
    }
    friend class VulkMemoryBase<D>;
};

class VulkMemory : public VulkHandle<VkDeviceMemory>, public VulkMappable<VulkMemory>
{
private:
    VkMemoryPropertyFlags properties{};
public:
    VulkMemory() {}
    VulkMemory(VulkDevice &device, VkBuffer buffer, VkMemoryPropertyFlags properties);
    VulkMemory(VulkMemory& other) = delete;
    VulkMemory& operator=(VulkMemory& other) = delete;

    VulkMemory(VulkMemory&& other)
        : VulkHandle<VkDeviceMemory>(std::move(other)),
        VulkMappable<VulkMemory>(std::move(other)),
        properties(other.properties)
    {
    }

    VulkMemory& operator=(VulkMemory&& other) {
        VulkHandle<VkDeviceMemory>::operator=(std::move(other));
        VulkMappable<VulkMemory>::operator=(std::move(other));
        properties = other.properties;
        return *this;
    }

    void initMemory(VulkDevice &device, VkDeviceSize memSizeBytes, uint32_t memoryTypeFilter, VkMemoryPropertyFlags properties);
    void initMemory(VulkDevice &device, VkBuffer buffer, VkMemoryPropertyFlags properties);
    //  void resize(VkDeviceSize newSizeBytes, VkDeviceSize preserveSizeBytes);
    VkDeviceMemory memHandle() const { return handle; }
    VkDeviceSize getMemSizeBytes() const { return memSizeBytes; }

    template <typename T2>
    void copyFrom(std::span<T2> source);

    VkMemoryPropertyFlags getProperties() const { return properties; }
};


template <typename T2>
inline void VulkMemory::copyFrom(std::span<T2> source)
{
    auto sz = source.size_bytes();
    assertm(sz <= memSizeBytes, "VulkBuffer::copyFrom() called with source span larger than buffer size");
    if (sz == 0) {
        // nothing to copy
        return;
    }
    bool wasMapped = this->isMapped();
    if (!wasMapped) {
        this->map();
    }
    memcpy(this->memPtr(), source.data(), sz); // memcpy is now declared
    if (!wasMapped) {
        this->unmap();
    }
}


#endif // VULKMEMORY_H
