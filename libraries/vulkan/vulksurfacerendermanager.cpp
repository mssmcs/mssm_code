#include "vulksurfacerendermanager.h"
#include <array>
#include "stb_image_write.h"

VulkSurfaceRenderManager::VulkSurfaceRenderManager() {}

VulkSurfaceRenderManager::~VulkSurfaceRenderManager()
{
    if (device) {
        device->waitForIdle();
    }
}

void VulkSurfaceRenderManager::beginInitialization(VkInstance instance, VulkAbstractWindow *window, bool includeDepthBuffer, int maxFramesInFlight)
{
    this->window = window;
    this->maxFramesInFlight = maxFramesInFlight;
    hasDepthBuffer = includeDepthBuffer;

    VkExtent2D actualWindowExtent = window->getExtent();
    VkSurfaceKHR surfaceHandle = window->createSurface(instance);

    surface = std::make_unique<VulkSurface>(instance, surfaceHandle);


    device = std::make_unique<VulkDevice>(surface->getInstance(), *surface, std::vector<const char *>{VK_KHR_SWAPCHAIN_EXTENSION_NAME});
    swapChain = std::make_unique<VulkSwapChain>(*device, *surface, actualWindowExtent, includeDepthBuffer);
    graphicsCommandPool = std::make_unique<VulkCommandPool>(*device);
    bufferPool.initialize(*device);

    int texIndex = 0;
    for (auto& img : images) {
        img->load(*graphicsCommandPool);
    }

    // RENDER PASS
    VulkRenderPass::configureBasicRenderPass(renderPass, swapChain->getImageFormat(), swapChain->getDepthBufferFormat());
    renderPass.build(*device);

   // VulkRenderPass::configureBasicRenderPass(renderPassDepth, swapChain->getImageFormat(), swapChain->getDepthBufferFormat());
   // renderPassDepth.build(*device);


    // FRAMEBUFFERS
    framebuffers = VulkFrameBuffer::createOneToOneFromViews(*device, renderPass, swapChain->getImageExtent(), swapChain->createImageViews(), swapChain->getDepthBufferImageView());
  //  framebuffersWithDepth = VulkFrameBuffer::createOneToOneFromViews(*device, renderPassDepth, swapChain->getImageExtent(), swapChain->createImageViews(), swapChain->getDepthBufferImageView());

    // FLIGHT CONTROLS
    framebufferSync.setup(*graphicsCommandPool, maxFramesInFlight);

    // DRAW CONTEXT
    drawContext = std::make_unique<VulkDrawContext>(this);
}

VulkPipeline& VulkSurfaceRenderManager::addPipeline(VulkShaders &shaders,
                                                 VulkPipelineLayout& layout,
                                           VkPipelineVertexInputStateCreateInfo *vertexInfo,
                                           VkPrimitiveTopology topology, bool is3d)
{
    pipelines.push_back(std::make_unique<VulkPipeline>(*device, swapChain->getImageExtent(), renderPass, shaders, layout, vertexInfo, topology, is3d));
    return *pipelines.back();
}

void VulkSurfaceRenderManager::beginDrawing(bool wasResized)
{
    if (!swapChain) {
        throw std::runtime_error("beginDrawing() called before initialization");
    }

    if (!swapChain->hasSwapChain()) {
        if (wasResized) {
            // if we were resized, we need to recreate the swapchain
            recreateSwapChain();
            if (!swapChain->hasSwapChain()) {
                // maybe next time
                return;
            }
        }
        else {
            // we're minimized or something
            return;
        }
    }

    // get next image to draw into
    while (!beforeDrawCommands(imageIndex)) {
        // recreateSwapChain() was called
    }
}

void VulkSurfaceRenderManager::endDrawing()
{
    if (!swapChain->hasSwapChain()) {
        return;
    }

    // Submits commands to GPU for drawing and presents the image
    processDrawCommands(imageIndex);
}


