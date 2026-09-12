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
#include "engine/components/light.hpp"
#include "engine/components/renderable.hpp"

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

    bool firePressed() const;

    void beginFrame();

    void drawMesh(
        MeshId meshId,
        const Transform& transform,
        const Material& material
    );

    void setCamera(
        const Transform& transform,
        const Camera& camera
    );

    void setLight(
        const Transform& transform,
        const Light& light
    );

    void endFrame();
    void shutdown();

private:
    bool createUnitSquareMesh();
    bool createTargetMesh();
    bool createShaderProgram();


    GPUMesh unitSquareMesh_;
    GPUMesh targetMesh_;

    std::uint32_t shaderProgram_ = 0;

    cg::Matrix4x4 view_;
    cg::Matrix4x4 projection_;

    bool firePressed_ = false;
};

} // namespace engine