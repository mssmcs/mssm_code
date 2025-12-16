#ifndef MESH_H
#define MESH_H

#include <list>
#include <functional>

#include "loopiterable.h"
#include "geometry.h"

/// half-edge mesh data structure
///
/// Example usage:
///
/// struct Vert {
/// double x;
/// double y;
/// };
///
/// struct EdgeData {
///    // additional data should be associated with half-edges
/// };
///
/// struct VertexData {
///     Vert pos;  // <- use any type that has x and y fields
///     // additional data associated with vertices
/// };
///
/// struct FaceData {
///     // additional data associated with faces
/// };
///
/// using MyMesh = Mesh<EdgeData, VertexData, FaceData>;
///
/// or
///
/// class MyMesh : public Mesh<EdgeData, VertexData, FaceData> {
/// };
///
///
/// To build the mesh:
///
/// MyMesh mesh;
///
/// // make a triangle
/// auto& v1 = mesh.initPoint({0,0});  // this can only ever be called once
/// auto& v2 = mesh.extrudeVertex(v1.firstEdge(), {1,0});
/// auto& v3 = mesh.extrudeVertex(v2.firstEdge(), {1,1});
/// mesh.join(v3.firstEdge(), v1.firstEdge());
///
/// // add a second triangle, making a square
/// auto& v4 = mesh.extrudeVertex(v3.firstEdge(), {0,1});
/// mesh.join(v4.firstEdge(), v1.firstEdge());


class MeshBase {
public:
    virtual size_t numFaces() const = 0;
    virtual size_t numVertices() const = 0;

};

template<typename T>
concept hasPosField = requires (T v) { v.pos; v.pos.x; v.pos.y; };

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
class Mesh
{
private:
    template <typename D>
    static D *AdvanceNext(D* e) { return e->next; }

    template <typename D>
    static D *AdvanceNextOutgoing(D* e) { return e->twin->next; }

    template <typename D>
    static D *AdvanceNextIncoming(D* e) { return e->next->twin; }

    template <typename D, typename V>
    static V &AccessEdge(D* e) { return *e; }

    template <typename D, typename V>
    static V &AccessFace(D* e) { return *e->fp; }

    template <typename D, typename V>
    static V &AccessTwinFace(D* e) { return *e->twin->fp; }

    template <typename D, typename V>
    static V &AccessVertex(D* e) { return *e->v; }

    template <typename D, typename V>
    static V &AccessPoint(D* e) { return e->v->pos; }

    template <typename D, typename V>
    static V &AccessPoint2(D* e) { return e->twin->v->pos; }
public:
    using posType = decltype(VBase::pos); // VBase::posType;

    class Edge;
    class Face;
    class Vertex;

    using Edges = LoopIterable<Edge, Edge, &AccessEdge, &AdvanceNext>;
    using FaceVertices = LoopIterable<Edge, Vertex, &AccessVertex, &AdvanceNext>;
    using FacePoints = LoopIterable<Edge, posType, &AccessPoint, &AdvanceNext>;
    using FaceFaces = LoopIterable<Edge, Face, &AccessTwinFace, &AdvanceNext>;
    using VertexEdges = LoopIterable<Edge, Edge, &AccessEdge, &AdvanceNextOutgoing>;
    using VertexEdgesI = LoopIterable<Edge, Edge, &AccessEdge, &AdvanceNextIncoming>;
    using VertexVertices = LoopIterable<Edge, Vertex, &AccessVertex, &AdvanceNextIncoming>;
    using VertexPoints = LoopIterable<Edge, posType, &AccessPoint2, &AdvanceNextOutgoing>;
    using VertexFaces = LoopIterable<Edge, Face, &AccessFace, &AdvanceNextOutgoing>;

    class Vertex : public VBase {
    private:
        Vertex* next{}; // next in list of vertices on the mesh (this is consequence of allocation strategy, not part of the half edge ADT)
        Vertex* prev{}; // prev in list of vertices on the mesh (this is consequence of allocation strategy, not part of the half edge ADT)
        Edge* edge{}; // one of the "outgoing" edges from this vertex
    public:
        Vertex(const VBase& vdata) : VBase{vdata} {}
        Vertex(const Vertex& other) = delete;
        Vertex& operator= (const Vertex& other) = delete;
        Vertex& operator= (const VBase& other) {
            VBase::operator=(other);
            return *this;
        }

        Vertex* getNext() const { return next; }
        Vertex* getPrev() const { return prev; }

        VertexVertices vertices() const { return {edge->twin}; }
        VertexPoints points() const { return {edge}; }
        VertexEdges outgoingEdges() const { return {edge}; }
        VertexEdgesI incomingEdges() const { return {edge->twin}; }
        VertexFaces faces() const { return {edge}; }

