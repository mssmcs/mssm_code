#ifndef VULKDESCRIPTORSET_H
#define VULKDESCRIPTORSET_H

#include "vulkbuffer.h"
#include "vulkdevice.h"
#include <list>
#include <memory>

class VulkDescriptorSetManager;


// NOTE:  this can be a single descriptor set or one per frame in flight
// depending on the frequency of the layout
class VulkDescSet
{
    std::vector<VkDescriptorSet> descSets;  // size 1 or maxinflight depending on
        // frequency
public:
    VulkDescSet() {}
    VulkDescSet(uint32_t size) : descSets(size) {}

    VkDescriptorSet handle(int frameInFlight)
    {
        return descSets.size() == 1 ? descSets.front() : descSets[frameInFlight];
    }

    VkDescriptorSet handle()
    {
        assertm(descSets.size() == 1, "VulkDescSet::handle() on multi-frame descriptor set");
        return descSets.front();
    }

    friend class VulkDescriptorPool;
};

enum class DescriptorBindingFrequency {
    Once,     // allocate single descriptor set for all frames
    PerFrame, // allocate descripter set per frame in flight
};

class VulkDescSetLayout : public VulkHandle<VkDescriptorSetLayout> {
    VulkDescriptorSetManager* manager{};
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    DescriptorBindingFrequency frequency{DescriptorBindingFrequency::Once};
public:
    VulkDescSetLayout(VulkDescriptorSetManager* manager) : manager{manager} {}
    VulkDescSetLayout(const VulkDescSetLayout&) = delete;
    VulkDescSetLayout& operator=(const VulkDescSetLayout&) = delete;

    VulkDescSetLayout(VulkDescSetLayout&& other) :
        VulkHandle<VkDescriptorSetLayout>{std::move(other)},
        bindings{std::move(other.bindings)},
        frequency{other.frequency}
    {
    }

    VulkDescSetLayout& operator=(VulkDescSetLayout&& other)
    {
        VulkHandle<VkDescriptorSetLayout>::operator=(std::move(other));
        bindings = std::move(other.bindings);
        frequency = other.frequency;
        return *this;
    }

    DescriptorBindingFrequency getFrequency() const { return frequency; }

    void appendPoolSizes(std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxFramesInFlight) const;

    uint32_t numBindings() const { return bindings.size(); }

    VulkDescSet createDescSet();

private:
    VulkDescSetLayout& addTextureBinding(uint32_t arraySize);
    VulkDescSetLayout& addUniformBufferBinding();
    VulkDescSetLayout& build(VulkDevice& device, uint32_t firstBindingNumber, DescriptorBindingFrequency frequency);

    friend class VulkDescSetLayoutBuilder;
    friend class VulkDescriptorSetManager;
};

class VulkDescSetLayoutBuilder {
    VulkDescSetLayout& layout;
public:
    VulkDescSetLayoutBuilder(VulkDescSetLayout& layout) : layout{layout} {}
    VulkDescSetLayoutBuilder& addTextureBinding(uint32_t arraySize) { layout.addTextureBinding(arraySize); return *this; }
    VulkDescSetLayoutBuilder& addUniformBufferBinding() { layout.addUniformBufferBinding(); return *this; }
    VulkDescSetLayout* build(VulkDevice& device, VulkDescriptorSetManager& manager, uint32_t firstBindingNumber, DescriptorBindingFrequency frequency)
    {
        return &layout.build(device, firstBindingNumber, frequency);
    }
};



class VulkDescriptorPool : public VulkHandle<VkDescriptorPool>
{
public:
    VulkDescriptorPool() {}
    VulkDescriptorPool(VulkDevice& device) : VulkHandle<VkDescriptorPool>{device} {}

    VulkDescSet createDescripterSet(VulkDescSetLayout& layout, uint32_t maxFramesInFlight);

public:
    void initialize(std::list<VulkDescSetLayout>& layouts, uint32_t maxFramesInFlight);
};

class VulkDescUpdate {
    VkDescriptorSet  descriptorSet;
    uint32_t         binding;
    VkDescriptorType descriptorType;
    uint32_t         descriptorCount;
    //uint32_t         arrayElement;
    // // VkShaderStageFlags    stageFlags;
    // // const VkSampler*      pImmutableSamplers;
public:
    VulkDescUpdate(VkDescriptorSet descriptorSet, uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount)
         : descriptorSet{descriptorSet}, binding{binding}, descriptorType{descriptorType}, descriptorCount{descriptorCount} {}
    virtual void populate(VkWriteDescriptorSet& ds);
};

