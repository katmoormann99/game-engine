//============================================================================
// Author: Kat Moormann
// File: graphics_backend.hpp
// Purpose: Owns the SDL/OpenGL rendering backend, GPU mesh resources,
//          shader program, and submission of ECS renderable entities.
// Date: June 28 2025
//============================================================================

#pragma once

#include "engine/components/transform.hpp"
#include "engine/components/material.hpp"
#include "engine/components/camera.hpp"

#include "engine/rendering/mesh.hpp"
#include "geometry/matrix.hpp"

#include <cstdint>

namespace engine
{

class GraphicsBackend
{
public:
    GraphicsBackend() = default;
    ~GraphicsBackend() = default;

    bool initialize();
    bool handleEvents();

    void beginFrame();

    void drawMesh(
        std::uint32_t meshId,
        const Transform& transform,
        const Material& material
    );

    void endFrame();
    void shutdown();

    void setCamera(const Transform& transform, const Camera& camera);

private:

    // Creates reusable square geometry for floors, walls, and ceilings.
    bool createUnitSquareMesh();

    // Loads, compiles, and links the vertex/fragment shader program.
    bool createShaderProgram();

    // Shared GPU geometry.
    GPUMesh unitSquareMesh_;

    // Linked OpenGL shader program.
    std::uint32_t shaderProgram_ = 0;

    cg::Matrix4x4 view_;
    cg::Matrix4x4 projection_;
};

} // namespace engine