        Edge& firstOutgoingEdge() const { return *edge; }       // well... "first" is arbitrary
        Edge& firstIncomingEdge() const { return *edge->twin; } // well... "first" is arbitrary

        Edge* pickCorner(posType p); // tries to intelligently pick a corner around this vertex
        // such that it could be joined or extruded to the position p (while keeping the topology nice?)

        friend class Mesh;

        template <typename T>
        friend class CircularList;
    };

    class Edge : public EBase  {
    private:
        Vertex* v;     // "start" vertex for this edge
        Edge* twin{};  // twin half edge
        Face* fp{};    // face that this is an edge of
        Edge* next{};  // next edge in loop (next ccw order)
        Edge* prev{};  // prev edge in loop (next cw edge)
    public:
        Edge(Vertex* v1) : v{v1} {}
        Edge(const Edge& other) = delete;
        Edge& operator= (const Edge& other) = delete;
        Edge& operator= (const EBase& other) {
            EBase::operator=(other);
            return *this;
        }

        Edge* getNext() const { return next; }
        Edge* getPrev() const { return prev; }

        Edges edgeLoop() const { return { const_cast<Edge*>(this)}; }
        FaceVertices vertexLoop() const { return { const_cast<Edge*>(this)}; }
        FacePoints pointLoop() const { return { const_cast<Edge*>(this)}; }

        Vertex& v1() const { return *v; }
        Vertex& v2() const { return *next->v; }
        Face& face() const { return *fp; }
        Face* facePtr() const { return fp; }
        Face& adjacentFace() const { return *twin->fp; }
        Face* adjacentFacePtr() const { return twin ? twin->fp : nullptr; }
        const posType& p1() const { return v->pos; }
        const posType& p2() const { return next->v->pos; }

        posType dir() const { return p2() - p1(); }
        posType midPoint() const { return (p1() + p2()) * 0.5; }

        bool isCCW() const;
        bool isCW() const { return !isCCW(); }

        Edge& nextCCW() { return *next; }
        Edge& nextCW() { return *prev; }
        Edge& twinEdge() { return *twin; }
        Edge* twinEdgePtr() { return twin; }
        bool hasTwin() const  { return twin; }

        void setTwin(Edge* e);

        friend class Mesh;
        friend class Vertex;
        template <typename T>
        friend class CircularList;
    };

    class Face : public FBase  {
    private:
        Edge* edge{}; // one of the edges of this face
        Face* next{}; // next in list of faces on the mesh (this is consequence of allocation strategy, not part of the half edge ADT)
        Face* prev{}; // prev in list of faces on the mesh (this is consequence of allocation strategy, not part of the half edge ADT)
    public:
        Face(Edge* edge) : edge{edge} { }
        Face(const Face& other) = delete;
        Face& operator= (const Face& other) = delete;
        Face& operator= (const FBase& other) {
            FBase::operator=(other);
            return *this;
        }

        Face* getNext() const { return next; }
        Face* getPrev() const { return prev; }

        Edges edges() const { return {edge}; }
        FaceVertices vertices() const { return {edge}; }
        FacePoints points() const { return {edge}; }
        FaceFaces faces() const { return {edge}; }

        posType centroid() const;

        void getTriVerts(posType& p1, posType& p2, posType& p3);

        bool isCW() const { return !edge->isCCW(); }
        bool isCCW() const { return edge->isCCW(); }

        friend class Mesh;
        template <typename T>
        friend class CircularList;
    };

private:
    CircularList<Vertex> vertexList;
    CircularList<Face> faceList;

    // cache deleted elements for reuse
    // TODO: think about how to make allocation scheme composable
    CircularList<Edge> deletedEdges;
    CircularList<Vertex> deletedVertices;
    CircularList<Face> deletedFaces;

    // TODO: remove... testing only
    int reusedEdges{};
    int reusedVertices{};
    int reusedFaces{};

public:
    Mesh() {}
    Mesh(const Mesh& other) = delete;
    Mesh& operator= (const Mesh& other) = delete;
   ~Mesh();

    size_t numFaces() const { return faceList.size(); }
    size_t numVertices() const { return vertexList.size(); }

    const CircularList<Face>& faces() const { return faceList; }
    const CircularList<Vertex>& vertices() const { return vertexList; }

    Vertex& initPoint(posType p1);
    Face& initTri(posType p1, posType p2, posType p3);
    Edge& initSeg(posType p1, posType p2);

