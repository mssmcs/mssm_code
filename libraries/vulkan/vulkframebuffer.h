#ifndef VULKFRAMEBUFFER_H
#define VULKFRAMEBUFFER_H

#include "vulkdevice.h"
#include "vulkutil.h"
#include "vulkimageview.h"

class VulkFrameBuffer : public VulkHandle<VkFramebuffer>
{
    std::vector<VulkImageView> views; // TODO: think about this. I'm using it purely for resource management
public:
    VulkFrameBuffer(VulkDevice& device, VkFramebuffer fb, std::vector<VulkImageView> views);
   ~VulkFrameBuffer();
   static std::vector<std::unique_ptr<VulkFrameBuffer>> createOneToOneFromViews(VulkDevice& device, VkRenderPass renderPass, VkExtent2D extent, std::vector<VulkImageView> views, VulkImageView depthBufferView = {});
private:
   static std::unique_ptr<VulkFrameBuffer> createFramebuffer(VulkDevice& device, VkRenderPass renderPass,  std::vector<VulkImageView> views, VkExtent2D extent);
};

#endif // VULKFRAMEBUFFER_H
