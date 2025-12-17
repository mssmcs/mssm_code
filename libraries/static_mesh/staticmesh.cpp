#include "staticmesh.h"


FaceData::FaceData()
{
    c = mssm::WHITE;
}

StaticMesh::StaticMesh(MeshLoader& meshLoader, const TriangularMesh<Vertex3dUV>& triMesh)
    : meshLoader(meshLoader)
{
    internal = meshLoader.createMesh(triMesh);
}

StaticMesh::StaticMesh(MeshLoader& meshLoader
                       , const TriangularMesh<Vertex3dUV>& triMesh, const mssm::Image& texture)
    : meshLoader(meshLoader)
{
    internal = meshLoader.createMesh(triMesh, texture);
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
