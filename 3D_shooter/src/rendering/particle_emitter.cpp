//============================================================================
// Author: Kat Moormann
// File: particle_emitter.cpp
// Purpose: Implements particle emission for explosion effects.
//============================================================================

#include "engine/rendering/particle_emitter.hpp"
#include "geometry/geometry.hpp"
#include <cmath>
#include <random>

namespace engine
{

void ParticleEmitter::emitExplosion(EntityFactory& factory, const cg::Point3& position, int particleCount)
{
    static std::mt19937 rng{std::random_device{}()};

    std::uniform_real_distribution<float> zDistribution(-1.0f, 1.0f);
    std::uniform_real_distribution<float> angleDistribution(0.0f, 2.0f * cg::PI);
    std::uniform_real_distribution<float> speedDistribution(8.0f, 25.0f);
    std::uniform_real_distribution<float> lifetimeDistribution(5.0f, 15.0f);
    std::uniform_real_distribution<float> sizeDistribution(0.4f, 1.2f);

    for (int i = 0; i < particleCount; ++i)
    {
        const float z = zDistribution(rng);
        const float theta = angleDistribution(rng);
        const float radial = std::sqrt(1.0f - z * z);

        cg::Vector3 direction(radial * std::cos(theta), radial * std::sin(theta), z);

        const float speed = speedDistribution(rng);
        const cg::Vector3 velocity = direction * speed;

        const float duration = lifetimeDistribution(rng);
        const float initialSize = sizeDistribution(rng);
        const float finalSize = 0.05f;

        Material material{1.0f, 0.35f, 0.05f, 1.0f};

        factory.createParticle(position, velocity, duration, initialSize, finalSize, material);
    }
}

} // namespace engine