std::vector<VkImageView> VulkSurfaceRenderManager::imageViews()
{
    std::vector<VkImageView> views;
    for (auto& img : images) {
        if (imagesDirty) {
            img->load(*graphicsCommandPool); // lazy load (if this image not already loaded)
        }
        views.push_back(img->imageView());
    }
    return views;
}

VulkImageInternal::VulkImageInternal(std::string filename, uint32_t texIndex, bool cachePixels)
    : filename{filename}, texIndex{texIndex}, cachePixels{cachePixels}
{
}

VulkImageInternal::VulkImageInternal(int width, int height, uint32_t texIndex, bool cachePixels)
: filename{}, texIndex{texIndex}, cachePixels{cachePixels}
{
    w = width;
    h = height;
    if (cachePixels) {
        pixels = new mssm::Color[w*h];
    }
}

void VulkImageInternal::load(VulkCommandPool &commandPool)
{
    if (isLoaded && !pixelsDirty) {
        return;
    }
    if (!isLoaded) {
        if (filename.empty()) {
            // TODO: do this in one step
            image.create(commandPool, w, h, VK_FORMAT_R8G8B8A8_SRGB, cachePixels);
            image.setPixels(commandPool, std::span<const mssm::Color>(pixels, w*h));
            delete [] pixels;
            if (cachePixels) {
                pixels = image.data<mssm::Color>();
            }
            else {
                pixels = nullptr;
            }
            isLoaded = true;
        }
        else {
            image.load(commandPool, filename, cachePixels);
            if (cachePixels) {
                pixels = image.data<mssm::Color>();
            }
            w = image.width();
            h = image.height();
            isLoaded = true;
        }
    }
    else {
        image.setPixels(commandPool, std::span<const mssm::Color>(pixels, w*h));
        pixelsDirty = false;
    }
}

void VulkImageInternal::updatePixels()
{
    pixelsDirty = true;
    // VulkCommandPool& commandPool;
    // image.setPixels(commandPool, std::span<const mssm::Color>(pixels, w*h));
    //throw std::runtime_error("VulkImageInternal::updatePixels() not implemented");
}

uint32_t VulkImageInternal::textureIndex() const
{
    return texIndex;
}

void VulkSurfaceRenderManager::recreateSwapChain()
{
    std::cerr << "Recreating swapchain" << std::endl;

    device->waitForIdle();

    framebuffers.clear();
    //framebuffersWithDepth.clear();

    swapChain.reset();

    VkExtent2D actualWindowExtent = window->getExtent();

    std::cerr << "Recreating swapchain with extent " << actualWindowExtent.width << "x" << actualWindowExtent.height << std::endl;

    swapChain = std::make_unique<VulkSwapChain>(*device, *surface, actualWindowExtent, hasDepthBuffer);

    framebuffers = VulkFrameBuffer::createOneToOneFromViews(*device, renderPass, swapChain->getImageExtent(), swapChain->createImageViews(), swapChain->getDepthBufferImageView());
   // framebuffersWithDepth = VulkFrameBuffer::createOneToOneFromViews(*device, renderPassDepth, swapChain->getImageExtent(), swapChain->createImageViews(), swapChain->getDepthBufferImageView());

    imageIndex = 0;
}


