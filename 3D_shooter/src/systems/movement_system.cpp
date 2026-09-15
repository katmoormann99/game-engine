//============================================================================
// Author: Kat Moormann 
// File: movement_system.cpp
// Purpose: Updates entity positions from velocity components during each fixed simulation time step
// Date: June 24 2025
//============================================================================


#include "engine/core/registry.hpp"
#include "engine/systems/movement_system.hpp"

namespace engine{

    void MovementSystem::update(Registry &registry, double dt)
    {
        auto &transforms = registry.transforms();
        auto &velocities = registry.velocities();
        auto &colliders = registry.sphereColliders();

        const auto &entities = velocities.entities();

        // Loop through every entity that has a velocity component 
        for (std::size_t i = 0; i < entities.size(); i++)
        {
            Entity entity = entities[i];

            if (colliders.has(entity)){
                // The CollisionSystem also moves collidable entites
                // if both the movementSystem and CollisionSystem move the entity the entity would move twice = not wanted behavior 
                continue;
            }

            if (!transforms.has(entity))
            {
                // If this entity does not have a transform skip this entity 
                continue;
            }

            // At this point we know the entity has both a Velocity and Transform component
            Transform &transform = transforms.get(entity);
            Velocity &velocity = velocities.get(entity);

            // new position = old position + (velocity * elapsed time)
            transform.position = transform.position + velocity.linear * static_cast<float>(dt);
        }

    }
}