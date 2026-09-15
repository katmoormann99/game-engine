//============================================================================
// Author: Kat Moormann
// File: mesh.hpp
// Purpose: Defines GPU mesh resources used by the rendering backend to store
//          reusable vertex and index data for ECS renderable entities.
//============================================================================

#pragma once

#include <cstdint>

namespace engine
{

struct GPUMesh
{
    std::uint32_t vao = 0;
    std::uint32_t vbo = 0;
    std::uint32_t ebo = 0;

    std::uint32_t indexCount = 0;
};

} // namespace engine