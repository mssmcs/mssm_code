#ifndef TRIANGULARMESH_H
#define TRIANGULARMESH_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include "Mesh.h"

// The GPU-friendly representation.
template <typename TVertex>
struct TriangularMesh {
    std::vector<TVertex> vertices;
    std::vector<uint32_t>   indices;
};

// The conversion function
// This converts from a half-edge mesh to the GPU-friendly triangular mesh.
// It handles triangulation and demotion of per-face attributes.
// It is generic, accepting a 'converter' lambda to map source vertex/face data to the destination vertex format.
template <typename TDestVertex, typename E, typename V, typename F, typename TConverter>
TriangularMesh<TDestVertex> buildTriangularMesh(const Mesh<E, V, F>& mesh, TConverter converter)
{
    TriangularMesh<TDestVertex> triMesh;
    // Use the Edge pointer as a key to uniquely identify a "corner" (a vertex in the context of a specific face)
    // This correctly duplicates vertices for per-face attributes (like color).
    std::unordered_map<const typename Mesh<E, V, F>::Edge*, uint32_t> corner_map;

    for (const auto& face : mesh.faces()) {
        std::vector<const typename Mesh<E, V, F>::Edge*> face_edges;
        for (const auto& edge : face.edges()) {
            face_edges.push_back(&edge);
        }

        if (face_edges.size() < 3) {
            continue;
        }

        // --- Triangulate the face using a triangle fan ---
        const auto* e0 = face_edges[0];
        for (size_t i = 1; i < face_edges.size() - 1; ++i) {
            const auto* e1 = face_edges[i];
            const auto* e2 = face_edges[i+1];
            
            const typename Mesh<E, V, F>::Edge* corners[] = {e0, e1, e2};
            
            // For each vertex of the new triangle
            for (const auto* corner : corners) {
                // If we haven't created a GPU vertex for this specific corner yet, create one.
                if (corner_map.find(corner) == corner_map.end()) {
                    corner_map[corner] = static_cast<uint32_t>(triMesh.vertices.size());
                    // Call the provided converter to create the destination vertex
                    triMesh.vertices.push_back(converter(corner->v1(), face));
                }
                // Add the index of this corner's vertex to our index buffer.
                triMesh.indices.push_back(corner_map[corner]);
            }
        }
    }

    return triMesh;
}


#endif // TRIANGULARMESH_H
