//============================================================================
// Author: Kat Moormann
// File: particle_system.hpp
// Purpose: Updates particle-specific state and visual behavior.
//============================================================================

#pragma once
namespace engine
{
    // Remember the system does not own the entity - the registry does 
    // The system must access the entity by a non owning reference
    class Registry;
    class ParticleSystem
    {
        public:
            void update(Registry &registry, double dt);
    };
} // namespace engine
