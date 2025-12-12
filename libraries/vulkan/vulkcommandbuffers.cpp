#include "vulkcommandbuffers.h"
#include "vulkdevice.h"
#include <stdexcept>


VulkCommandBuffers::VulkCommandBuffers(VulkCommandPool& commandPool, int count)
    : VulkHasDev{commandPool.device()}, commandPool{commandPool}
{
    buffers.resize(count);

    std::vector<VkCommandBuffer> bufferHandles(count);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = bufferHandles.size();

    VKCALLD(vkAllocateCommandBuffers, &allocInfo, bufferHandles.data());

    for (int i = 0; i < buffers.size(); i++) {
        buffers[i].initialize(commandPool, bufferHandles[i]);
    }
}

VulkCommandBuffers::~VulkCommandBuffers()
{    
//    fn()->vkFreeCommandBuffers(device(), commandPool, buffers.size(), buffers.data());
}

VulkCommandPool::VulkCommandPool(VulkDevice &device)
{
    queue = device.getGraphicsQueue();  // TODO: do we need command pools for other (non graphics) queues?
    initialize(device);
}

void VulkCommandPool::initialize(VulkDevice &device)
{
    initDeviceHandle(device);

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = device.getGraphicsQueueIndex();

    VKCALLD(vkCreateCommandPool, &pool_info, nullptr, &handle);
}

VulkCommandBuffer::VulkCommandBuffer(VulkCommandPool &commandPoolRef)
{
    initialize(commandPoolRef);
}

void VulkCommandBuffer::initialize(VulkCommandPool &commandPoolRef)
{
    initDeviceHandle(commandPoolRef.device());

    commandPool = &commandPoolRef;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = *commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (fn()->vkAllocateCommandBuffers(device(), &allocInfo, &cmdBuffer)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffer");
    }
}

void VulkCommandBuffer::initialize(VulkCommandPool &commandPoolRef, VkCommandBuffer newBuffer)
{
    initDeviceHandle(commandPoolRef.device());

    assertm(commandPool == nullptr, "VulkCommandBuffer::initialize commandPool not null");
    assertm(cmdBuffer == nullptr, "VulkCommandBuffer::initialize buffer not null");
    commandPool = &commandPoolRef;
    cmdBuffer = newBuffer;
}

void VulkCommandBuffer::begin(VkCommandBufferUsageFlags flags)
{
    if (hasBegun) {
        throw std::runtime_error("double begin");


    }
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;
    if (fn()->vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer");
    }
    hasBegun = true;
}

void VulkCommandBuffer::end()
{
    hasBegun = false;
    if (fn()->vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer");
    }
}

void VulkCommandBuffer::bindIndexBuffer(VulkRawBuffer& buffer, VkDeviceSize offset, VkIndexType indexType)
{
    fn()->vkCmdBindIndexBuffer(cmdBuffer, buffer, offset, indexType);
}

void VulkCommandBuffer::bindVertexBuffer(VulkRawBuffer &buffer, uint32_t bindingIndex, VkDeviceSize offset)
{
    buffer.setLastBindingIndex(bindingIndex);
    VkBuffer vertexBuffers[] = { buffer };
    VkDeviceSize offsets{ offset };
    assertm(vertexBuffers[0] != nullptr, "VulkCommandBuffer::bindVertexBuffer vertexBuffers[0] is nullptr");
    fn()->vkCmdBindVertexBuffers(cmdBuffer, bindingIndex, 1, vertexBuffers, &offsets);
}

void VulkCommandBuffer::drawIndexed(uint32_t indexCount,
                                    uint32_t instanceCount,
                                    uint32_t firstIndex,
                                    int32_t vertexOffset,
                                    uint32_t firstInstance)
{
    fn()->vkCmdDrawIndexed(cmdBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkCommandBuffer::draw(uint32_t vertexCount,
                             uint32_t instanceCount,
                             uint32_t firstVertex,
                                                                                                                       uint32_t firstInstance)
                                                                                         {
                                                                                             fn()->vkCmdDraw(cmdBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
                                                                                         }
                                                                                         
                                                                                         void VulkCommandBuffer::copyBuffer(VulkRawBuffer& srcBuffer, VulkRawBuffer& dstBuffer, VkDeviceSize size)
                                                                                         {
                                                                                             VkBufferCopy copyRegion{};
                                                                                             copyRegion.size = size;
                                                                                             fn()->vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
                                                                                         }
                                                                                         
                                                                                         void VulkCommandBuffer::submit(bool waitIdle)
                                                                                         {
                                                                                             VkSubmitInfo submitInfo{};    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    submitted = true;
    fn()->vkQueueSubmit(getQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    if (waitIdle) {
        fn()->vkQueueWaitIdle(getQueue());
    }
}

void VulkCommandBuffer::oneTimeCommand(std::function<void(VulkDevice& device, VulkCommandBuffer &)> func)
{
    begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    func(device(), *this);
    end();
    submit(true);
}

void VulkCommandBuffer::setScissor(VkRect2D scissor)
{
    fn()->vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
}

void VulkCommandBuffer::setViewport(VkRect2D viewport)
{
    VkViewport vkViewport{};
    vkViewport.minDepth = 0.0f;
    vkViewport.maxDepth = 1.0f;
    vkViewport.x = viewport.offset.x;
    vkViewport.y = viewport.offset.y;
    vkViewport.width = viewport.extent.width;
    vkViewport.height = viewport.extent.height;
    fn()->vkCmdSetViewport(cmdBuffer, 0, 1, &vkViewport);
}

void VulkCommandBuffer::oneTimeCommmand(VulkCommandPool &commandPool,
                                        std::function<void(VulkDevice &, VulkCommandBuffer &)> func)
{
    VulkCommandBuffer commandBuffer(commandPool);  // one time use command buffer, is this ok?
    commandBuffer.oneTimeCommand(func);
}

VulkCommandBuffer::~VulkCommandBuffer()
{
    fn()->vkFreeCommandBuffers(device(), *commandPool, 1, &cmdBuffer);
}



VulkCommandSequence::VulkCommandSequence(VulkCommandPool &commandPool)
: commandPool{commandPool}, commandBuffer(commandPool)
{
    commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}

void VulkCommandSequence::add(std::function<void(VulkDevice &, VulkCommandBuffer &)> func)
{
    func(commandBuffer.device(), commandBuffer);
}

void VulkCommandSequence::execute(bool waitIdle)
{
    commandBuffer.end();
    commandBuffer.submit(waitIdle);
}
