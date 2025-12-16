#ifndef STATICMESH_H
#define STATICMESH_H

#include "color.h"
#include "mesh.h" // For Meshy
#include "vec3d.h"
#include "vec2d.h"
#include <memory>
#include <string>

struct EdgeData {
    bool marked{};
    Vec3f normal;
};

struct VertexData {
    Vec3f pos;
    Vec3f normal;

    bool marked{};
};

// New struct for vertex data with UV coordinates
struct VertexDataUV {
    Vec3f pos;
    Vec3f normal;
    Vec2f uv; // Texture coordinates

    bool marked{};
};

struct FaceData {
    mssm::Color c;
    bool marked{};
    FaceData();
};

enum class MeshType {
    Standard,
    Textured
};

// Forward declarations
namespace mssm {
    class Graphics3d;
    class Image;
}

class StaticMeshInternal {
public:
    virtual ~StaticMeshInternal() = default;
    virtual uint32_t getIndexCount() const = 0;
    virtual MeshType getMeshType() const = 0;
    virtual const mssm::Image* getTexture() const { return nullptr; }
};

class MeshLoader {
public:
    virtual std::shared_ptr<StaticMeshInternal> createMesh(const Mesh<EdgeData, VertexData, FaceData>& mesh) = 0;
    virtual std::shared_ptr<StaticMeshInternal> createMesh(const Mesh<EdgeData, VertexDataUV, FaceData>& mesh, const mssm::Image& texture) = 0;
    virtual std::shared_ptr<StaticMeshInternal> loadMesh(const std::string& filepath) = 0;
    virtual void queueForDestruction(std::shared_ptr<StaticMeshInternal> mesh) = 0;
};

class StaticMesh {
protected:
    MeshLoader& meshLoader;
public:
    std::shared_ptr<StaticMeshInternal> internal;
public:
    StaticMesh(MeshLoader& meshLoader, const Mesh<EdgeData, VertexData, FaceData>& mesh);
    StaticMesh(MeshLoader& meshLoader, const Mesh<EdgeData, VertexDataUV, FaceData>& mesh, const mssm::Image& texture);
    StaticMesh(MeshLoader& meshLoader, const std::string& filepath);
    ~StaticMesh();
};

#endif // STATICMESH_H
