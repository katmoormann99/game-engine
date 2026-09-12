//============================================================================
// Author: Kat Moormann
// File: graphics_backend.hpp
// Purpose: Defines the graphics backend interface responsible for window
//          management, frame setup, and submitting mesh draw calls to OpenGL.
// Date: June 24 2025
//============================================================================

#pragma once

#include "engine/components/transform.hpp"

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
        const Transform& transform
    );

    void endFrame();
    void shutdown();

private:
    // Create the first test triangle and upload it to the GPU.
    bool createTestMesh();

    // Create the minimal shader program used to draw the test mesh.
    bool createShaderProgram();

    // OpenGL object IDs.
    std::uint32_t vao_ = 0;
    std::uint32_t vbo_ = 0;
    std::uint32_t shaderProgram_ = 0;
};

} // namespace engine