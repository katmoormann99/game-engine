//============================================================================
// Author: Kat Moormann
// File: collision_system.hpp
// Purpose: Performs continuous sphere-vs-room collision detection and
//          response for moving ECS entities.
// Date: July 10 2025
//============================================================================

#pragma once

#include "engine/core/registry.hpp"
#include "engine/spatial/spatial_grid.hpp"
namespace engine
{

/**
 * CollisionSystem performs continuous collision detection for moving
 * entities with:
 *
 *      Transform
 *      Velocity
 *      SphereCollider
 *
 * Rather than moving an object for the entire timestep and checking whether
 * it crossed a wall afterward, the system finds the earliest collision time.
 *
 * For each fixed timestep:
 *
 *      1. Find the earliest wall collision.
 *      2. Move exactly to the impact point.
 *      3. Reflect velocity around the wall normal.
 *      4. Nudge the object slightly away from the wall.
 *      5. Simulate the remaining time in the timestep.
 */
class CollisionSystem
{
public:

    explicit CollisionSystem(float cellSize) : spatialGrid_(cellSize) {}

    void update(Registry& registry, float dt);

private: 

    SpatialGrid spatialGrid_;
};

} // namespace engine