#ifndef VULKCANVASBASE_H
#define VULKCANVASBASE_H

#include "vulkimage.h"
#include "vulksurfacerendermanager.h"

class VulkCanvasBase0 {
public:
    virtual ~VulkCanvasBase0() = default;
    virtual void initializePipelines() = 0;
    virtual void beginPaint() = 0;
    virtual void endPaint() = 0;
};

class VulkCanvasBase : public VulkCanvasBase0 {
protected:
    VulkSurfaceRenderManager &renderManager;
    VulkDevice &device;
    VulkDescriptorSetManager descSetManager;

    std::list<VulkImage> textures; // use list to maintain valid pointers

    VulkDrawContext *dc{};
    VkExtent2D extent{};
    VkRenderPass renderPass{};

public:
    VulkCanvasBase(VulkSurfaceRenderManager &renderManager);
    virtual ~VulkCanvasBase() = default;

    virtual void initializePipelines() = 0;

    VulkImage *addTexture(int width, int height, VkFormat format);
    VulkImage *addTexture(std::string filename, bool retainStagingBuffer);


    template<typename T>
    VulkBoundPipeline createPipeline(VkPrimitiveTopology topology,
                                     std::string vertShader,
                                     std::string fragShader,
                                     VulkPipelineLayout &pipelineLayout,
                                     VulkSmartBuffer<T> *buffer,
                                     bool is3d);

    template<typename T1, typename T2>
    VulkBoundPipeline createPipeline(VkPrimitiveTopology topology,
                                     std::string vertShader,
                                     std::string fragShader,
                                     VulkPipelineLayout &pipelineLayout,
                                     VulkSmartBuffer<T1> *buffer1,
                                     VulkSmartBuffer<T2> *buffer2,
                                     bool is3d);

    virtual void beginPaint();
    virtual void endPaint();
};


template<typename T1, typename T2>
inline VulkBoundPipeline VulkCanvasBase::createPipeline(VkPrimitiveTopology topology,
                                                        std::string vertShader,
                                                        std::string fragShader,
                                                        VulkPipelineLayout &pipelineLayout,
                                                        VulkSmartBuffer<T1> *buffer1,
                                                        VulkSmartBuffer<T2> *buffer2,
                                                        bool is3d)
{
    VulkShaders shaders(device);

    std::cout << "Creating Pipeline for\n  " << vertShader << " and\n  " << fragShader << std::endl;

    shaders.addVertStage(vertShader);
    shaders.addFragStage(fragShader);

    VulkPipeline& pipeline = renderManager.addPipeline<T1, T2>(shaders,
                                                               pipelineLayout,
                                                               topology, is3d);

    return {pipeline, {buffer1, buffer2}};
}

template<typename T>
inline VulkBoundPipeline VulkCanvasBase::createPipeline(VkPrimitiveTopology topology,
                                                        std::string vertShader,
                                                        std::string fragShader,
                                                        VulkPipelineLayout &pipelineLayout,
                                                        VulkSmartBuffer<T> *buffer,
                                                        bool is3d)
{
    VulkShaders shaders(device);

    std::cout << "Creating Pipeline for\n  " << vertShader << " and\n  " << fragShader << std::endl;

    shaders.addVertStage(vertShader);
    shaders.addFragStage(fragShader);

    VulkPipeline& pipeline = renderManager.addPipeline<T>(shaders,
                                                          pipelineLayout,
                                                          topology, is3d);

    return {pipeline, {buffer}};
}


#endif // VULKCANVASBASE_H
