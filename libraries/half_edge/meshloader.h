#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <vector>
#include "vec3d.h"
#include "vec2d.h"

class VMeshEdge {
public:
    virtual ~VMeshEdge() {}
};

class VMeshFace {
public:
    virtual ~VMeshFace() {}
    virtual std::vector<VMeshEdge*> getEdges() = 0;
};

class VMeshVertex {
public:
    virtual ~VMeshVertex() {}
};

class VMesh {
public:
    virtual ~VMesh() {}
    virtual VMeshVertex* createVertex(Vec3d pos, Vec2f uv) = 0;
    virtual VMeshFace* createFace(std::vector<VMeshVertex*> verts) = 0;
    virtual void linkEdges(VMeshEdge* e1, VMeshEdge* e2) = 0;
    virtual void repairNonManifoldEdges() = 0;
};

void loadMesh(VMesh& mesh, const std::string& filename, bool triangulate);


#endif // MESHLOADER_H
