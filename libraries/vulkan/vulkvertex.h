#ifndef VULKVERTEX_H
#define VULKVERTEX_H

#define VK_USE_64_BIT_PTR_DEFINES 1
#include "volk.h"
#include <vector>
#include <stdexcept>

template<typename T>
VkVertexInputRate vulkVertexRate()
{
    return VK_VERTEX_INPUT_RATE_VERTEX;
}

template<typename T>
void vulkVertexAttributes(std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
{
    throw std::runtime_error("specialize vulkVertexAttributes<T> for this type");
}

template <typename T1, typename T2>
static inline size_t constexpr offset_of(T1 T2::*member) {
    constexpr T2 object {};
    return size_t(&(object.*member)) - size_t(&object);
}

template<typename V>
static VkVertexInputBindingDescription getVulkBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(V);
    bindingDescription.inputRate = vulkVertexRate<V>();
    return bindingDescription;
}

void addAttribute(std::vector<VkVertexInputAttributeDescription> &attributeDescriptions, VkFormat format, size_t offset);


template<typename V>
static std::vector<VkVertexInputAttributeDescription> getVulkAttributeDescriptions()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    vulkVertexAttributes<V>(attributeDescriptions);

    return attributeDescriptions;
}


// template<class V>
// class VulkVertex
// {
// public:
//     template <typename T1, typename T2>
//     static inline size_t constexpr offset_of(T1 T2::*member) {
//         constexpr T2 object {};
//         return size_t(&(object.*member)) - size_t(&object);
//     }

//     static VkVertexInputBindingDescription getBindingDescription()
//     {
//         VkVertexInputBindingDescription bindingDescription{};
//         bindingDescription.binding = 0;
//         bindingDescription.stride = sizeof(V);
//         bindingDescription.inputRate = V::rate();
//         return bindingDescription;
//     }

//     static void addAttribute(std::vector<VkVertexInputAttributeDescription> &attributeDescriptions, VkFormat format, size_t offset)
//     {
//         VkVertexInputAttributeDescription attributeDescription{};
//         attributeDescription.binding = 0;
//         attributeDescription.location = attributeDescriptions.size();  // note: overwritten later
//         attributeDescription.format = format;
//         attributeDescription.offset = offset;
//         attributeDescriptions.push_back(attributeDescription);
//     }

//     static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
//     {
//         std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

//         V::attributes(attributeDescriptions);

//         return attributeDescriptions;
//     }
// };


class VertexInfo
{
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};

public:
    VertexInfo() {}

    VkPipelineVertexInputStateCreateInfo *info();

    template<typename T>
    void addBinding();
};


template<typename T>
void VertexInfo::addBinding()
{
    auto bindingNum = bindingDescriptions.size();

    bindingDescriptions.push_back(getVulkBindingDescription<T>());
    bindingDescriptions.back().binding = bindingNum;

    for (auto& aDesc : getVulkAttributeDescriptions<T>()) {
        aDesc.binding = bindingNum;
        aDesc.location = attributeDescriptions.size();
        attributeDescriptions.push_back(aDesc);
    }
}


#endif // VULKVERTEX_H
