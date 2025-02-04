#ifndef VULKABSTRACTWINDOW_H
#define VULKABSTRACTWINDOW_H

#define VK_USE_64_BIT_PTR_DEFINES 1
#include "volk.h"

class VulkAbstractWindow
{
public:
    VulkAbstractWindow();
    virtual VkExtent2D getExtent() = 0;
    virtual VkSurfaceKHR createSurface(VkInstance instance) = 0;
};

#endif // VULKABSTRACTWINDOW_H
