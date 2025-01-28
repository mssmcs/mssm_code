#include "vulkrenderpass.h"
#include <stdexcept>
#include <limits>

int VulkRenderPass::addAttachment(std::function<void(VkAttachmentDescription &)> configFunc)
{
    attachments.push_back({});
    configFunc(attachments.back());
    return attachments.size() - 1;
}

int VulkRenderPass::addSubpass(std::vector<int> colorAttachmentIndices,
                               std::function<void(VkSubpassDescription &)> configFunc)
{
    subpasses.push_back({});
    VkSubpassDescription &subpass = subpasses.back();
    subpass.colorAttachmentCount = colorAttachmentIndices.size();
    auto refs = new VkAttachmentReference[colorAttachmentIndices.size()];
    subpass.colorAttachmentCount = colorAttachmentIndices.size();
    subpass.pColorAttachments = refs; // TODO Cleanup!

    for (int i = 0; i < colorAttachmentIndices.size(); i++) {
        refs[i].attachment = colorAttachmentIndices[i];
        refs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    // TODO: probably need to think more about this, do all subpasses need depth buffer?
    handleDepthBuffer(subpass);


    configFunc(subpass);
    return subpasses.size() - 1;
}

int VulkRenderPass::addDependency(std::function<void(VkSubpassDependency &)> configFunc)
{
    dependencies.push_back({});
    configFunc(dependencies.back());
    return dependencies.size() - 1;
}

VulkRenderPass::~VulkRenderPass()
{
    cleanup();
}

void VulkRenderPass::cleanup()
{
    // delete attachment refs that were allocated on heap
    for (auto sub : subpasses) {
        delete[] sub.pColorAttachments;
        sub.pColorAttachments = nullptr;
        delete sub.pDepthStencilAttachment;
        sub.pDepthStencilAttachment = nullptr;
    }

    attachments.clear();
    subpasses.clear();
    dependencies.clear();
}

void VulkRenderPass::build(VulkDevice &device)
{
    initDeviceHandle(device);

    //  VkSubpassDependency dependency = {};

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = attachments.size();
    render_pass_info.pAttachments = attachments.empty() ? nullptr : attachments.data();
    render_pass_info.subpassCount = subpasses.size();
    render_pass_info.pSubpasses = subpasses.empty() ? nullptr : subpasses.data();
    render_pass_info.dependencyCount = dependencies.size();
    render_pass_info.pDependencies = dependencies.empty() ? nullptr : dependencies.data();

    // TODO: error handling (min counts for attachments, subpasses, dependencies?)

    if (fn()->vkCreateRenderPass(device, &render_pass_info, nullptr, &handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass");
    }

    cleanup();
}

VulkRenderPass::VulkRenderPass()
{
}

void VulkRenderPass::configureBasicRenderPass(VulkRenderPass &renderPass, VkFormat format, VkFormat depthFormat)
{
    bool addDepthBuffer = depthFormat != VK_FORMAT_UNDEFINED;

    //https://www.reddit.com/r/vulkan/comments/s80reu/subpass_dependencies_what_are_those_and_why_do_i/

    int img1 = renderPass.addAttachment([format](auto &a) {
        a.format = format;
        a.samples = VK_SAMPLE_COUNT_1_BIT;
        a.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        a.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        a.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        a.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        a.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        a.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    });

    if (addDepthBuffer) {
        renderPass.addAttachment([depthFormat](auto &a) {
            a.format = depthFormat;
            a.samples = VK_SAMPLE_COUNT_1_BIT;
            a.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            a.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            a.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            a.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            a.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            a.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        });
    }

    int sub1 = renderPass.addSubpass({img1}, [](auto &s) {
        s.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    });

    renderPass.addDependency([sub1](auto &dep) {
        dep.srcSubpass = VK_SUBPASS_EXTERNAL;
        dep.dstSubpass = sub1;
        dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dep.srcAccessMask = 0;
        dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; /* | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT*/;
    });

    if (addDepthBuffer) {
        renderPass.addDependency([sub1](auto &dep) {
            dep.srcSubpass = VK_SUBPASS_EXTERNAL;
            dep.dstSubpass = sub1;
            dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dep.srcAccessMask = 0;
            dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        });
    }
}

void VulkRenderPass::handleDepthBuffer(VkSubpassDescription &subpass)
{
    // if a depth buffer exists in our list of attachments,
    // then set it as the depth buffer for this subpass
    uint32_t depthAttachmentIndex = std::numeric_limits<uint32_t>::max();

    for (int i = 0; i < attachments.size(); i++) {
        if (attachments[i].finalLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            depthAttachmentIndex = i;
            break;
        }
    }

    if (depthAttachmentIndex != std::numeric_limits<uint32_t>::max()) {
        auto aref = new VkAttachmentReference{};
        aref->attachment = depthAttachmentIndex;
        aref->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        subpass.pDepthStencilAttachment = aref;
    }
}
