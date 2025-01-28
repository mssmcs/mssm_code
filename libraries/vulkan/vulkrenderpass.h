#ifndef VULKRENDERPASS_H
#define VULKRENDERPASS_H

#include "vulkdevice.h"
#include <functional>

// renderpass is used to define a pipeline, but later
// a pipeline can use any renderpass that is "compatible" with the
// one used to create the pipeline
class VulkRenderPass : public VulkHandle<VkRenderPass>
{
    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkSubpassDescription> subpasses;
    std::vector<VkSubpassDependency> dependencies;
public:
    VulkRenderPass();
    VulkRenderPass(VulkRenderPass& other) = delete;
    VulkRenderPass& operator=(VulkRenderPass& other) = delete;
   ~VulkRenderPass();
    void cleanup();
    int addAttachment(std::function<void(VkAttachmentDescription &)> configFunc);
    int addSubpass(std::vector<int> attachments, std::function<void(VkSubpassDescription &)> configFunc);
    int addDependency(std::function<void(VkSubpassDependency &)> configFunc);
    void build(VulkDevice &device);
    int attachmentCount() const { return attachments.size(); }

    static void configureBasicRenderPass(VulkRenderPass &renderPass, VkFormat format, VkFormat depthFormat = VK_FORMAT_UNDEFINED);
private:
    void handleDepthBuffer(VkSubpassDescription &subpass);

};


#endif // VULKRENDERPASS_H
