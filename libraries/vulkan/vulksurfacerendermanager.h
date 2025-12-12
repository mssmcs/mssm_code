#ifndef VULKSURFACERENDERMANAGER_H
#define VULKSURFACERENDERMANAGER_H

#include "image.h"
#include "color.h"
#include "vulkcommandbuffers.h"
#include "vulkdescriptorset.h"
#include "vulkdevice.h"
#include "vulkframebuffer.h"
#include "vulkimage.h"
#include "vulkpipeline.h"
#include "vulkrenderpass.h"
#include "vulksmartbuffer.h"
#include "vulksurface.h"
#include "vulkswapchain.h"
#include "vulksynchronization.h"
#include "vulkvertex.h"
#include "vulkabstractwindow.h"
#include "staticmesh.h"

#include <chrono>


class VulkSurfaceRenderManager;

class VulkBoundPipeline
{
public:
    VulkPipeline* pipeline;
    std::vector<VulkSmartBufferBase*> buffers;
public:
    VulkBoundPipeline() {}
    VulkBoundPipeline(VulkPipeline& pipeline, const std::vector<VulkSmartBufferBase*>& buffers) : pipeline(&pipeline), buffers(buffers) {}
    operator VkPipeline() const { return *pipeline; }
    operator VulkPipeline&() const { return *pipeline; }
    VulkPipelineLayout& layout() { return pipeline->layout(); }

};


class VulkDrawContext
{
public:
    mssm::Color backgroundColor{mssm::Color::BLACK()};

    VulkSurfaceRenderManager* renderManager;
    VulkDevice& device;
    VulkCommandBuffer* commandBuffer;
    VkFramebuffer frameBuffer;
    VkExtent2D extent;
    VkRenderPass renderPass;
    VulkPipeline* currPipeline{};

    VulkRawBuffer* boundIndexBuffer;
public:
    VulkDrawContext(VulkSurfaceRenderManager* renderManager);
    void beginBuffer();
    void beginRenderPass();
    void endRenderPass();
    void endBuffer();

public:

    template <typename TPushConstant>
    void sendPushConstants(VulkPipelineLayout& layout, TPushConstant& pushConstants)
    {
        device.fn.vkCmdPushConstants(*commandBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(TPushConstant), &pushConstants);
    }

    void cmdBindPipeline(VulkBoundPipeline& pipeline, VulkPipelineDescriptorSets& pds) {
        if (pipeline.pipeline == currPipeline) {
             return;
        }
        currPipeline = pipeline.pipeline;
        device.fn.vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        cmdBindDescriptorSetsImpl(pipeline.layout(), pds);
        uint32_t bindingIdx = 0;
        for (auto buffer : pipeline.buffers) {
            commandBuffer->bindVertexBuffer(*buffer->getRawBuffer(), bindingIdx++, 0);
        }
    }

protected:

    void cmdBindDescriptorSetsImpl(VulkPipelineLayout& layout, VulkPipelineDescriptorSets &pds);

private:
    void update(VulkSurfaceRenderManager* renderManager);
    friend class VulkSurfaceRenderManager;
};

class VulkImageInternal : public mssm::ImageInternal
{
    std::string filename;
    VulkImage image;
    uint32_t texIndex{0};
    bool isLoaded{false};
    bool pixelsDirty{false};
    bool cachePixels{false};
public:
    VulkImageInternal(std::string filename, uint32_t texIndex, bool cachePixels);
    VulkImageInternal(int width, int height, uint32_t texIndex, bool cachePixels);
    void load(VulkCommandPool& commandPool);
    virtual uint32_t textureIndex() const override;
    VkImageView imageView() const { return image.imageView(); }
    std::string getFilename() const { return filename; }

    // ImageInternal interface
protected:
    void updatePixels() override;
};

class VulkSurfaceRenderManager : public mssm::ImageLoader, public MeshLoader
{
public:
    std::vector<float> timeStats;
private:

    int maxFramesInFlight{0};

    VulkAbstractWindow *window{};

    std::unique_ptr<VulkSurface> surface;

    std::unique_ptr<VulkDevice> device;

    std::unique_ptr<VulkCommandPool> graphicsCommandPool;

    bool hasDepthBuffer{false};

    std::unique_ptr<VulkSwapChain> swapChain;
    uint32_t imageIndex{0}; // current swapchain image index

    VulkRenderPass renderPass;
    //VulkRenderPass renderPassDepth;

    std::vector<std::unique_ptr<VulkPipeline>> pipelines;

    std::vector<std::unique_ptr<VulkFrameBuffer>> framebuffers;
    //std::vector<std::unique_ptr<VulkFrameBuffer>> framebuffersWithDepth;

    bool imagesDirty{true};
    std::vector<std::shared_ptr<VulkImageInternal>> images;

    VulkFramebufferSynchronization framebufferSync;

    std::unique_ptr<VulkDrawContext> drawContext;

    VulkBufferPool bufferPool;

