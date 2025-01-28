#include "vulkcanvasbase.h"

VulkCanvasBase::VulkCanvasBase(VulkSurfaceRenderManager &renderManager)
    : renderManager(renderManager)
    , device{renderManager.getDevice()}
    , descSetManager{renderManager.getNumFramesInFlight()}
{

}


VulkImage *VulkCanvasBase::addTexture(int width, int height, VkFormat format)
{
    VulkImage image;

    image.create(renderManager.getGraphicsCommandPool(), width, height, format, true);

    textures.push_back(std::move(image));

    return &textures.back();
}

VulkImage* VulkCanvasBase::addTexture(std::string filename, bool retainStagingBuffer)
{
    VulkImage image;

    image.load(renderManager.getGraphicsCommandPool(), filename, retainStagingBuffer);

    textures.push_back(std::move(image));

    return &textures.back();
}

void VulkCanvasBase::beginPaint()
{
    dc = &renderManager.getUpdatedDrawContext();
    extent = dc->extent;
    renderPass = dc->renderPass;

    dc->beginBuffer();
    dc->beginRenderPass();
}

void VulkCanvasBase::endPaint()
{
    dc->endRenderPass();
    dc->endBuffer();
}
