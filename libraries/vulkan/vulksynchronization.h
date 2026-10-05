#ifndef VULKSYNCHRONIZATION_H
#define VULKSYNCHRONIZATION_H

#include "vulkcommandbuffers.h"
#include "vulkdevice.h"
#include <memory>

class VulkFlightControl
{
    VulkDevice& device;
    VkSemaphore available{};
    VkFence inFlight{};
public:
    VulkFlightControl(VulkDevice &device);
    ~VulkFlightControl();
    void waitForFence();
    void resetFence();
    VkFence inFlightFence() { return inFlight; }
    VkSemaphore availableSemaphore() { return available; }
};

class VulkFramebufferSynchronization
{
    VulkDevice* device{};
    std::vector<std::unique_ptr<VulkFlightControl>> flightControls; // size = maxFramesInFlight
    // one per swapchain image: a semaphore waited on by vkQueuePresentKHR can't safely be
    // reused until that same image is re-acquired (VUID-vkQueueSubmit-pSignalSemaphores-00067)
    std::vector<VkSemaphore> renderFinished; // size = swapchain image count
    size_t currentFlight = 0;
    std::unique_ptr<VulkCommandBuffers> commandBuffers;
    void destroyImageSemaphores();
public:
    VulkFramebufferSynchronization() {}
    ~VulkFramebufferSynchronization();
    void setup(VulkCommandPool &commandPool, int maxFramesInFlight);
    void setupImageSemaphores(size_t swapchainImageCount); // call after (re)creating the swapchain
    void advanceFrameInFlight();
    void waitForFrameInFlight();
    void resetFrameInFlightFence();
    VkSemaphore imageAvailableSemaphore() { return flightControls[currentFlight]->availableSemaphore(); }
    VkSemaphore renderFinishedSemaphore(uint32_t imageIndex) { return renderFinished[imageIndex]; }
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
