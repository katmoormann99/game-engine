//============================================================================
// Author: Kat Moormann
// File: lifetime_system.hpp
// Purpose: Updates entity lifetime state and removes expired entities
//          from the simulation.
//============================================================================

#pragma once

#include "engine/core/registry.hpp"

namespace engine
{

class LifetimeSystem
{
public:
    void update(Registry& registry, double dt);
};

} // namespace engine