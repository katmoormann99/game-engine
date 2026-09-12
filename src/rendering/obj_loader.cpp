//============================================================================
// Author: Kat Moormann
// File: obj_loader.cpp
// Purpose: Implements loading of simple triangle-based Wavefront OBJ meshes
//          and generates smooth per-vertex normals when normals are absent.
//============================================================================

#include "engine/rendering/obj_loader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace engine
{

MeshData loadOBJ(const std::string& path)
{
    std::ifstream file(path);

    if (!file)
    {
        throw std::runtime_error(
            "Could not open OBJ file: " + path
        );
    }

    std::vector<cg::Point3> positions;
    std::vector<std::uint32_t> indices;

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::istringstream stream(line);

        std::string tag;
        stream >> tag;

        if (tag == "v")
        {
            float x;
            float y;
            float z;

            stream >> x >> y >> z;

            positions.emplace_back(x, y, z);
        }
        else if (tag == "f")
        {
            std::uint32_t a;
            std::uint32_t b;
            std::uint32_t c;

            stream >> a >> b >> c;

            // OBJ indexing begins at 1.
            // C++ vector indexing begins at 0.
            indices.push_back(a - 1);
            indices.push_back(b - 1);
            indices.push_back(c - 1);
        }
    }

    MeshData mesh;

    mesh.vertices.resize(positions.size());
    mesh.indices = indices;

    // Copy positions and initialize normals.
    for (std::size_t i = 0; i < positions.size(); ++i)
    {
        mesh.vertices[i].position = positions[i];
        mesh.vertices[i].normal.set(
            0.0f,
            0.0f,
            0.0f
        );
    }

    // Calculate face normals and accumulate them into the vertices.
    for (std::size_t i = 0;
         i + 2 < indices.size();
         i += 3)
    {
        const std::uint32_t i0 = indices[i];
        const std::uint32_t i1 = indices[i + 1];
        const std::uint32_t i2 = indices[i + 2];

        const cg::Point3& p0 = positions[i0];
        const cg::Point3& p1 = positions[i1];
        const cg::Point3& p2 = positions[i2];

        cg::Vector3 edge1(p0, p1);
        cg::Vector3 edge2(p0, p2);

        cg::Vector3 faceNormal =
            edge1.cross(edge2);

        mesh.vertices[i0].normal += faceNormal;
        mesh.vertices[i1].normal += faceNormal;
        mesh.vertices[i2].normal += faceNormal;
    }

    // Convert accumulated normals into unit vectors.
    for (MeshVertex& vertex : mesh.vertices)
    {
        if (vertex.normal.norm_squared() > 1e-12f)
        {
            vertex.normal.normalize();
        }
        else
        {
            vertex.normal.set(
                0.0f,
                0.0f,
                1.0f
            );
        }
    }

    return mesh;
}

} // namespace engine