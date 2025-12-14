#ifndef VULKSTATICMESHINTERNALUV_H
#define VULKSTATICMESHINTERNALUV_H

#include "staticmesh.h"
#include "vulkbuffer.h"
#include "vertex3duv.h"
#include "mesh.h"
#include "vulkcommandbuffers.h"
#include "image.h" // For mssm::Image

class VulkStaticMeshInternalUV : public StaticMeshInternal {
public:
    VulkStaticMeshInternalUV(VulkDevice& device, VulkCommandPool& commandPool, const Mesh<EdgeData, VertexDataUV, FaceData>& mesh, const mssm::Image& texture);

    const VulkBuffer<Vertex3dUV>& getVertexBuffer() const { return vertexBuffer; }
    const VulkBuffer<uint32_t>& getIndexBuffer() const { return indexBuffer; }
    uint32_t getIndexCount() const override { return indexCount; }
    MeshType getMeshType() const override { return MeshType::Textured; }
    const mssm::Image* getTexture() const override { return texture.get(); }

private:
    VulkBuffer<Vertex3dUV> vertexBuffer;
    VulkBuffer<uint32_t>   indexBuffer;
    uint32_t               indexCount;
    std::shared_ptr<mssm::Image> texture;
};

#endif // VULKSTATICMESHINTERNALUV_H
