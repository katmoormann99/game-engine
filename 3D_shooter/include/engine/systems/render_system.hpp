//============================================================================
// Author: Kat Moormann
// File: render_system.hpp
// Purpose: Reads entity transform and renderable components and prepares
//          visible entities for rendering.
// Date: June 24 2025
//============================================================================

#pragma once

#include "engine/core/registry.hpp"

namespace engine
{

class GraphicsBackend;
class RenderSystem
{
public:
    void render(const Registry& registry, GraphicsBackend& graphics) const;

private: 
    mutable bool firstRender_ = true; // debugging purposes
};

} // namespace engine