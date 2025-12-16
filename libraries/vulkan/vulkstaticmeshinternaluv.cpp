#include "vulkstaticmeshinternaluv.h"
#include "mesh.h"
#include "vulkcommandbuffers.h"
#include <unordered_map>

VulkStaticMeshInternalUV::VulkStaticMeshInternalUV(VulkDevice& device, VulkCommandPool& commandPool, const Mesh<EdgeData, VertexDataUV, FaceData>& mesh, const mssm::Image& tex)
    : texture(std::make_shared<mssm::Image>(tex))
{
    std::vector<Vertex3dUV> vertices;
    std::vector<uint32_t> indices;
    
    std::unordered_map<const Mesh<EdgeData, VertexDataUV, FaceData>::Vertex*, uint32_t> vertex_map;

    for (const auto& face : mesh.faces()) {
        std::vector<const Mesh<EdgeData, VertexDataUV, FaceData>::Vertex*> face_verts;
        for (const auto& edge : face.edges()) {
            face_verts.push_back(&edge.v1());
        }

        if (face_verts.size() < 3) {
            continue;
        }

        for (const auto* v_ptr : face_verts) {
             if (vertex_map.find(v_ptr) == vertex_map.end()) {
                vertex_map[v_ptr] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(Vertex3dUV{v_ptr->pos, v_ptr->normal, face.c, v_ptr->uv});
            }
        }

        // Create a triangle fan for the indices
        uint32_t i0 = vertex_map[face_verts[0]];
        for (size_t i = 1; i < face_verts.size() - 1; ++i) {
            uint32_t i1 = vertex_map[face_verts[i]];
            uint32_t i2 = vertex_map[face_verts[i+1]];
            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);
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
}
