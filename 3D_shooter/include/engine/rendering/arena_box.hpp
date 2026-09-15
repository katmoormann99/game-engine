//============================================================================
// Author: Kat Moormann
// File: arena_box.hpp
// Purpose: Defines a lightweight wireframe arena renderer that visualizes the
//          3D collision volume using OpenGL line geometry.
// Date: August 05 2025
//============================================================================

#pragma once

#include <cstdint>

namespace engine
{

class ArenaBox
{
public:
    bool initialize();

    void draw(const float* view, const float* projection);

    void shutdown();

private:
    std::uint32_t vao_ = 0;
    std::uint32_t vbo_ = 0;
    std::uint32_t ebo_ = 0;
    std::uint32_t shaderProgram_ = 0;
};

} // namespace engine