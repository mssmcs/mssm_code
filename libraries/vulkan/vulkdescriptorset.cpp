#include "vulkdescriptorset.h"


VulkDescSet VulkDescriptorPool::createDescripterSet(VulkDescSetLayout& layout, uint32_t maxFramesInFlight)
{
    if (!isHandleValid()) {
        throw std::runtime_error("Descriptor pool not initialized");
    }

    uint32_t count = 1;

    switch (layout.getFrequency()) {
    case DescriptorBindingFrequency::Once:
        count = 1;
        break;
    case DescriptorBindingFrequency::PerFrame:
        count = maxFramesInFlight;
        break;
    }

    std::vector<VkDescriptorSetLayout> layouts(count, layout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = handle;
    allocInfo.descriptorSetCount = layouts.size();
    allocInfo.pSetLayouts = layouts.data();

    VulkDescSet descSet(count);

    VKCALLD(vkAllocateDescriptorSets, &allocInfo, descSet.descSets.data());

    return descSet;
}

void VulkDescriptorPool::initialize(std::list<VulkDescSetLayout> &layouts,
                                    uint32_t maxFramesInFlight)
{
    if (isHandleValid()) {
        throw std::runtime_error("Descriptor pool already initialized");
    }

    initDeviceHandle(layouts.front().device());

    std::vector<VkDescriptorPoolSize> poolSizes;

    for (const auto& layout : layouts) {
        layout.appendPoolSizes(poolSizes, maxFramesInFlight);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxFramesInFlight * layouts.size();

    VkDescriptorPool tDescPool;

    VKCALLD(vkCreateDescriptorPool, &poolInfo, nullptr, &tDescPool);

    setHandle(tDescPool);
}

VulkDescSetLayout& VulkDescSetLayout::addUniformBufferBinding()
{
    assertm(!isHandleValid(), "Descriptor set layout already built");
    VkDescriptorSetLayoutBinding binding{};
    binding.binding = bindings.size();
    binding.descriptorCount = 1;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    binding.pImmutableSamplers = nullptr;
    binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(binding);
    return *this;
}

void VulkDescSetLayout::appendPoolSizes(std::vector<VkDescriptorPoolSize> &poolSizes,
                                        uint32_t maxFramesInFlight) const
{
    for (auto binding : bindings) {
        poolSizes.push_back({binding.descriptorType, maxFramesInFlight * binding.descriptorCount});
    }
}

VulkDescSet VulkDescSetLayout::createDescSet()
{
    return manager->createDescSet(*this);
}

VulkDescSetLayout& VulkDescSetLayout::addTextureBinding(uint32_t arraySize)
{
    assertm(!isHandleValid(), "Descriptor set layout already built");
    VkDescriptorSetLayoutBinding binding{};
    binding.binding = bindings.size();
    binding.descriptorCount = arraySize;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.pImmutableSamplers = nullptr;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(binding);
    return *this;
}

VulkDescSetLayout &VulkDescSetLayout::build(VulkDevice& device, uint32_t firstBindingNumber, DescriptorBindingFrequency frequency)
{
    assertm(!isHandleValid(), "Descriptor set layout already built");

    for (auto& b : bindings) {
        b.binding += firstBindingNumber;
    }

    initDeviceHandle(device);

    this->frequency = frequency;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    VkDescriptorSetLayout handle{};

    VKCALLD(vkCreateDescriptorSetLayout, &layoutInfo, nullptr, &handle);

    setHandle(handle);

    return *this;
}


void VulkDescUpdate::populate(VkWriteDescriptorSet &ds)
{
    ds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    ds.descriptorCount = descriptorCount;
    ds.descriptorType = descriptorType;
    ds.dstBinding = binding;
    ds.dstSet = descriptorSet;
    ds.dstArrayElement = 0;
    // ds.pImmutableSamplers = pImmutableSamplers;
    // ds.stageFlags = stageFlags;
}

void VulkDescUpdateBuffer::populate(VkWriteDescriptorSet &ds)
{
    VulkDescUpdate::populate(ds);
    ds.pBufferInfo = &bufferInfo;
}

void VulkDescUpdateImage::populate(VkWriteDescriptorSet &ds)
{
    VulkDescUpdate::populate(ds);
    ds.pImageInfo = imageInfos.data();
}


void VulkDescSetUpdates::addBufferUpdate(int binding, VkBuffer buffer, int descriptorCount)
{
    updates.push_back(std::make_unique<VulkDescUpdateBuffer>(descSet, binding, buffer, descriptorCount));
}

void VulkDescSetUpdates::addImageUpdate(int binding,
                                        std::vector<VkImageView> imageViews,
                                        VkSampler sampler,
                                        int descriptorCount)
{
    if (imageViews.empty()){
        return;
    }
    updates.push_back(std::make_unique<VulkDescUpdateImage>(descSet, binding, imageViews, sampler, descriptorCount));
}


void VulkDescSetUpdates::apply()
{
    std::vector<VkWriteDescriptorSet> descriptorWrites(updates.size());
    for (int i = 0; i < updates.size(); i++) {
        updates[i]->populate(descriptorWrites[i]);
    }
    VKCALLD_void(vkUpdateDescriptorSets, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

VulkDescSet VulkDescriptorSetManager::createDescSet(VulkDescSetLayout& layout)
{
    if (!descPool.isHandleValid()) {
        descPool.initialize(descSetLayouts, maxFramesInFlight);
    }
    return descPool.createDescripterSet(layout, maxFramesInFlight);
}

VulkDescSetLayoutBuilder VulkDescriptorSetManager::addLayout()
{
    return VulkDescSetLayoutBuilder{descSetLayouts.emplace_back(this)};
}

VkDescriptorSet *VulkPipelineDescriptorSets::data(uint32_t frameInFlight)
{
    descSetHandles.resize(descSets.size());
    for (int i = 0; i < descSets.size(); i++) {
        descSetHandles[i] = descSets[i]->handle(frameInFlight);
    }
    return descSetHandles.data();
}
