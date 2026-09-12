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
    
    const auto& transforms = registry.transforms();

    // Go through every entity that has a Light component 
    const auto& lights = registry.lights();
    for (Entity lightEntity : lights.entities())
    {
        // If this light entity does NOT have a Transform SKIP IT
        // Transform is necessary data for the placement of the light 
        if (!transforms.has(lightEntity)){
            continue;
        }
        
        const Light& light = lights.get(lightEntity);

        if (!light.active)
        {
            continue;
        }
        graphics.setLight(transforms.get(lightEntity), light);
        break;
    }

    // Go through every entity that has a Camera component 
    const auto& cameras = registry.cameras();
    for (Entity cameraEntity : cameras.entities())
    {
        if (!transforms.has(cameraEntity)){
            continue;
        }
        // If this camera entity does NOT have a Transform SKIP IT
        // Transform is necessary data for the placement of the camera 
        const Camera& camera = cameras.get(cameraEntity);

        if (!camera.active)
        {
            continue;
        }
        graphics.setCamera(transforms.get(cameraEntity), camera);
    }

    const auto& renderables = registry.renderables();
    const auto& materials = registry.materials();

    const auto& entities = renderables.entities();

    for (Entity entity : entities)
    {
        if (!transforms.has(entity) || !materials.has(entity))
        {
            continue;
        }

        const Renderable& renderable = renderables.get(entity);
        const Transform& transform = transforms.get(entity);
        const Material& material = materials.get(entity);

        graphics.drawMesh(
            renderables.get(entity).meshId,
            transforms.get(entity),
            materials.get(entity)
        );
    }
}

}