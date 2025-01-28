#ifndef VULKVERTEX_H
#define VULKVERTEX_H

#include "volk.h"
#include <vector>

template<class V>
class VulkVertex
{
public:
    template <typename T1, typename T2>
    static inline size_t constexpr offset_of(T1 T2::*member) {
        constexpr T2 object {};
        return size_t(&(object.*member)) - size_t(&object);
    }

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(V);
        bindingDescription.inputRate = V::rate();
        return bindingDescription;
    }

    static void addAttribute(std::vector<VkVertexInputAttributeDescription> &attributeDescriptions, VkFormat format, size_t offset)
    {
        VkVertexInputAttributeDescription attributeDescription{};
        attributeDescription.binding = 0;
        attributeDescription.location = attributeDescriptions.size();  // note: overwritten later
        attributeDescription.format = format;
        attributeDescription.offset = offset;
        attributeDescriptions.push_back(attributeDescription);
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        V::attributes(attributeDescriptions);

        return attributeDescriptions;
    }
};


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

    bindingDescriptions.push_back(T::getBindingDescription());
    bindingDescriptions.back().binding = bindingNum;

    for (auto& aDesc : T::getAttributeDescriptions()) {
        aDesc.binding = bindingNum;
        aDesc.location = attributeDescriptions.size();
        attributeDescriptions.push_back(aDesc);
    }
}


#endif // VULKVERTEX_H
