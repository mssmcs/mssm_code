#ifndef VULKPIPELINE_H
#define VULKPIPELINE_H

#include "vulkcommandbuffers.h"
#include "vulkdescriptorset.h"
#include "vulkdevice.h"
#include "vulkshaders.h"

class VulkPipeline : public VulkHandle<VkPipeline>
{
    VulkPipelineLayout* layoutPtr{};
public:
    VulkPipeline(VulkDevice& device, VkExtent2D extent, VkRenderPass renderPass, VulkShaders &shaders, VulkPipelineLayout& layout, VkPipelineVertexInputStateCreateInfo *vertexInfo, VkPrimitiveTopology topology, bool is3d);
    VulkPipelineLayout& layout() { return *layoutPtr; }
    VulkPipeline(const VulkPipeline& other) = delete;
    VulkPipeline& operator= (const VulkPipeline& other) = delete;

    template <typename TPushConstant>
    void sendPushConstants(VulkCommandBuffer& commandBuffer, TPushConstant& pushConstants)
    {
        vkCmdPushConstants(commandBuffer, *layoutPtr, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(TPushConstant), &pushConstants);
    }

};
#endif // VULKPIPELINE_H
