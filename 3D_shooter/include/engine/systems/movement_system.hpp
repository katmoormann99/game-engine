//============================================================================
// Author: Kat Moormann 
// File: movement_system.hpp
// Purpose: Updates entity positions from velocity components during each fixed simulation time step
// Date: June 24 2025
//============================================================================

#pragma once 

#include "engine/core/registry.hpp"

namespace engine{

    class MovementSystem
    {
        public:
        void update(Registry &registry, double dt);
    };
}