// Gets next swapchain image that we'll be drawing into
bool VulkSurfaceRenderManager::beforeDrawCommands(uint32_t& imageIndexOut)
{
    if (timeStats.size() < 7) {
        timeStats.resize(7);
    }
    timeStats[0] = std::chrono::duration<float, std::chrono::microseconds::period>(t1 - t0).count();
    timeStats[1] = std::chrono::duration<float, std::chrono::microseconds::period>(t2 - t1).count();
    timeStats[2] = std::chrono::duration<float, std::chrono::microseconds::period>(t3a - t2).count();
    timeStats[3] = std::chrono::duration<float, std::chrono::microseconds::period>(t3 - t3a).count();
    timeStats[4] = std::chrono::duration<float, std::chrono::microseconds::period>(t4 - t3).count();
    timeStats[6] = std::chrono::duration<float, std::chrono::microseconds::period>(t4 - t0).count();

    t0 = std::chrono::high_resolution_clock::now();

    timeStats[5] = std::chrono::duration<float, std::chrono::microseconds::period>(t0 - t4).count();

    framebufferSync.waitForFrameInFlight();
    auto& cmdBuff = framebufferSync.activeCommandBuffer();
    cmdBuff.submitted = false;

    t1 = std::chrono::high_resolution_clock::now();

    for (auto& buffer : buffers) {
        buffer->prepare();
    }

    VkResult result = device->fn.vkAcquireNextImageKHR(*device,
                                                       *swapChain,
                                                       UINT64_MAX,
                                                       framebufferSync.imageAvailableSemaphore(),
                                                       VK_NULL_HANDLE,
                                                       &imageIndexOut);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swapchain image");
    }

    t2 = std::chrono::high_resolution_clock::now();

    return true;
}

// Submits commands to GPU for drawing and presents the image
bool VulkSurfaceRenderManager::processDrawCommands(uint32_t imageIndex)
{
    t3a = std::chrono::high_resolution_clock::now();

    for (auto& buffer : buffers) {
        buffer->submit();
        // TODO: use semaphores and wait for them below?
        //
        // btw:  https://www.reddit.com/r/vulkan/comments/us9p72/multiple_command_buffers_vs_multiple_batches_of/
        // "Yeah I know that but I've never said anything about multiple vkQueueSubmit calls,
        // just one call with multiple VkSubmitInfo structures passed to it,
        //    with one command buffer each, or one call,
        // with one VkSubmitInfo containing all the command buffers."
    }

    auto& cmdBuff = framebufferSync.activeCommandBuffer();
    cmdBuff.submitted = true;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[]   = { framebufferSync.imageAvailableSemaphore() };
    VkSemaphore signal_semaphores[] = { framebufferSync.renderFinishedSemaphore() };
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = wait_semaphores;
    submitInfo.pWaitDstStageMask = wait_stages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = framebufferSync.activeCommandBufferPtr();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signal_semaphores;  // signal renderfinished when done

    VkResult res= device->fn.vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, framebufferSync.inFlightFence());
    debugVkCall("vkQueueSubmit", res);

    t3 = std::chrono::high_resolution_clock::now();

    VkSwapchainKHR swapChains[] = { *swapChain };
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores; // wait on renderfinished before presenting
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapChains;
    present_info.pImageIndices = &imageIndex;

    VkResult result = device->fn.vkQueuePresentKHR(device->getPresentationQueue(), &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreateSwapChain();
        return false;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swapchain image");
    }

    framebufferSync.advanceFrameInFlight();

    t4 = std::chrono::high_resolution_clock::now();

    return true;
}

VulkDrawContext::VulkDrawContext(VulkSurfaceRenderManager *renderManager)
    : renderManager{renderManager},
      device(*renderManager->device),
      commandBuffer(&renderManager->activeCommandBuffer()),
      frameBuffer(renderManager->activeFramebuffer()),
      extent(renderManager->swapChain->getImageExtent()),
      renderPass(renderManager->renderPass)
{
}

void VulkDrawContext::beginBuffer()
{
    commandBuffer->begin(0);
    // VkCommandBufferBeginInfo begin_info = {};
    // begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    // if (device.fn.vkBeginCommandBuffer(*commandBuffer, &begin_info) != VK_SUCCESS) {
    //     throw std::runtime_error("failed to begin recording command buffer");
    // }

    currPipeline = nullptr;
    boundIndexBuffer = nullptr;
}