    Vertex& extrudeVertex(Edge& corner, posType point);
    Edge &join(Edge& corner1, Edge& corner2);
    Face* removeEdge(Edge& e);
    Face* removeMultipleEdges(std::list<Edge*> edgesToRemove);
    Face* removeVertex(Vertex& v);
    Vertex& divideEdge(Edge& e, posType pos);
    Vertex* poke(Face* face, posType pos);

    Vertex* poke(posType pos);
    Vertex& extrudeVertex(Vertex& v, posType point);
    bool join(Vertex& v1, Vertex& v2);
    Vertex& divideEdge(Vertex& v1, Vertex& v2, posType pos);
    bool areAdjacent(Vertex& v1, Vertex& v2);

    bool isOrigFace(Face& face) { return &face == &faceList.front(); }
    bool validate();

    Face& firstFace() { return faceList.front(); }

    Vertex* closestVertex(posType pos) const;
    Edge* closestEdge(posType pos) const;
    Face* enclosingFace(posType pos) const; // note: assumes 2d
    Edge* edgeBetween(Vertex& v1, Vertex& v2) const; // find an edge from v1 to v2

    void iterateEdges(std::function<void(Edge& e)> visitor) const;
    bool iterateEdgesWhileTrue(std::function<bool(Edge& e)> visitor)  const;

    Vertex& createVertex(const VBase& vdata);

    Edge& createEdge(Vertex& vertex);
    Face& createFace(Edge& edge);

    Face& createFace(std::vector<Edge*> edges);
    Face& createFace(std::vector<Vertex*> vertices);

    void repairNonManifoldEdges();

private:

    void deleteVertex(Vertex& v);
    void deleteFace(Face& f);
    void deleteEdge(Edge& e);

    Vertex& insertVertexBetween(Edge& e1, Edge& e2, posType pos);
    bool validate(Vertex& f);
    bool validate(Face& f);
    bool validate(Edge& e);
    void removeTail(Edge& e, Vertex& v);

    Edge* findTwinlessEdge();
    std::vector<Edge*> buildTwinlessEdgeLoop(Edge *start);
};

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
void Mesh<EBase, VBase, FBase>::Edge::setTwin(Edge *e)
{
    if (e == this) {
        throw std::logic_error("Twin cannot be self");
    }
    if (twin) {
        throw std::logic_error("Twin already set");
    }
    if (e->twin && e->twin != this) {
        throw std::logic_error("Twin is twin of another edge");
    }
    twin = e;
}



template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::posType Mesh<EBase, VBase, FBase>::Face::centroid() const
{
    Mesh<EBase, VBase, FBase>::posType sum{0,0};
    int count = 0;
    for (auto& p : points()) {
        sum += p;
        count++;
    }
    return sum / count;
}

template <typename EdgeType>
EdgeType* pickCorner(std::vector<EdgeType*> outgoingEdges, const is2dVector auto& p)
{
    for (EdgeType* e : outgoingEdges) {
        if (leftOfVertex(e->prev->v->pos, e->v->pos, e->next->v->pos, p)) {
            return e;
        }
    }
    return nullptr;
}