class VulkDescUpdateBuffer : public VulkDescUpdate
{
    VkDescriptorBufferInfo bufferInfo{};
public:
    VulkDescUpdateBuffer(VkDescriptorSet descriptorSet, uint32_t binding, VkBuffer buffer, uint32_t descriptorCount)
        : VulkDescUpdate(descriptorSet, binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount)
    {
        bufferInfo.buffer = buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;
    }
    void populate(VkWriteDescriptorSet& ds) override;
};

class VulkDescUpdateImage : public VulkDescUpdate
{
    std::vector<VkDescriptorImageInfo> imageInfos;
public:
    VulkDescUpdateImage(VkDescriptorSet descriptorSet, uint32_t binding, std::vector<VkImageView> imageViews, VkSampler sampler, uint32_t descriptorCount)
        : VulkDescUpdate(descriptorSet, binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount)
    {
        imageInfos.resize(descriptorCount);
        for (int i = 0; i < descriptorCount; i++) {
            // if there are fewer image views than descriptors, repeat the last one
            int viewIdx = std::min(static_cast<int>(imageViews.size())-1, i);
            auto& imageInfo = imageInfos[i];
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = imageViews[viewIdx];
            imageInfo.sampler = sampler;
        }
    }
    void populate(VkWriteDescriptorSet& ds) override;
};

class VulkDescSetUpdates : public VulkHasDev {
    VulkDescSetLayout& layout;
    VkDescriptorSet descSet;
    std::vector<std::unique_ptr<VulkDescUpdate>> updates;
public:
    VulkDescSetUpdates(VulkDescSetLayout& layout, VkDescriptorSet descSet)
        : VulkHasDev{layout.device()}, layout{layout}, descSet{descSet} {}
    void addBufferUpdate(int binding, VkBuffer buffer, int descriptorCount = 1);
    void addImageUpdate(int binding, std::vector<VkImageView> imageViews, VkSampler sampler, int descriptorCount);
    void apply();
};

template <typename T>
class VulkUniformBuffer  {
public:
    std::vector<VulkBuffer<T>> buffers; // 1 per flight
    int updateCount{};
public:
    VulkUniformBuffer(VulkDevice& device, uint32_t numFlights);
    VulkBuffer<T>& buffer(int flightNumber) { return buffers[flightNumber]; }
    void update(int flightNumber, T& data) { buffers[flightNumber].copyFromValue(data); }
};

template<typename T>
VulkUniformBuffer<T>::VulkUniformBuffer(VulkDevice &device, uint32_t numFlights)
{
    for (int i = 0; i < numFlights; i++) {
        buffers.resize(buffers.size() + 1);
        buffers.back().initialize(device, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        buffers.back().map();
    }
}

class VulkPipelineLayout : public VulkHandle<VkPipelineLayout>
{
    std::vector<VulkDescSetLayout*> descriptorSetLayouts;
public:
    VulkPipelineLayout() {}
    void initialize(const std::initializer_list<VulkDescSetLayout*> layouts, uint32_t pushConstantSize = 0);

    template <typename TPushConstant>
    void initialize(const std::initializer_list<VulkDescSetLayout*> layouts) {
        initialize(layouts, sizeof(TPushConstant));
    }
};

class VulkPipelineDescriptorSets {
    VulkPipelineLayout* layout{};
    std::vector<VulkDescSet*> descSets;
    std::vector<VkDescriptorSet> descSetHandles;
public:
    VulkPipelineDescriptorSets() {}
    VulkPipelineDescriptorSets(VulkPipelineLayout& layout) : layout{&layout} {}
    void addDescSet(VulkDescSet& descSet) { descSets.push_back(&descSet); }
    size_t size() const { return descSets.size(); }
    VkDescriptorSet* data(uint32_t frameInFlight);
};

class VulkDescriptorSetManager {
public:
    std::list<VulkDescSetLayout> descSetLayouts;
    uint32_t maxFramesInFlight{};
private:
    VulkDescriptorPool descPool;
public:
    VulkDescriptorSetManager(uint32_t maxFramesInFlight) : maxFramesInFlight{maxFramesInFlight} {}
    VulkDescSet createDescSet(VulkDescSetLayout& layout);
    VulkDescSetLayoutBuilder addLayout();
};




#endif // VULKDESCRIPTORSET_H
