#include "staticmesh.h"


FaceData::FaceData()
{
    c = mssm::GREEN;
}

StaticMesh::StaticMesh(MeshLoader& meshLoader, const Mesh<EdgeData, VertexData, FaceData>& mesh)
    : meshLoader(meshLoader)
{
    internal = meshLoader.createMesh(mesh);
}

StaticMesh::StaticMesh(MeshLoader& meshLoader, const Mesh<EdgeData, VertexDataUV, FaceData>& mesh, const mssm::Image& texture)
    : meshLoader(meshLoader)
{
    internal = meshLoader.createMesh(mesh, texture);
}

StaticMesh::StaticMesh(MeshLoader& meshLoader, const std::string& filepath)
    : meshLoader(meshLoader)
{
    internal = meshLoader.loadMesh(filepath);
}

StaticMesh::~StaticMesh()
{
    if (internal) {
        meshLoader.queueForDestruction(internal);
    }
}
