#ifndef VULKSTATICMESHINTERNAL_H
#define VULKSTATICMESHINTERNAL_H

#include "staticmesh.h"
#include "vulkbuffer.h"
#include "vertex3duv.h"
#include "mesh.h"
#include "vulkcommandbuffers.h"

// Forward declarations
namespace mssm {
    class Graphics3d;
}
class VulkCanvas;

class VulkStaticMeshInternal : public StaticMeshInternal {
public:
    VulkStaticMeshInternal(VulkDevice& device, VulkCommandPool& commandPool, const Mesh<EdgeData, VertexData, FaceData>& mesh);

    const VulkBuffer<Vertex3dUV>& getVertexBuffer() const { return vertexBuffer; }
    const VulkBuffer<uint32_t>& getIndexBuffer() const { return indexBuffer; }
    uint32_t getIndexCount() const override { return indexCount; }
    MeshType getMeshType() const override { return MeshType::Standard; }

private:
    VulkBuffer<Vertex3dUV> vertexBuffer;
    VulkBuffer<uint32_t>   indexBuffer;
    uint32_t               indexCount;
};

#endif // VULKSTATICMESHINTERNAL_H
