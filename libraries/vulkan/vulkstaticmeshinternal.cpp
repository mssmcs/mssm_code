#include "vulkstaticmeshinternal.h"
#include "mesh.h"
#include "vulkcommandbuffers.h"
#include <unordered_map>

VulkStaticMeshInternal::VulkStaticMeshInternal(VulkDevice& device, VulkCommandPool& commandPool, const Mesh<EdgeData, VertexData, FaceData>& mesh)
{
    std::vector<Vertex3dUV> vertices;
    std::vector<uint32_t> indices;
    
    std::unordered_map<const Mesh<EdgeData, VertexData, FaceData>::Vertex*, uint32_t> vertex_map;

    for (const auto& face : mesh.faces()) {
        for (const auto& edge : face.edges()) {
            auto v = &edge.v1();
            if (vertex_map.find(v) == vertex_map.end()) {
                vertex_map[v] = vertices.size();
                vertices.push_back({v->pos, v->normal, face.c, Vec2f{0, 0}});
            }
            indices.push_back(vertex_map[v]);
        }
    }

    this->indexCount = indices.size();

    // Create staging buffer for vertices
    VulkBuffer<Vertex3dUV> vertexStagingBuffer(device, vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vertexStagingBuffer.copyFrom(vertices);

    // Create device-local vertex buffer
    this->vertexBuffer.initialize(device, vertices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Create staging buffer for indices
    VulkBuffer<uint32_t> indexStagingBuffer(device, indices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    indexStagingBuffer.copyFrom(indices);

    // Create device-local index buffer
    this->indexBuffer.initialize(device, indices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Copy from staging to device-local buffers
    VulkCommandBuffer::oneTimeCommmand(commandPool, [&](VulkDevice& device, VulkCommandBuffer& commandBuffer) {
        commandBuffer.copyBuffer(vertexStagingBuffer, this->vertexBuffer, sizeof(Vertex3dUV) * vertices.size());
        commandBuffer.copyBuffer(indexStagingBuffer, this->indexBuffer, sizeof(uint32_t) * indices.size());
    });

    device.waitForIdle();
}
