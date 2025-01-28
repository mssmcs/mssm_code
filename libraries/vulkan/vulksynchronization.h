#ifndef VULKSYNCHRONIZATION_H
#define VULKSYNCHRONIZATION_H

#include "vulkcommandbuffers.h"
#include "vulkdevice.h"
#include <memory>

class VulkFlightControl
{
    VulkDevice& device;
    VkSemaphore available{};
    VkSemaphore finished{};
    VkFence inFlight{};
public:
    VulkFlightControl(VulkDevice &device);
    ~VulkFlightControl();
    void waitForFence();
    void resetFence();
    VkFence inFlightFence() { return inFlight; }
    VkSemaphore availableSemaphore() { return available; }
    VkSemaphore finishedSemaphore() { return finished; }
};

class VulkFramebufferSynchronization
{
    std::vector<std::unique_ptr<VulkFlightControl>> flightControls; // size = maxFramesInFlight
    size_t currentFlight = 0;
    std::unique_ptr<VulkCommandBuffers> commandBuffers;
public:
    VulkFramebufferSynchronization() {}
    void setup(VulkCommandPool &commandPool, int maxFramesInFlight);
    void advanceFrameInFlight();
    void waitForFrameInFlight();
    VkSemaphore imageAvailableSemaphore() { return flightControls[currentFlight]->availableSemaphore(); }
    VkSemaphore renderFinishedSemaphore() { return flightControls[currentFlight]->finishedSemaphore(); }
    VkFence inFlightFence() { return flightControls[currentFlight]->inFlightFence(); }

    VulkCommandBuffer& activeCommandBuffer() const {
        // if (commandBuffers->isEmpty()) {
        //     return VK_NULL_HANDLE;
        // }
        return (*commandBuffers)[currentFlight];
    }

    VkCommandBuffer* activeCommandBufferPtr() const {
        // if (commandBuffers->isEmpty()) {
        //     return VK_NULL_HANDLE;
        // }
        return commandBuffers->ptr(currentFlight);
    }

    size_t getCurrentFlight() const { return currentFlight; }
};


#endif // VULKSYNCHRONIZATION_H
