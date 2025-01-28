#ifndef VULKIMAGEVIEW_H
#define VULKIMAGEVIEW_H

#include "vulkutil.h"
#include "vulkdevice.h"

class VulkImageView // : public VulkShared<VkDevice, VkImageView>
{
    std::shared_ptr<VulkHandle<VkImageView>> view{};
public:
    VulkImageView();
    VulkImageView(const VulkImageView& other)
        : view{other.view}
    {

    }
    VulkImageView(VulkDevice& device, VkImageView view);
    VulkImageView(VulkDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspectMask);
    operator VkImageView() const { return *view; }
    bool hasView() const { return view.get() != nullptr; }
};

std::shared_ptr<VulkHandle<VkImageView>> createImageView(VulkDevice& device, VkImage image, VkFormat format);

#endif // VULKIMAGEVIEW_H
