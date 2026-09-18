#include "engine/systems/particle_system.hpp"
#include "engine/core/registry.hpp"

#include <algorithm>
namespace engine
{

void ParticleSystem::update(Registry& registry, double dt)
{
    auto& particles = registry.particles();
    auto& transforms = registry.transforms();
    auto& materials = registry.materials();

    const auto& entities = particles.entities();

    for (std::size_t i = 0; i < entities.size(); ++i)
    {
        const Entity entity = entities[i];
        Particle& particle = particles.get(entity);
        particle.age += static_cast<float>(dt);

        // Visual particle behavior requires these components.
        if (!transforms.has(entity) || !materials.has(entity)) {continue;}
        Transform& transform = transforms.get(entity);
        Material& material = materials.get(entity);

        float t = 1.0f;

        if (particle.duration > 0.0f)
        {
            t = particle.age / particle.duration;
        }

        t = std::clamp(t, 0.0f, 1.0f);

        const float size = particle.initialSize + t * (particle.finalSize - particle.initialSize);
        transform.scale = cg::Vector3(size, size, size);
        material.a = particle.initialAlpha + t * (particle.finalAlpha - particle.initialAlpha);
    }
}

} // namespace engine