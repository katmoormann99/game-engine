//============================================================================
// Author: Kat Moormann
// File: mesh.hpp
// Purpose: Defines CPU and GPU mesh resources used by the rendering system.
//============================================================================

#pragma once

#include "geometry/point3.hpp"
#include "geometry/vector3.hpp"

#include <cstdint>
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

class GPUMesh
{
public:
    bool initialize(const MeshData& meshData);
    void draw() const;
    void shutdown();

private:
    std::uint32_t vao_ = 0;
    std::uint32_t vbo_ = 0;
    std::uint32_t ebo_ = 0;
    std::uint32_t indexCount_ = 0;
};

} // namespace engine