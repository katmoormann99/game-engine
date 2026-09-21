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
#include "engine/rendering/skybox.hpp"
#include "engine/rendering/arena_box.hpp"
#include "engine/rendering/mesh.hpp"
#include "engine/rendering/cross_hair.hpp"
#include "engine/rendering/shader.hpp"
#include "geometry/matrix.hpp"

#include "geometry/vector3.hpp"
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
    void drawMesh(MeshId meshId, const Transform& transform, const Material& material);
    void drawSkybox();
    void drawArenaBox();
    void drawParticle(const Transform& transform, const Material &material);
    void drawCrosshair();

    void setCamera(const Transform& transform, const Camera& camera);
    void setLight(const Transform& transform, const Light& light);
    void endFrame();
    void shutdown();

    cg::Vector3 aimDirection() const;

private:
    bool createUnitSquareMesh();
    bool createTargetMesh();

    GPUMesh unitSquareMesh_;
    GPUMesh targetMesh_;
    Shader shader_;

    Shader particleShader_;

    cg::Matrix4x4 view_;
    cg::Matrix4x4 projection_;

    float mouseX_ = 400.0f;
    float mouseY_ = 400.0f;

    int windowWidth_ = 800;
    int windowHeight_ = 800;

    bool firePressed_ = false;

    Skybox skybox_;
    ArenaBox arenaBox_;
    Crosshair crosshair_;
};

} // namespace engine