template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Edge *Mesh<EBase, VBase, FBase>::Vertex::pickCorner(posType p)
{
    for (auto& e : outgoingEdges()) {
        if (leftOfVertex(e.prev->p1(), e.p1(), e.next->p1(), p)) {
            return &e;
        }
    }


    return nullptr;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
bool Mesh<EBase, VBase, FBase>::Edge::isCCW() const
{
    return ::isCCW<posType>(pointLoop());
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
void Mesh<EBase, VBase, FBase>::Face::getTriVerts(posType &p1, posType &p2, posType &p3)
{
    auto e = edge;
    p1 = e->p1();
    e = e->next;
    p2 = e->p1();
    e = e->next;
    p3 = e->p1();
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::~Mesh()
{
    // delete edges via faces
    for (auto& f : faceList) {
        if (f.edge) {
            Edge* prev = f.edge;
            Edge* e = prev->next;
            prev->next = nullptr;
            while (e) {
                Edge* next = e->next;
                delete e;
                e = next;
            }
        }
    }

    deletedEdges.clear();
    deletedFaces.clear();
    deletedVertices.clear();
    vertexList.clear();
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
void Mesh<EBase, VBase, FBase>::deleteVertex(Vertex &v)
{
    vertexList.remove(&v);
    deletedVertices.push_back(&v);
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
void Mesh<EBase, VBase, FBase>::deleteFace(Face &f)
{
    faceList.remove(&f);
    deletedFaces.push_back(&f);
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
void Mesh<EBase, VBase, FBase>::deleteEdge(Edge &e)
{
    deletedEdges.push_back(&e);
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Face& Mesh<EBase, VBase, FBase>::initTri(posType p1, posType p2, posType p3)
{
    // auto& v1 = initPoint(p1);
    // auto& v2 = initPoint(p2);
    // auto& v3 = initPoint(p3);
    // Edge& e1 = join(v1, v2);
    // Edge& e2 = join(v2, v3);
    // Edge& e3 = join(v3, v1);


    auto& v1 = initPoint(p1);
    auto& v2 = extrudeVertex(v1.firstOutgoingEdge(), p2);
    auto& v3 = extrudeVertex(v2.firstOutgoingEdge(), p3);
    Edge& e3 = join(v3.firstOutgoingEdge(), v1.firstOutgoingEdge());
    if (e3.isCCW()) {
        return e3.face();
    }
    else {
        return e3.adjacentFace();
    }
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Edge &Mesh<EBase, VBase, FBase>::initSeg(posType p1, posType p2)
{
    auto& v = initPoint(p1);
    return extrudeVertex(v.firstOutgoingEdge(), p2);
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Vertex& Mesh<EBase, VBase, FBase>::initPoint(posType p1)
{
    auto& v = createVertex({p1});
    auto& e = createEdge(v);
    auto& f = faceList.empty() ? createFace(e) : faceList.front();

    // if (!vertexList.empty()) {
    //     throw std::logic_error("Cannot initialize point in non-empty mesh");
    // }

    e.fp = &f;

    e.next = &e;
    e.prev = &e;
    e.twin = &e;
    v.edge = &e;

    return v;
}


template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
bool Mesh<EBase, VBase, FBase>::validate(Vertex &v)
{
    int maxCount = 100;

    if (!v.edge) {
        // if (vertices.size() == 1 &&
        //     edges.size() == 0 && faces.size() == 0) {
        //     // special case of single vertex
        //     return true;
        // }
        //cout << "Vertex has no edge" << endl;
        return false;
    }

    Edge* e = v.edge;

    int count = 0;
    do {
        count++;
        if (count > maxCount) {
            //cout << "Vertex has too many edges" << endl;
            return false;
        }
        if (e->v != &v) {
            //cout << "Edge does not point to vertex" << endl;
            return false;
        }
        e = e->twin;
        e = e->next;
    } while (e != v.edge);

    return true;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
bool Mesh<EBase, VBase, FBase>::validate(Face &f)
{
    int maxCount = 100;
    if (!f.edge) {
        //cout << "Face has no edge" << endl;
        return false;
    }
    int count = 0;
    Edge* e = f.edge;
    do {
        count++;
        if (count > maxCount) {
            //cout << "Face has too many edges" << endl;
            return false;
        }
        if (e->fp != &f) {
            //cout << "Edge does not point to its face" << endl;
            return false;
        }
        e = e->next;
        if (!e) {
            //cout << "Edge has no next" << endl;
            return false;
        }
    } while (e != f.edge);


    return true;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
bool Mesh<EBase, VBase, FBase>::validate(Edge &e)
{
    if (!e.next) {
        //cout << "Edge has no next" << endl;
        return false;
    }
    if (!e.prev) {
        //cout << "Edge has no prev" << endl;
        return false;
    }
    if (!e.twin) {
        //cout << "Edge has no twin" << endl;
        return false;
    }
    if (e.next->prev != &e) {
        //cout << "Edge next prev mismatch" << endl;
        return false;
    }
    if (e.prev->next != &e) {
        //cout << "Edge prev next mismatch" << endl;
        return false;
    }
    if (e.twin->twin != &e) {
        //cout << "Edge twin twin mismatch" << endl;
        return false;
    }
    return true;
}


template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
bool Mesh<EBase, VBase, FBase>::validate()
{
    //std::vector<Face*> cwFaces;
    for (auto& f : faceList) {
        if (!validate(f)) {
            return false;
        }

        // if (!f.edge->isCCW()) {
        //     cwFaces.push_back(&f);
        // }
    }

    // if (cwFaces.size() > 1) {
    //     //cout << "Multiple CW faces" << endl;
    //     return false;
    // }

    if (!iterateEdgesWhileTrue([this](Edge& e) { return validate(e); })) {
        return false;
    }

    for (auto& v : vertexList) {
        if (!validate(v)) {
            return false;
        }
    }

    return true;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Vertex *Mesh<EBase, VBase, FBase>::poke(posType pos)
{
    Face* face = enclosingFace(pos);

    if (!face) {
        return nullptr;
    }

    return poke(face, pos);
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Vertex *Mesh<EBase, VBase, FBase>::poke(Face *face, posType pos)
{
    std::vector<Edge*> corners;

    for (auto& e : face->edges()) {
        corners.push_back(&e);
    }

    Vertex& newV = extrudeVertex(*corners[0], pos);

    Edge* innerCorner = newV.edge;

    for (int i = 1; i < corners.size(); i++) {
        join(*corners[i], *innerCorner);
        innerCorner = innerCorner->next->next->twin;
    }

    return &newV;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Vertex& Mesh<EBase, VBase, FBase>::extrudeVertex(Edge &corner, posType point)
{
    if (!corner.fp) {
        throw std::logic_error("null faceptr");
    }

    if (corner.twin == &corner) {
        // special case: extruding from the first vertex.

        Vertex& oldV = corner.v1();
        Vertex& newV = createVertex({point});

        Edge& oldNew = corner;
        Edge& newOld = createEdge(newV);

        newV.edge = &newOld;

        oldNew.twin = &newOld;
        oldNew.next = &newOld;
        oldNew.prev = &newOld;

        newOld.twin = &oldNew;
        newOld.next = &oldNew;
        newOld.prev = &oldNew;

        newOld.fp = corner.fp;



        return newV;
    }

    Edge& prev = *corner.prev;

    Vertex& oldV = corner.v1();
    Vertex& newV = createVertex({point});

    Edge& oldNew = createEdge(oldV);
    Edge& newOld = createEdge(newV);

    newV.edge = &newOld;

    oldNew.twin = &newOld;
    newOld.twin = &oldNew;

    oldNew.next = &newOld;
    oldNew.prev = &prev;

    newOld.next = &corner;
    newOld.prev = &oldNew;

    prev.next   = &oldNew;
    corner.prev = &newOld;

    oldNew.fp = corner.fp;
    newOld.fp = corner.fp;

    return newV;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Vertex& Mesh<EBase, VBase, FBase>::extrudeVertex(Vertex &v, posType point)
{
    // an edge represents not only the side of a face, but also
    // a "face corner"
    // when extruding, we need to no only know the vertex we are extruding,
    // but also which face we are extruding "within", since all edges exist
    // as part of a face
    // so extrude vertex really should take an edge, not a vertex to indicate
    // both the vertex and the face
    Edge* corner = v.pickCorner(point);
    if (!corner) {
        // this is our first edge

        Vertex& v2 = createVertex({point});

        Edge& e1 = createEdge(v);
        Edge& e2 = createEdge(v2);

        v.edge = &e1;
        v2.edge = &e2;

        e1.twin = &e2;
        e2.twin = &e1;

        e1.next = &e2;
        e2.next = &e1;
        e1.prev = &e2;
        e2.prev = &e1;

        return v2;
    }

    return extrudeVertex(*corner, point);
}

/// joins two corners (of the same face)
/// returns the half edge going from corner1 towards corner2
/// this will also be the edge associated with the original face
/// (its twin will be along the newly created face)
template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Edge& Mesh<EBase, VBase, FBase>::join(Edge &corner1, Edge &corner2)
{
    if (corner1.fp != corner2.fp) {
        throw std::logic_error("Cannot join corners from different faces!");
    }

    if (!corner1.fp) {
        throw std::logic_error("null faceptr");
    }

    Edge& beforeV1 = corner1.nextCW();
    Edge& afterV1 = corner1;

    Edge& beforeV2 = corner2.nextCW();
    Edge& afterV2 = corner2;

    Edge& v1v2 = createEdge(corner1.v1());
    Edge& v2v1 = createEdge(corner2.v1());

    v1v2.twin = &v2v1;
    v2v1.twin = &v1v2;

    v1v2.next = &afterV2;
    afterV2.prev = &v1v2;

    v1v2.prev = &beforeV1;
    beforeV1.next = &v1v2;

    v2v1.next = &afterV1;
    afterV1.prev = &v2v1;

    v2v1.prev = &beforeV2;
    beforeV2.next = &v2v1;

    // now handle faces
    // since we connected two existing vertices, we must have split a fp, and
    // that means that the edge loops on either side of the split are isolated

    auto oldFace = beforeV1.fp;
    oldFace->edge = &v1v2;

    // create a new face for the other side and link it to v2v1
    Face& newFace = createFace(v2v1);

    // iterate edges of the faces, setting face ptrs
    for (auto& e : v2v1.edgeLoop()) {
        e.fp = &newFace;
    }

    v1v2.fp = oldFace;

    for (auto& e : v1v2.edgeLoop()) {
        if (e.fp != oldFace) {
            throw std::logic_error("Think about why this happened");
//            cout << "should this happen??" << endl;
            e.fp = oldFace;
        }
    }

    return v1v2;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Vertex& Mesh<EBase, VBase, FBase>::insertVertexBetween(Edge &e1, Edge &e2, posType pos)
{
    // posType t1 = e1.v2().pos;
    // posType t2 = e2.v1().pos;

    Vertex& oldV = e2.v1();
    Vertex& newV = createVertex({pos});

    Edge& oldNew = createEdge(oldV);
    Edge& newOld = createEdge(newV);

    newV.edge = &newOld;

    oldNew.twin = &newOld;
    newOld.twin = &oldNew;

    oldNew.next = &newOld;
    oldNew.prev = &e1;

    newOld.next = &e2;
    newOld.prev = &oldNew;

    e1.next = &oldNew;
    e2.prev = &newOld;

    oldNew.fp = e1.fp;
    newOld.fp = e2.fp;  // TODO: e1.fp should be equal to e2.fp (assert?)

    return newV;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
bool Mesh<EBase, VBase, FBase>::join(Vertex &v1, Vertex &v2)
{
    auto corner1 = v1.pickCorner(v2.pos);
    auto corner2 = v2.pickCorner(v1.pos);

    if (corner1 && corner2 && corner1->fp == corner2->fp) {
        //cout << "Found good corners" << endl;
        join(*corner1, *corner2);
        return true;
    }

    // prefer join across ccw
    for (auto& c1 : v1.outgoingEdges()) {
        if (c1.isCCW()) {
            for (auto& c2 : v2.outgoingEdges()) {
                if (c1.fp == c2.fp) {
                    join(c1, c2);
                    return true;
                }
            }
        }
    }

    for (auto& c1 : v1.outgoingEdges()) {
        if (!c1.isCCW()) {
            for (auto& c2 : v2.outgoingEdges()) {
                if (c1.fp == c2.fp) {
                    join(c1, c2);
                    return true;
                }
            }
        }
    }

    //cout << "Cannot join!!" << endl;
    return false;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Vertex& Mesh<EBase, VBase, FBase>::divideEdge(Vertex &v1, Vertex &v3, posType pos)
{
    Edge* e = edgeBetween(v1, v3);
    if (!e) {
        throw std::logic_error("Vertices are not connected by an edge");
    }
    return divideEdge(*e, pos);
}

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
inline bool Mesh<EBase, VBase, FBase>::areAdjacent(Vertex &v1, Vertex &v2)
{
    for (auto& e : v1.outgoingEdges()) {
        if (&e.v2() == &v2) {
            return true;
        }
    }
    return false;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Vertex& Mesh<EBase, VBase, FBase>::divideEdge(Edge &e, posType pos)
{
    Edge& e12 = e;
    Edge& e32 = *e.twin;

    // split edge between v1 and v3 and place new vertex v2 at pos
    Vertex& v2 = createVertex({pos});

    Edge& e23 = createEdge(v2);
    Edge& e21 = createEdge(v2);

    v2.edge = &e21;

    e23.fp = e12.fp;
    e21.fp = e32.fp;

    Edge& e21Next = *e32.next;
    Edge& e23Next = *e12.next;

    // e12's prev should be unchanged

    // link e12 and e23
    e12.next = &e23;
    e23.prev = &e12;

    // link e23 and e23Next
    e23.next = &e23Next;
    e23Next.prev = &e23;

    // e32's prev should be unchanged

    // link e32 to e21
    e32.next = &e21;
    e21.prev = &e32;

    // link e21 and e21Next
    e21.next = &e21Next;
    e21Next.prev = &e21;

    e12.twin = &e21;
    e21.twin = &e12;

    e23.twin = &e32;
    e32.twin = &e23;

    return v2;
}

// used by remove edge
// if e and its twin are the only edges connected to v, then v should be removed
// otherwise, v should be reattached to another edge
template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
void Mesh<EBase, VBase, FBase>::removeTail(Edge& e, Vertex &v)
{
    if (v.edge == &e) {
        if (e.prev->twin == &e) {
            deleteVertex(v);
        }
        else {
            // reattach v1 to another edge
            v.edge = e.twin->next;
        }
    }
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Face* Mesh<EBase, VBase, FBase>::removeEdge(Edge &e)
{
    auto& twin = *e.twin;
    Mesh::Face* remainingFace;

    if (e.fp == twin.fp) {
        remainingFace = e.fp;
        // same face on both sides of edge.  Might not be able to remove it
        // we can only remove it if it is a single "spur" jutting into the face.
        // which means that one of the following (but not both must be true):
        bool spur1 = e.next == e.twin;
        bool spur2 = e.prev == e.twin;
        if (spur1 == spur2) {
            if (!spur1) {
                // cout << "Cannot create disconnected islands" << endl;
                return nullptr;
            }
            // cout << "Not set up to remove isolated edge" << endl;
            return nullptr;
        }
        else if (spur1) {
            e.fp->edge = e.prev;
        }
        else {
            e.fp->edge = e.next;
        }
    }
    else {
        // this will merge faces, so one needs to go.... which one?  Probably doesn't matter
        Face* loseFace = twin.fp; // sorry twin!

        remainingFace = e.fp;

        // make sure the face that's staying doesn't have a reference to the edge
        // that's being deleted.  Could do this conditionally, but it's easier to just
        // always set it
        remainingFace->edge = e.next;

        // relink edges of face that's going away to the new face
        Edge* eFix = twin.next;
        do {
            eFix->fp = remainingFace;
            eFix = eFix->next;
        } while (eFix != &twin);

        deleteFace(*loseFace);
    }

    removeTail(e, *e.v);
    removeTail(twin, *twin.v);

    e.prev->next = twin.next;
    e.next->prev = twin.prev;

    twin.prev->next = e.next;
    twin.next->prev = e.prev;

    deleteEdge(twin);
    deleteEdge(e);

    return remainingFace;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Face* Mesh<EBase, VBase, FBase>::removeMultipleEdges(std::list<Edge*> edgesToRemove)
{
    int count = edgesToRemove.size()*edgesToRemove.size();
    //int numToRemove = edgesToRemove.size();
    int triesToRemove = 0;

    Face* finalFace{nullptr};

    while (count > 0 && edgesToRemove.size() > 0) {
        triesToRemove++;
        finalFace = removeEdge(*edgesToRemove.back());
        if (finalFace) {
            edgesToRemove.pop_back();
        }
        else {
            Edge* e = edgesToRemove.back();
            edgesToRemove.pop_back();
            edgesToRemove.push_front(e);
        }
        count--;
    }

    // if (triesToRemove > numToRemove) {
    //     cout << (triesToRemove - numToRemove) << "  Took " << triesToRemove << " tries to remove " << numToRemove << " edges" << endl;
    // }

    if (edgesToRemove.size() > 0) {
        throw std::logic_error("Could not remove all edges in list !!!");
//        cout << "Could not remove all edges!!!" << endl;
    }

    return finalFace;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Face *Mesh<EBase, VBase, FBase>::removeVertex(Vertex &v)
{
    std::list<Edge*> edgesToRemove;
    for (auto& e : v.outgoingEdges()) {
        edgesToRemove.push_back(&e);
    }
    return removeMultipleEdges(edgesToRemove);
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Vertex *Mesh<EBase, VBase, FBase>::closestVertex(posType pos) const
{
    Mesh::Vertex* best = nullptr;
    double mindist = std::numeric_limits<double>::max();
    for (auto& v : vertexList) {
        double dist = (v.pos - pos).magnitude();
        if (dist < mindist) {
            mindist = dist;
            best = &v;
        }
    }
    return best;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Edge *Mesh<EBase, VBase, FBase>::closestEdge(posType pos) const
{
    Edge* best{nullptr};
    double mindist = std::numeric_limits<double>::max();
    iterateEdges([&best, &mindist, &pos](Edge& e) {
        double dist = distanceToSegment(pos, e.v1().pos, e.v2().pos);
        if (dist < mindist) {
            mindist = dist;
            best = &e;
        }
    });
    return best;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Face *Mesh<EBase, VBase, FBase>::enclosingFace(posType pos) const
{
    for (auto& f : faceList) {
        if (f.isCW()) {
            continue;
        }
        if (pointInPolygon(pos, f.points())) {
            return &f;
        }
    }
    return nullptr;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Edge *Mesh<EBase, VBase, FBase>::edgeBetween(Vertex &v1, Vertex &v2) const
{
    for (auto& e : v1.outgoingEdges()) {
        if (&e.v2() == &v2) {
            return &e;
        }
    }
    return nullptr;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
void Mesh<EBase, VBase, FBase>::iterateEdges(std::function<void (Edge &)> visitor) const
{
    for (auto& f : faceList) {
        for (auto& e : f.edges()) {
            visitor(e);
        }
    }
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
bool Mesh<EBase, VBase, FBase>::iterateEdgesWhileTrue(std::function<bool (Edge &)> visitor) const
{
    for (auto& f : faceList) {
        for (auto& e : f.edges()) {
            if (!visitor(e)) {
                return false;
            }
        }
    }
    return true;
}


template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Vertex &Mesh<EBase, VBase, FBase>::createVertex(const VBase& vdata)
{
    if (deletedVertices.empty()) {
        vertexList.push_back(new Vertex(vdata));
        return vertexList.back();
    }
    auto& v = deletedVertices.pop_front();

    v = vdata;  // set base to defaults

    vertexList.push_back(&v);
    reusedVertices++;
    return v;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Edge &Mesh<EBase, VBase, FBase>::createEdge(Vertex &vertex)
{
    if (deletedEdges.empty()) {
        return *new Edge(&vertex);
    }
    auto& e = deletedEdges.pop_front();

    EBase base{};
    e = base;  // set base to defaults

    e.v = &vertex;
    reusedEdges++;

    return e;
}

template <typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Face &Mesh<EBase, VBase, FBase>::createFace(Edge &edge)
{
    if (deletedFaces.empty()) {
        faceList.push_back(new Face(&edge));
        return faceList.back();
    }
    auto& f = deletedFaces.pop_front();

    FBase base{};
    f = base;  // set base to defaults

    f.edge = &edge;
    faceList.push_back(&f);
    reusedFaces++;

    return f;
}

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
inline Mesh<EBase, VBase, FBase>::Face &Mesh<EBase, VBase, FBase>::createFace(std::vector<Edge *> edges)
{
    auto& f = createFace(*edges[0]);
    for (int i = 0; i < edges.size(); i++) {
        auto* e1 = edges[i];
        auto* e2 = edges[(i+1) % edges.size()];
        e1->next = e2;
        e2->prev = e1;
        e1->fp = &f;
    }
    return f;
}

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
inline Mesh<EBase, VBase, FBase>::Face &Mesh<EBase, VBase, FBase>::createFace(std::vector<Vertex *> vertices)
{
    std::vector<Edge *> edges;
    for (auto v : vertices) {
        auto& e = createEdge(*v);
        if (!v->edge) {
            v->edge = &e;
        }
        edges.push_back(&e);
    }
    return createFace(edges);
}

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
Mesh<EBase, VBase, FBase>::Edge *Mesh<EBase, VBase, FBase>::findTwinlessEdge()
{
    Edge* e1{nullptr};

    for (auto& f : faces()) {
        for (auto& e : f.edges()) {
            if (!e.twin) {
                e1 = &e;
                break;
            }
            // found an edge without a twin
        }
        if (e1) {
            break;
        }
    }

    return e1;
}

template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
std::vector<typename Mesh<EBase, VBase, FBase>::Edge *> Mesh<EBase, VBase, FBase>::buildTwinlessEdgeLoop(Edge *start)
{
    // start is a twinless edge, try to build a loop of twinless edges

    int edgeCount = 0;

    this->iterateEdges([&edgeCount](Edge& e) {
        edgeCount++;
    });

    std::vector<Edge*> loop;

    loop.push_back(start);

    Edge* eNext{start};
    Edge* eCurr{nullptr};

    do {
        eCurr = eNext;
        eNext = nullptr;
        // find a twinless edge that ends on eCurr's vertex
        this->iterateEdgesWhileTrue([eCurr, &eNext](Edge& e) {
            if (&e.v2() == &eCurr->v1() && e.twin == nullptr) {
                eNext = &e;
                return false; // stop iteration
            }
            return true; // keep searching
        });
        if (eNext) {
            // found the next edge
            if (eNext == loop.front()) {
                // loop is complete!
                break;
            }
            loop.push_back(eNext);
            if (loop.size() > edgeCount) {
                throw std::runtime_error("Cannot repair non manifold mesh");
            }
        }
    }
    while (eNext);

    return loop;
}


template<typename EBase, typename VBase, typename FBase> requires hasPosField<VBase>
void Mesh<EBase, VBase, FBase>::repairNonManifoldEdges()
{
    // search for edges without twins
    // when one is found, try to find a loop of twinless edges, keeping track of the vertices
    // if a loop is found, create a new face using those vertices

    while (true) {
        Edge* e1 = findTwinlessEdge();

        if (!e1) {
            // no twinless edges found
            return;
        }

        auto loop = buildTwinlessEdgeLoop(e1);

        std::vector<Vertex*> loopVerts;

        for (auto& e : loop) {
            loopVerts.push_back(&e->v2());
        }

        Face& newFace = createFace(loopVerts);

        int edgeIdx = 0;
        for (auto& e : newFace.edges()) {
            // link twins
            Edge* et = loop[edgeIdx];

            et->setTwin(&e);
            e.setTwin(et);

            edgeIdx++;
        }
    }
}


#endif // MESH_H
