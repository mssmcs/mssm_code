#ifndef TRIANGULARMESH_H
#define TRIANGULARMESH_H

#include "mesh.h" // The half-edge mesh header
#include "tiny_obj_loader.h"
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

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

// Specializations for hashing and comparing tinyobj::index_t for use in unordered_map
namespace std {
    template<> struct hash<tinyobj::index_t> {
        size_t operator()(const tinyobj::index_t& idx) const {
            size_t h1 = hash<int>()(idx.vertex_index);
            size_t h2 = hash<int>()(idx.normal_index);
            size_t h3 = hash<int>()(idx.texcoord_index);
            // A common way to combine hashes
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
    template<> struct equal_to<tinyobj::index_t> {
        bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b) const {
            return a.vertex_index == b.vertex_index &&
                   a.normal_index == b.normal_index &&
                   a.texcoord_index == b.texcoord_index;
        }
    };
}

// This loads an OBJ file directly into a GPU-friendly TriangularMesh,
// bypassing the half-edge structure. It is robust for non-manifold meshes.
template <typename TDestVertex, typename TVertexConverter>
TriangularMesh<TDestVertex> loadTriangularMesh(const std::string& filename, TVertexConverter converter)
{
    TriangularMesh<TDestVertex> triMesh;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn, err;
    std::string folder = filename.substr(0, filename.find_last_of("/\\") + 1);

    // Always triangulate when loading directly for rendering
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), folder.c_str(), true)) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<tinyobj::index_t, uint32_t> unique_vertices;

    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t face_num = 0; face_num < shape.mesh.num_face_vertices.size(); ++face_num) {
            // Since we triangulated, we know fv is 3
            size_t fv = 3;
            
            for (size_t v_num = 0; v_num < fv; ++v_num) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v_num];

                if (unique_vertices.find(idx) == unique_vertices.end()) {
                    unique_vertices[idx] = static_cast<uint32_t>(triMesh.vertices.size());
                    
                    int material_id = shape.mesh.material_ids[face_num];
                    std::optional<const tinyobj::material_t*> mat = std::nullopt;
                    if (material_id >= 0 && material_id < materials.size()) {
                        mat = &materials[material_id];
                    }
                    
                    // Call the user-provided converter to create the vertex
                    triMesh.vertices.push_back(converter(attrib, idx, mat));
                }
                triMesh.indices.push_back(unique_vertices[idx]);
            }
            index_offset += fv;
        }
    }

    return triMesh;
}


#endif // TRIANGULARMESH_H
