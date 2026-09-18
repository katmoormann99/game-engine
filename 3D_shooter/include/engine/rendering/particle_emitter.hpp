//============================================================================
// Author: Kat Moormann
// File: particle_emitter.hpp
// Purpose: Spawns groups of particles for transient visual effects.
//============================================================================

#pragma once

#include "engine/factory/entity_factory.hpp"

namespace engine
{

class ParticleEmitter
{
public:
    void emitExplosion(EntityFactory& factory, const cg::Point3& position, int particleCount = 75);
};

} // namespace engine