    std::vector<std::unique_ptr<VulkSmartBufferBase>> buffers;

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::chrono::high_resolution_clock::time_point t3;
    std::chrono::high_resolution_clock::time_point t3a;
    std::chrono::high_resolution_clock::time_point t4;
    std::chrono::high_resolution_clock::time_point t0;


public:
    VulkSurfaceRenderManager();
   ~VulkSurfaceRenderManager();
    VulkSurfaceRenderManager(const VulkSurfaceRenderManager&) = delete;
    VulkSurfaceRenderManager& operator=(const VulkSurfaceRenderManager&) = delete;

    void releaseSurface() { surface->release(); } // release handle to prevent double deletion

    uint32_t getNumFramesInFlight() const { return maxFramesInFlight; }

    void beginInitialization(VkInstance instance, VulkAbstractWindow *window, bool includeDepthBuffer, int maxFramesInFlight = 2);

    template <typename T>
    VulkSmartBuffer<T>* createBuffer(VkDeviceSize count);

    template <typename T>
    VulkSmartBuffer<T>* createIndexBuffer(VkDeviceSize count);

    VulkPipeline &addPipeline(VulkShaders &shaders, VulkPipelineLayout &layout, VkPipelineVertexInputStateCreateInfo *vertexInfo, VkPrimitiveTopology topology, bool is3d);

    template <typename T>
    VulkPipeline &addPipeline(VulkShaders &shaders, VulkPipelineLayout &layout, VkPrimitiveTopology topology, bool is3d)
    {
        VertexInfo vertexInfo;
        vertexInfo.addBinding<T>();
        return addPipeline(shaders, layout, vertexInfo.info(), topology, is3d);
    }

    template <typename T1, typename T2>
    VulkPipeline &addPipeline(VulkShaders &shaders, VulkPipelineLayout &layout, VkPrimitiveTopology topology, bool is3d)
    {
        VertexInfo vertexInfo;
        vertexInfo.addBinding<T1>();
        vertexInfo.addBinding<T2>();
        return addPipeline(shaders, layout, vertexInfo.info(), topology, is3d);
    }

    void beginDrawing(bool wasResized);
    void endDrawing();

    bool isDrawable() const { return swapChain->hasSwapChain(); }   // false probably means we're minimized

    VulkDrawContext& getUpdatedDrawContext() {
        drawContext->update(this);
        return *drawContext;
    }

    VulkDevice& getDevice() { return *device; }
    VulkCommandPool& getGraphicsCommandPool() { return *graphicsCommandPool; }

    void waitForIdle() {
        if (device) {
            device->waitForIdle();
        }
    }

    size_t flightNumber() const {
        return framebufferSync.getCurrentFlight();
    }



    bool getImagesDirty() { return imagesDirty; }
    void clearImagesDirty() { imagesDirty = false; }

    std::vector<VkImageView> imageViews();

protected:
    bool beforeDrawCommands(uint32_t &imageIndexOut);
    bool processDrawCommands(uint32_t imageIndex);
    void recreateSwapChain();

    VkFramebuffer activeFramebuffer() const {
        if (framebuffers.empty()) {
            return VK_NULL_HANDLE;
        }
        return *framebuffers[imageIndex];
    }

    VulkCommandBuffer& activeCommandBuffer() const {
        return framebufferSync.activeCommandBuffer();
    }

    VkPipelineLayout getPipelineLayout(VkPipeline pipeline) {
        for (size_t i = 0; i < pipelines.size(); i++) {
            if ((*pipelines[i]) == pipeline) {
                return (*pipelines[i]).layout();
            }
        }
        return VK_NULL_HANDLE;
    }

    friend class VulkDrawContext;

    // ImageLoader interface
public:
    std::shared_ptr<mssm::ImageInternal> loadImg(std::string filename, bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> createImg(int width,
                                                   int height,
                                                   mssm::Color c,
                                                   bool cachePixels) override;
    std::shared_ptr<mssm::ImageInternal> initImg(int width,
                                                 int height,
                                                 mssm::Color *pixels,
                                                 bool cachePixels) override;
    void saveImg(std::shared_ptr<mssm::ImageInternal> img, std::string filename) override;

    std::shared_ptr<StaticMeshInternal> createMesh(const Mesh<EdgeData, VertexData, FaceData>& mesh) override;
    std::shared_ptr<StaticMeshInternal> loadMesh(const std::string& filepath) override;
};

template<typename T>
inline VulkSmartBuffer<T> *VulkSurfaceRenderManager::createBuffer(VkDeviceSize vertexCount)
{
    buffers.push_back(std::make_unique<VulkSmartBuffer<T>>(bufferPool, framebufferSync, *device, vertexCount, maxFramesInFlight));
    return static_cast<VulkSmartBuffer<T> *>(buffers.back().get());
}

template<typename T>
inline VulkSmartBuffer<T> *VulkSurfaceRenderManager::createIndexBuffer(VkDeviceSize count)
{
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    buffers.push_back(std::make_unique<VulkSmartBuffer<T>>(bufferPool, framebufferSync, *device, count, maxFramesInFlight, usage, properties));
    return static_cast<VulkSmartBuffer<T> *>(buffers.back().get());
}

#endif // VULKSURFACERENDERMANAGER_H
