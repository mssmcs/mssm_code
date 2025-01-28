#include "vulkmemory.h"


uint32_t findMemoryType(VulkDevice& device,
                        uint32_t typeFilter,
                        VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i))
            && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

VulkMemory::VulkMemory(VulkDevice &device, VkBuffer buffer, VkMemoryPropertyFlags properties)
{
    initMemory(device, buffer, properties);
}


void VulkMemory::initMemory(VulkDevice &device,
                            VkDeviceSize memSizeBytes,
                            uint32_t memoryTypeFilter,
                            VkMemoryPropertyFlags properties)
{
    this->initDeviceHandle(device); // setting devPt

    this->properties = properties;
    this->memSizeBytes = memSizeBytes;

    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memSizeBytes;
    allocInfo.memoryTypeIndex = findMemoryType(device, memoryTypeFilter, properties);

    VKCALLD(vkAllocateMemory, &allocInfo, nullptr, &this->handle);
}

void VulkMemory::initMemory(VulkDevice &device, VkBuffer buffer, VkMemoryPropertyFlags properties)
{
    initDeviceHandle(device);
    VkMemoryRequirements memRequirements;
    VKCALLD_void(vkGetBufferMemoryRequirements, buffer, &memRequirements);
    initMemory(device, memRequirements.size, memRequirements.memoryTypeBits, properties);
}


