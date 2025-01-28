#include "vulkpipeline.h"
//#include "linmath.h"
//#include <stdexcept>

VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp)
{
    VkPipelineDepthStencilStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE;
    info.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE;
    info.depthCompareOp = bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
    info.depthBoundsTestEnable = VK_FALSE;
    info.minDepthBounds = 0.0f; // Optional
    info.maxDepthBounds = 1.0f; // Optional
    info.stencilTestEnable = VK_FALSE;

    return info;
}

class ViewportState
{
private:
    VkViewport viewport{};
    VkRect2D scissor{};
    VkPipelineViewportStateCreateInfo viewport_state{};

public:
    ViewportState(VkExtent2D extent);
    VkPipelineViewportStateCreateInfo *info() { return &viewport_state; }
};

ViewportState::ViewportState(VkExtent2D extent)
{
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) extent.width;
    viewport.height = (float) extent.height;
    viewport.minDepth = -1.0f;
    viewport.maxDepth = 1.0f;

    scissor.offset = {0, 0};
    scissor.extent = extent;

    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;
}

class Rasterizer
{
    VkPipelineRasterizationStateCreateInfo rasterizer{};

public:
    Rasterizer(bool cullBackFacing);
    VkPipelineRasterizationStateCreateInfo *info() { return &rasterizer; }
};

Rasterizer::Rasterizer(bool cullBackFacing)
{
    //cullBackFacing = false;//gjh
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = cullBackFacing ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
}

class Multisampling
{
    VkPipelineMultisampleStateCreateInfo multisampling{};

public:
    Multisampling();
    VkPipelineMultisampleStateCreateInfo *info() { return &multisampling; }
};

Multisampling::Multisampling()
{
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
}

class InputAssembly
{
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};

public:
    InputAssembly(VkPrimitiveTopology topology);
    VkPipelineInputAssemblyStateCreateInfo *info() { return &input_assembly; }
};

InputAssembly::InputAssembly(VkPrimitiveTopology topology)
{
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = topology;
    input_assembly.primitiveRestartEnable = VK_FALSE;
}

class ColorBlending
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo color_blending{};

public:
    ColorBlending();
    VkPipelineColorBlendStateCreateInfo *info() { return &color_blending; }
};

ColorBlending::ColorBlending()
{
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    // colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
    //                                       | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // colorBlendAttachment.blendEnable = VK_FALSE;

    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &colorBlendAttachment;
    color_blending.blendConstants[0] = 0.0f;
    color_blending.blendConstants[1] = 0.0f;
    color_blending.blendConstants[2] = 0.0f;
    color_blending.blendConstants[3] = 0.0f;
}

class DynamicStates
{
    std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                  VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic_info{};

public:
    DynamicStates();
    VkPipelineDynamicStateCreateInfo *info() { return &dynamic_info; }
};

DynamicStates::DynamicStates()
{
    dynamic_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamic_info.pDynamicStates = dynamic_states.data();
}


void VulkPipelineLayout::initialize(const std::initializer_list<VulkDescSetLayout*> layouts, uint32_t pushConstantSize)
{
    assertm(layouts.size() > 0, "No descriptor set layouts provided");

    initDeviceHandle((*begin(layouts))->device());

    std::vector<VkDescriptorSetLayout> handles;

    for (auto &layout : layouts) {
        descriptorSetLayouts.push_back(layout);
        handles.push_back(*layout);
    }

    VkPushConstantRange pushConstant{};
    pushConstant.offset = 0;
    pushConstant.size = pushConstantSize;
    pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pushConstantRangeCount = pushConstantSize > 0 ? 1 : 0;
    pipeline_layout_info.pPushConstantRanges = pushConstantSize > 0 ? &pushConstant : nullptr;
    pipeline_layout_info.setLayoutCount = handles.size();
    pipeline_layout_info.pSetLayouts = handles.data();

    VkPipelineLayout layoutHandle;

    VKCALLD(vkCreatePipelineLayout, &pipeline_layout_info, nullptr, &layoutHandle);

    setHandle(layoutHandle);
}


VulkPipeline::VulkPipeline(VulkDevice& device, VkExtent2D extent, VkRenderPass renderPass, VulkShaders& shaders, VulkPipelineLayout &layout, VkPipelineVertexInputStateCreateInfo* vertexInfo, VkPrimitiveTopology topology, bool is3d)
    : VulkHandle<VkPipeline>(device)
{
    layoutPtr = &layout;

    ViewportState viewport_state(extent);
    Rasterizer    rasterizer(is3d);
    Multisampling multisampling;
    ColorBlending colorBlending;
    DynamicStates dynamicStates;

    VkPipelineDepthStencilStateCreateInfo depthStencilState;

    InputAssembly inputAssembly(topology);

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = shaders.numStages();
    pipeline_info.pStages = shaders.pStages();
    pipeline_info.pVertexInputState = vertexInfo;
    pipeline_info.pInputAssemblyState = inputAssembly.info();
    pipeline_info.pViewportState = viewport_state.info();
    pipeline_info.pRasterizationState = rasterizer.info();
    pipeline_info.pMultisampleState = multisampling.info();
    pipeline_info.pColorBlendState = colorBlending.info();
    pipeline_info.pDynamicState = dynamicStates.info();
    pipeline_info.layout = layout;
    pipeline_info.renderPass = renderPass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    if (is3d) {
        depthStencilState = depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS);
        pipeline_info.pDepthStencilState = &depthStencilState;
    }
    else {
        depthStencilState = depth_stencil_create_info(false, false, VK_COMPARE_OP_ALWAYS);
        pipeline_info.pDepthStencilState = &depthStencilState;
    }

    VKCALLD(vkCreateGraphicsPipelines, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &handle);
}



