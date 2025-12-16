#ifndef HALFEDGELOADER_H
#define HALFEDGELOADER_H

#include "meshloader.h"
#include "mesh.h"

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
class VMeshEdgeHE : public VMeshEdge {
    Mesh<EBase, VBase, FBase>::Edge* edge;
public:
    VMeshEdgeHE(Mesh<EBase, VBase, FBase>::Edge* edge) : edge(edge) {}
    void linkTwin(VMeshEdgeHE* twin) {
        edge->setTwin(twin->edge);
    }
};

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
class VMeshFaceHE : public VMeshFace {
public:
    Mesh<EBase, VBase, FBase>::Face* face;
public:
    VMeshFaceHE(Mesh<EBase, VBase, FBase>::Face* face) : face(face) {}
    virtual std::vector<VMeshEdge*> getEdges();
};

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
class VMeshVertexHE : public VMeshVertex {
public:
    Mesh<EBase, VBase, FBase>::Vertex* vertex;
public:
    VMeshVertexHE(Mesh<EBase, VBase, FBase>::Vertex* vertex) : vertex(vertex) {}
};

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
class VMeshHE : public VMesh {
public:
    Mesh<EBase, VBase, FBase>* mesh;
public:
    VMeshHE(Mesh<EBase, VBase, FBase>* mesh) : mesh(mesh) {}

    virtual VMeshVertex* createVertex(Vec3d pos, Vec2f uv);
    virtual VMeshFace* createFace(std::vector<VMeshVertex*> verts);
    virtual void linkEdges(VMeshEdge* e1, VMeshEdge* e2);
    virtual void repairNonManifoldEdges();
};

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
inline std::vector<VMeshEdge *> VMeshFaceHE<EBase, VBase, FBase>::getEdges()
{
    std::vector<VMeshEdge *> edges;
    for (auto& e : face->edges()) {
        VMeshEdgeHE<EBase, VBase, FBase> *edge = new VMeshEdgeHE<EBase, VBase, FBase>(&e);
        edges.push_back(edge);
    }
    return edges;
}

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
VMeshVertex *VMeshHE<EBase, VBase, FBase>::createVertex(Vec3d pos, Vec2f uv)
{
    VBase vdata;
    vdata.pos = decltype(vdata.pos){pos};
    if constexpr (requires { vdata.uv; }) {
        vdata.uv = uv;
    }
    auto& mv = mesh->createVertex(vdata);
    return new VMeshVertexHE<EBase, VBase, FBase>(&mv);
}

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
VMeshFace *VMeshHE<EBase, VBase, FBase>::createFace(std::vector<VMeshVertex *> verts)
{
    std::vector<typename Mesh<EBase, VBase, FBase>::Vertex*> mVerts;
    for (auto v : verts) {
        auto vhe = static_cast<VMeshVertexHE<EBase, VBase, FBase>*>(v);
        mVerts.push_back(vhe->vertex);
    }
    auto& mf = mesh->createFace(mVerts);
    return new VMeshFaceHE<EBase, VBase, FBase>(&mf);
}

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
void VMeshHE<EBase, VBase, FBase>::linkEdges(VMeshEdge *e1, VMeshEdge *e2)
{
    auto e1he = static_cast<VMeshEdgeHE<EBase, VBase, FBase>*>(e1);
    auto e2he = static_cast<VMeshEdgeHE<EBase, VBase, FBase>*>(e2);
    e1he->linkTwin(e2he);
}

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
void VMeshHE<EBase, VBase, FBase>::repairNonManifoldEdges()
{
    mesh->repairNonManifoldEdges();
}

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
void loadMesh(Mesh<EBase, VBase, FBase>& mesh, const std::string& filename, bool triangulate)
{
    VMeshHE heMesh(&mesh);
    loadMesh(heMesh, filename, triangulate);
}

#endif // HALFEDGELOADER_H