void VulkDrawContext::beginRenderPass()
{
    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = renderPass;
    render_pass_info.framebuffer = frameBuffer;
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = extent;

    VkClearValue clearColor;
    backgroundColor.setRealArrayRGBA(clearColor.color.float32);
    VkClearValue clearDepth{.depthStencil = {1.0f, 0}};

    std::array<VkClearValue, 2> clearValues = {clearColor, clearDepth};

    render_pass_info.clearValueCount = clearValues.size();
    render_pass_info.pClearValues = clearValues.data();

    commandBuffer->setViewport({{0,0}, extent});
    commandBuffer->setScissor({{0,0},extent});

    // VkViewport viewport = {};
    // viewport.x = 0.0f;
    // viewport.y = 0.0f;
    // viewport.width = (float) extent.width;
    // viewport.height = (float) extent.height;
    // viewport.minDepth = 0.0f;
    // viewport.maxDepth = 1.0f;

    // VkRect2D scissor = {};
    // scissor.offset = {0, 0};
    // scissor.extent = extent;

    // device.fn.vkCmdSetViewport(*commandBuffer, 0, 1, &viewport);
    // device.fn.vkCmdSetScissor(*commandBuffer, 0, 1, &scissor);

    device.fn.vkCmdBeginRenderPass(*commandBuffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkDrawContext::endRenderPass()
{
    device.fn.vkCmdEndRenderPass(*commandBuffer);
}

void VulkDrawContext::endBuffer()
{
    commandBuffer->end();
}

void VulkDrawContext::update(VulkSurfaceRenderManager *renderManager)
{
    commandBuffer = &renderManager->activeCommandBuffer();
    frameBuffer = renderManager->activeFramebuffer();
    extent = renderManager->swapChain->getImageExtent();
    renderPass = renderManager->renderPass;
}

void VulkDrawContext::cmdBindDescriptorSetsImpl(VulkPipelineLayout& layout, VulkPipelineDescriptorSets& pds) {

    uint32_t firstSet = 0;
    uint32_t descriptorSetCount = pds.size();
    uint32_t dynamicOffsetCount = 0;
    const uint32_t* pDynamicOffsets = nullptr;

    device.fn.vkCmdBindDescriptorSets(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                      layout, firstSet,
                                      descriptorSetCount, pds.data(renderManager->flightNumber()),
                                      dynamicOffsetCount, pDynamicOffsets);
}



std::shared_ptr<mssm::ImageInternal> VulkSurfaceRenderManager::loadImg(std::string filename, bool cachePixels)
{
    for (auto& img : images) {
        if (img->getFilename() == filename) {
            return img;
        }
    }
    imagesDirty = true;
    auto img = std::make_shared<VulkImageInternal>(filename, images.size(), cachePixels);
    images.push_back(img);
    return img;
}


std::shared_ptr<mssm::ImageInternal> VulkSurfaceRenderManager::createImg(int width,
                                                                         int height,
                                                                         mssm::Color c,
                                                                         bool cachePixels)
{
    imagesDirty = true;
    auto img = std::make_shared<VulkImageInternal>(width, height, images.size(), true);
    auto p = img->getPixels();
    for (int i = 0; i < width*height; i++) {
        p[i] = c;
    }
    images.push_back(img);
    return img;
}

std::shared_ptr<mssm::ImageInternal> VulkSurfaceRenderManager::initImg(int width,
                                                                       int height,
                                                                       mssm::Color *pixels,
                                                                       bool cachePixels)
{
    imagesDirty = true;
    auto img = std::make_shared<VulkImageInternal>(width, height, images.size(), true);
    auto p = img->getPixels();
    for (int i = 0; i < width*height; i++) {
        p[i] = pixels[i];
    }
    images.push_back(img);
    return img;
}

void VulkSurfaceRenderManager::saveImg(std::shared_ptr<mssm::ImageInternal> img,
                                       std::string filename)
{
    stbi_write_png(filename.c_str(), img->width(), img->height(), 4, img->getPixels(), 4*img->width());
}
