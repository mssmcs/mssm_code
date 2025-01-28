#include "vulkframebuffer.h"
#include <stdexcept>

VulkFrameBuffer::VulkFrameBuffer(VulkDevice& device, VkFramebuffer fb, std::vector<VulkImageView> views)
    : VulkHandle(device, fb), views{views}
{}

VulkFrameBuffer::~VulkFrameBuffer()
{
}

std::unique_ptr<VulkFrameBuffer> VulkFrameBuffer::createFramebuffer(VulkDevice& device,
                                                                    VkRenderPass renderPass,
                                                                    std::vector<VulkImageView> views,
                                                                    VkExtent2D extent)
{
    std::vector<VkImageView> attachments;

    for (auto &v : views) {
        attachments.push_back(v);
    }

    // if (views.size() != renderPass.attachmentCount()) {
    //     throw std::runtime_error(
    //         "failed to create framebuffer: num views must match renderpass attachment count");
    // }

    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = renderPass;
    framebuffer_info.attachmentCount = attachments.size(); // From Spec: attachmentCount must be equal to the attachment count specified in renderPass
    framebuffer_info.pAttachments = attachments.data();
    framebuffer_info.width = extent.width;
    framebuffer_info.height = extent.height;
    framebuffer_info.layers = 1;

    VkFramebuffer fb;

    if (device.fn.vkCreateFramebuffer(device, &framebuffer_info, nullptr, &fb) != VK_SUCCESS) {
        throw std::runtime_error("failed to create framebuffer");
    }

    return make_unique<VulkFrameBuffer>(device, fb, views);
}

std::vector<std::unique_ptr<VulkFrameBuffer>> VulkFrameBuffer::createOneToOneFromViews(VulkDevice &device,
                                                                                       VkRenderPass renderPass,
                                                                                       VkExtent2D extent,
                                                                                       std::vector<VulkImageView> views,
                                                                                       VulkImageView depthBufferView)
{
    std::vector<std::unique_ptr<VulkFrameBuffer>> framebuffers;

    // setting up a one-to-one correspondence between views and framebuffers
    // framebuffers can have multiple views, but we are not using that feature here

    for (size_t i = 0; i < views.size(); i++) {
        std::vector<VulkImageView> fbviews{ views[i] }; // <- single view per framebuffer
        if (depthBufferView.hasView()) {
            fbviews.push_back(depthBufferView);
        }
        framebuffers.push_back(createFramebuffer(device,renderPass,fbviews,extent));
    }

    return framebuffers;
}
