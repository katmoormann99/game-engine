//============================================================================
// Author: Kat Moormann
// File: obj_loader.hpp
// Purpose: Loads triangle geometry from Wavefront OBJ files into CPU-side
//          vertex and index buffers for use by the rendering backend.
// Date: June 28 2025
//============================================================================

#pragma once
#include "geometry/point3.hpp"
#include "geometry/vector3.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace engine
{
    struct MeshVertex
    {
        cg::Point3 position;
        cg::Vector3 normal;
    };

    struct MeshData
    {
        std::vector<MeshVertex> vertices;
        std::vector<std::uint32_t> indices;
    };

    MeshData loadOBJ(const std::string &path);
}

