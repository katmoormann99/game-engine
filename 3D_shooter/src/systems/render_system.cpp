//============================================================================
// Author: Kat Moormann
// File: render_system.cpp
// Purpose: Renders ECS entities, particle effects, and scene graphics.
// Date: June 24 2025
//============================================================================

#include "engine/systems/render_system.hpp"
#include "engine/rendering/graphics_backend.hpp"

#include <iostream>

namespace engine
{

void RenderSystem::render(const Registry& registry, GraphicsBackend& graphics) const
{
    const auto& transforms = registry.transforms();

    // Set the active light.
    const auto& lights = registry.lights();

    for (Entity lightEntity : lights.entities())
    {
        if (!transforms.has(lightEntity)) { continue; }

        const Light& light = lights.get(lightEntity);

        if (!light.active) { continue; }

        if (firstRender_) { std::cout << "[RENDER] Using Light Entity " << lightEntity << '\n'; }

        graphics.setLight(transforms.get(lightEntity), light);
        break;
    }

    // Set the active camera and draw scene boundaries.
    const auto& cameras = registry.cameras();

    for (Entity cameraEntity : cameras.entities())
    {
        if (!transforms.has(cameraEntity)) { continue; }

        const Camera& camera = cameras.get(cameraEntity);

        if (!camera.active) { continue; }

        if (firstRender_) { std::cout << "[RENDER] Using Camera Entity " << cameraEntity << '\n'; }

        graphics.setCamera(transforms.get(cameraEntity), camera);
        graphics.drawSkybox();
        graphics.drawArenaBox();
        break;
    }

    // Draw standard ECS entities.
    const auto& renderables = registry.renderables();
    const auto& materials = registry.materials();

    for (Entity entity : renderables.entities())
    {
        if (!transforms.has(entity) || !materials.has(entity)) { continue; }

        const Renderable& renderable = renderables.get(entity);
        const Transform& transform = transforms.get(entity);
        const Material& material = materials.get(entity);

        if (firstRender_) { std::cout << "[RENDER] Drawing Entity " << entity << " | MeshId " << static_cast<int>(renderable.meshId) << '\n'; }

        graphics.drawMesh(renderable.meshId, transform, material);
    }

    // Draw particle entities using the dedicated particle rendering path.
    const auto& particles = registry.particles();

    for (Entity entity : particles.entities())
    {
        if (!transforms.has(entity) || !materials.has(entity)) { continue; }

        const Transform& transform = transforms.get(entity);
        const Material& material = materials.get(entity);

        if (firstRender_) { std::cout << "[RENDER] Drawing Particle Entity " << entity << '\n'; }

        graphics.drawParticle(transform, material);
    }

    if (firstRender_)
    {
        std::cout << "[RENDER] Render traversal complete\n";
        firstRender_ = false;
    }
}

} // namespace engine