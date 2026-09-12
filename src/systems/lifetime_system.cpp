//============================================================================
// Author: Kat Moormann
// File: lifetime_system.cpp
// Purpose: Implements lifetime updates and destroys entities whose
//          simulation lifetime has expired.
//============================================================================

#include "engine/systems/lifetime_system.hpp"

#include <vector>

namespace engine
{

void LifetimeSystem::update(Registry& registry, double dt)
{
    auto& lifetimes = registry.lifetimes();

    std::vector<Entity> expired;
    const auto& entities = lifetimes.entities();

    for (std::size_t i = 0; i < entities.size(); ++i)
    {
        Entity entity = entities[i];

        Lifetime& lifetime = lifetimes.get(entity);
        lifetime.remaining -= dt;

        if (lifetime.remaining <= 0.0)
        {
            expired.push_back(entity);
        }
    }

    for (Entity entity : expired)
    {
        registry.destroy(entity);
    }
}

} // namespace engine