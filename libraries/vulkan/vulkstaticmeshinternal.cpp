#include "vulkstaticmeshinternal.h"
#include "mesh.h"
#include "vulkcommandbuffers.h"
#include <unordered_map>
#include "triangularmesh.h"

VulkStaticMeshInternal::VulkStaticMeshInternal(VulkDevice& device, VulkCommandPool& commandPool, const Mesh<EdgeData, VertexData, FaceData>& mesh)
{
    auto converter = [](const VertexData& v, const FaceData& f) {
        Vertex3dUV dest_v;
        dest_v.pos = v.pos;
        dest_v.normal = v.normal;
        dest_v.color = f.c.toRealVec4<Vec4f>();
        dest_v.uv = {0.0f, 0.0f}; // Default UV for non-UV meshes
        return dest_v;
    };

    auto triMesh = buildTriangularMesh<Vertex3dUV>(mesh, converter);

    this->indexCount = triMesh.indices.size();

    // Create staging buffer for vertices
    VulkBuffer<Vertex3dUV> vertexStagingBuffer(device, triMesh.vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vertexStagingBuffer.copyFrom(triMesh.vertices);

    // Create device-local vertex buffer
    this->vertexBuffer.initialize(device, triMesh.vertices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Create staging buffer for indices
    VulkBuffer<uint32_t> indexStagingBuffer(device, triMesh.indices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    indexStagingBuffer.copyFrom(triMesh.indices);

    // Create device-local index buffer
    this->indexBuffer.initialize(device, triMesh.indices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Copy from staging to device-local buffers
    VulkCommandBuffer::oneTimeCommmand(commandPool, [&](VulkDevice& device, VulkCommandBuffer& commandBuffer) {
        commandBuffer.copyBuffer(vertexStagingBuffer, this->vertexBuffer, sizeof(Vertex3dUV) * triMesh.vertices.size());
        commandBuffer.copyBuffer(indexStagingBuffer, this->indexBuffer, sizeof(uint32_t) * triMesh.indices.size());
    });
}
