#include "vulksynchronization.h"

VulkFlightControl::VulkFlightControl(VulkDevice& device)
    : device{device}
{
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (device.fn.vkCreateSemaphore(device, &semaphore_info, nullptr, &available) != VK_SUCCESS
        || device.fn.vkCreateSemaphore(device, &semaphore_info, nullptr, &finished) != VK_SUCCESS
        || device.fn.vkCreateFence(device, &fence_info, nullptr, &inFlight) != VK_SUCCESS) {
        throw std::runtime_error("failed to create synchronization objects for a frame");
    }
}

VulkFlightControl::~VulkFlightControl()
{
    device.fn.vkDestroySemaphore(device, available, nullptr);
    device.fn.vkDestroySemaphore(device, finished, nullptr);
    device.fn.vkDestroyFence(device, inFlight, nullptr);
}

void VulkFlightControl::waitForFence()
{
    device.fn.vkWaitForFences(device, 1, &inFlight, VK_TRUE, UINT64_MAX);
}

void VulkFlightControl::resetFence()
{
    device.fn.vkResetFences(device, 1, &inFlight);
}

void VulkFramebufferSynchronization::setup(VulkCommandPool& commandPool, int maxFramesInFlight)
{
    currentFlight = 0;

    flightControls.clear();

    for (int i = 0; i < maxFramesInFlight; i++) {
        flightControls.push_back(std::make_unique<VulkFlightControl>(commandPool.device()));
    }

    commandBuffers = std::make_unique<VulkCommandBuffers>(commandPool, maxFramesInFlight);
}

void VulkFramebufferSynchronization::advanceFrameInFlight()
{
    currentFlight = (currentFlight + 1) % flightControls.size();
}

void VulkFramebufferSynchronization::waitForFrameInFlight()
{
    auto &flightControl = flightControls[currentFlight];
    flightControl->waitForFence();
    flightControl->resetFence();
}

