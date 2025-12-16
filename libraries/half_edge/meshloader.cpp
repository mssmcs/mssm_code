#include "meshloader.h"
#include "vec3d.h"
#include "vec2d.h"
#include <list>
#include <iostream>
#include <functional>
#include <map>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc

#include "tiny_obj_loader.h"

// TODO: use tiny_obj_loader's callback interface to avoid copying data

class ObjFace;

class ObjVert {
public:
    VMeshVertex* meshVertex{nullptr};
    Vec3d pos;
    Vec2f uv;
    bool visited{false};
    std::set<ObjFace*> faces;
public:
    ~ObjVert();
};

class ObjFace {
public:
    bool visited{false};
    VMeshFace* meshFace{nullptr};
    VMeshEdge* corner{nullptr};
    std::vector<ObjVert*> verts;
    std::vector<VMeshEdge*> edges;
    std::set<ObjFace*> adjFaces;
public:
    ~ObjFace();
    VMeshEdge *oppositeEdge(int idx);
    int findEdge(ObjVert* v1, ObjVert* v2);
};

int ObjFace::findEdge(ObjVert *v1, ObjVert *v2)
{
    for (int i = 0; i < verts.size(); i++) {
        if (verts[i] == v1) {
            if (verts[(i+1) % verts.size()] == v2) {
                return i;
            }
        }
    }
    return -1;
}

VMeshEdge* ObjFace::oppositeEdge(int idx)
{
    ObjVert* v1 = verts[idx];
    ObjVert* v2 = verts[(idx+1) % verts.size()];

    for (auto& f : adjFaces) {
        int idx2 = f->findEdge(v2, v1);
        if (idx2 != -1) {
            return f->edges[idx2];
        }
    }
    return nullptr;
}

class ObjMesh {
public:
    std::list<ObjFace> faces;
    std::list<ObjVert> verts;
    std::vector<ObjVert*> vertPtrs;
public:
    ~ObjMesh();
    void reserveTotalVerts(int n) {};
    void reserveTotalFaces(int n) {};
    void startShape(std::string name, int numFaces) {};
    ObjVert* addVert(Vec3d pos, Vec2f uv);
    void addFace(int shapeIdx, std::vector<ObjVert*> vertIndices);
    void linkFaces();
    void buildMesh(VMesh& mesh);
};

ObjVert* ObjMesh::addVert(Vec3d pos, Vec2f uv)
{
    ObjVert& v = verts.emplace_back();
    v.pos = pos;
    v.uv = uv;
    vertPtrs.push_back(&verts.back());
    return &v;
}

void ObjMesh::addFace(int shapeIdx, std::vector<ObjVert*> vertIndices)
{
    ObjFace& f = faces.emplace_back();
    for (auto i : vertIndices) {
        f.verts.push_back(i);
    }
    for (auto v : f.verts) {
        v->faces.insert(&faces.back());
    }
}

void ObjMesh::linkFaces()
{
    for (auto& f : faces) {
        for (auto& v : f.verts) {
            for (auto& f2 : v->faces) {
                if (&f != f2) {
                    f.adjFaces.insert(f2);
                }
            }
        }
    }
}

void ObjMesh::buildMesh(VMesh &mesh)
{
    for (auto& v : verts) {
        v.meshVertex = mesh.createVertex(v.pos, v.uv);
    }

    for (ObjFace& f : faces) {
        std::vector<VMeshVertex*> meshVerts;
        for (auto& v : f.verts) {
            meshVerts.push_back(v->meshVertex);
        }
        f.meshFace = mesh.createFace(meshVerts);
        f.edges = f.meshFace->getEdges();
    }

    for (ObjFace& f : faces) {
        for (int i = 0; i < f.edges.size(); i++) {
            auto edge = f.edges[i];
            auto otherEdge = f.oppositeEdge(i);
            if (otherEdge) {
                mesh.linkEdges(edge, otherEdge);
            }
        }
    }
}

namespace std {
    template<> struct less<tinyobj::index_t> {
        bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b) const {
            if (a.vertex_index < b.vertex_index) return true;
            if (a.vertex_index > b.vertex_index) return false;
            if (a.normal_index < b.normal_index) return true;
            if (a.normal_index > b.normal_index) return false;
            if (a.texcoord_index < b.texcoord_index) return true;
            if (a.texcoord_index > b.texcoord_index) return false;
            return false;
        }
    };
}

void loadMesh(VMesh& mesh, const std::string& filename, bool triangulate)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), NULL, triangulate);

    ObjMesh objMesh;

    if (!warn.empty()) {
        std::cerr << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load/parse .obj file" << std::endl;
        return;
    }

    std::map<tinyobj::index_t, ObjVert*> unique_vertices;

    for (size_t s = 0; s < shapes.size(); s++) {
        objMesh.startShape(shapes[s].name, shapes[s].mesh.num_face_vertices.size());

        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            
            std::vector<ObjVert*> face_verts;
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                if (unique_vertices.find(idx) == unique_vertices.end()) {
                    tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                    
                    tinyobj::real_t tx = 0, ty = 0;
                    if (idx.texcoord_index >= 0) {
                        tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                        ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                    }

                    unique_vertices[idx] = objMesh.addVert({vx, vy, vz}, {tx, 1-ty}); // OBJ format has 0,0 at bottom-left, so flip y
                }
                face_verts.push_back(unique_vertices[idx]);
            }
            objMesh.addFace(s, face_verts);
            index_offset += fv;
        }
    }

    objMesh.linkFaces();
    objMesh.buildMesh(mesh);
    try {
        mesh.repairNonManifoldEdges();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

ObjVert::~ObjVert()
{
    delete meshVertex;
}

ObjFace::~ObjFace()
{
    delete meshFace;
}

ObjMesh::~ObjMesh()
{

}
