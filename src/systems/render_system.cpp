//============================================================================
// Author: Kat Moormann
// File: render_system.cpp
// Purpose: Implements rendering traversal for entities that contain both
//          Transform and Renderable components.
// Date: June 24 2025
//============================================================================

#include "engine/systems/render_system.hpp"
#include "engine/rendering/graphics_backend.hpp"

#include <iostream>

namespace engine
{

void RenderSystem::render(const Registry& registry, GraphicsBackend &graphics) const
{
    const auto& renderables = registry.renderables();
    const auto& transforms = registry.transforms();

    const auto& entities = renderables.entities();

    for (Entity entity : entities)
    {
        if (!transforms.has(entity))
        {
            continue;
        }

        const Renderable& renderable = renderables.get(entity);

        const Transform& transform = transforms.get(entity);

        graphics.drawMesh(
            renderable.meshId,
            transform
        );
    }
}

}