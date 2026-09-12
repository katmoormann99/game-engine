//============================================================================
// Author: Kat Moormann
// File: collision_system.cpp
// Purpose: Implements continuous sphere-vs-plane and sphere-vs-sphere
//          collision detection and reflection response.
// Date: July 10 2025
//============================================================================

#include "engine/systems/collision_system.hpp"

#include "engine/components/sphere_collider.hpp"
#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/components/projectile.hpp"


#include "geometry/vector3.hpp"

#include <array>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace engine
{

namespace
{
// Plane equation:
//      n dot p + d = 0
struct CollisionPlane
{
    cg::Vector3 normal;
    float d;
};

// x = -50 to +50
// y = -50 to +50
// z =   0 to 100
const std::array<CollisionPlane, 6> ROOM_PLANES =
{{
    // Left wall
    {cg::Vector3( 1.0f,  0.0f,  0.0f), 50.0f},

    // Right wall
    {cg::Vector3(-1.0f,  0.0f,  0.0f), 50.0f},

    // Front wall
    {cg::Vector3( 0.0f,  1.0f,  0.0f), 50.0f},

    // Back wall
    {cg::Vector3( 0.0f, -1.0f,  0.0f), 50.0f},

    // Floor
    {cg::Vector3( 0.0f,  0.0f,  1.0f), 0.0f},

    // Ceiling
    {cg::Vector3( 0.0f,  0.0f, -1.0f), 100.0f}
}};


// Stores the earliest sphere collision scheduled for one entity.
struct PendingSphereCollision
{
    bool hasCollision = false;
    float time = 0.0f;
    cg::Vector3 normal{0.0f, 0.0f, 0.0f};

    // The other entity involved in this sphere-sphere collision
    Entity otherEntity = 0;
};


// Keep only the EARLIEST sphere collision for an entity.
void scheduleSphereCollision(
    std::unordered_map<Entity, PendingSphereCollision>& pending, 
    Entity entity, 
    Entity otherEntity,
    float time, 
    const cg::Vector3& normal
)
{
    PendingSphereCollision& collision = pending[entity];

    if (!collision.hasCollision || time < collision.time)
    {
        collision.hasCollision = true;
        collision.time = time;
        collision.normal = normal;
        collision.otherEntity = otherEntity;
    }
}


// ------------------------------------------------------------
// Continuous sphere-sphere time-of-impact.
// We move A relative to B:
//
//      relativePosition = A - B
//      relativeVelocity = vA - vB
//
// Then expand B's sphere to:
//
//      radius = rA + rB
//
// and determine when the relative trajectory touches it.
// ------------------------------------------------------------
bool sphereSphereTimeOfImpact(
    const cg::Point3& centerA,
    const cg::Vector3& velocityA,
    float radiusA,

    const cg::Point3& centerB,
    const cg::Vector3& velocityB,
    float radiusB,

    float maxTime,
    float& hitTime
)
{
    cg::Vector3 relativePosition(
        centerB,
        centerA
    );

    cg::Vector3 relativeVelocity =
        velocityA - velocityB;

    const float combinedRadius =
        radiusA + radiusB;

    // Solve:
    //
    // |p + vt|^2 = R^2
    //
    // giving:
    //
    // at^2 + bt + c = 0

    const float a =
        relativeVelocity.dot(relativeVelocity);

    const float b =
        2.0f *
        relativePosition.dot(relativeVelocity);

    const float c =
        relativePosition.dot(relativePosition) -
        combinedRadius * combinedRadius;


    // No meaningful relative movement.
    if (a < 1e-8f)
    {
        return false;
    }


    const float discriminant =
        b * b -
        4.0f * a * c;

    if (discriminant < 0.0f)
    {
        return false;
    }


    const float sqrtDiscriminant =
        std::sqrt(discriminant);

    const float t0 =
        (-b - sqrtDiscriminant) /
        (2.0f * a);


    constexpr float EPS_T = 1e-5f;

    if (t0 <= EPS_T ||
        t0 > maxTime)
    {
        return false;
    }


    hitTime = t0;

    return true;
}

} // anonymous namespace


void CollisionSystem::update(Registry& registry, float dt)
{
    constexpr float EPS_T = 1e-5f;
    constexpr float NUDGE = 1e-3f;
    constexpr float EPS2 = 1e-6f;

    // transforms → actual Transform storage in Registry
    // velocities → actual Velocity storage in Registry
    // colliders  → actual SphereCollider storage in Registry
    auto& transforms = registry.transforms();
    auto& velocities = registry.velocities();
    auto& colliders = registry.sphereColliders();


    // give me a reference to the entity ID list inside the collider storage, but I promise not to modify that list
    // so entities might essentially refer to: [3, 7, 12]
    const auto& entities = colliders.entities();

    // Entities cannot be safely destroyed while we are iterating through ComponentStorage
    // So collision responses queue destruction UNTILL all collision process for this timestep has finished
    std::vector<Entity> entitiesToDestroy;
    
    auto queueDestroy = [&entitiesToDestroy](Entity entity)
    {
        if (std::find(entitiesToDestroy.begin(), entitiesToDestroy.end(), entity) == entitiesToDestroy.end())
        {
            entitiesToDestroy.push_back(entity);
        }
    };

    auto isQueuedForDestroy = [&entitiesToDestroy](Entity entity)
    {
        return std::find(
            entitiesToDestroy.begin(),
            entitiesToDestroy.end(),
            entity
        ) != entitiesToDestroy.end();
    };

    // PHASE 1: Find sphere-sphere collision events for this fixed timestep.
    std::unordered_map<Entity, PendingSphereCollision> pendingSphereCollisions;


    for (std::size_t i = 0; i < entities.size(); ++i)
    {
        Entity entityA = entities[i];
        if (!transforms.has(entityA) || !velocities.has(entityA))
        {
            continue;
        }


        for (std::size_t j = i + 1; j < entities.size(); ++j)
        {
            Entity entityB = entities[j];
            if (!transforms.has(entityB) || !velocities.has(entityB))
            {
                continue;
            }


            Transform& transformA = transforms.get(entityA);
            Transform& transformB = transforms.get(entityB);

            Velocity& velocityA = velocities.get(entityA);
            Velocity& velocityB = velocities.get(entityB);

            const SphereCollider& colliderA = colliders.get(entityA);
            const SphereCollider& colliderB = colliders.get(entityB);


            // Start-of-step overlap correction.
            //
            // If two spheres are already overlapping, separate
            // them slightly and reflect their directions.

            cg::Vector3 AB(transformA.position, transformB.position);

            const float minimumDistance = colliderA.radius + colliderB.radius;
            const float distanceSquared = AB.norm_squared();


            if (distanceSquared < minimumDistance * minimumDistance)
            {
                if (distanceSquared > EPS2)
                {
                    cg::Vector3 normal = AB;
                    normal.normalize();

                    velocityA.linear = velocityA.linear.reflect(normal);
                    velocityB.linear = velocityB.linear.reflect(normal * -1.0f);

                    // Small separation so they do not remain overlapping.
                    transformA.position = cg::Point3(
                        transformA.position.x - normal.x * NUDGE,
                        transformA.position.y - normal.y * NUDGE,
                        transformA.position.z - normal.z * NUDGE
                    );

                    transformB.position = cg::Point3(
                        transformB.position.x + normal.x * NUDGE,
                        transformB.position.y + normal.y * NUDGE,
                        transformB.position.z + normal.z * NUDGE
                    );
                }

                continue;
            }


            // Continuous sphere-sphere collision.
            float hitTime = 0.0f;

            if (!sphereSphereTimeOfImpact(
                    transformA.position,
                    velocityA.linear,
                    colliderA.radius,

                    transformB.position,
                    velocityB.linear,
                    colliderB.radius,

                    dt,
                    hitTime))
            {
                continue;
            }


            // Find where both centers will be at impact.
            cg::Point3 hitA(
                transformA.position.x +
                    velocityA.linear.x * hitTime,

                transformA.position.y +
                    velocityA.linear.y * hitTime,

                transformA.position.z +
                    velocityA.linear.z * hitTime
            );


            cg::Point3 hitB(
                transformB.position.x +
                    velocityB.linear.x * hitTime,

                transformB.position.y +
                    velocityB.linear.y * hitTime,

                transformB.position.z +
                    velocityB.linear.z * hitTime
            );


            // Contact normal from B toward A.
            cg::Vector3 normalBtoA(
                hitB,
                hitA
            );


            if (normalBtoA.norm_squared() <
                EPS2)
            {
                continue;
            }

            normalBtoA.normalize();

            // A reflects around +normal.
            // B reflects around -normal.
            scheduleSphereCollision(pendingSphereCollisions, entityA, entityB, hitTime, normalBtoA);
            scheduleSphereCollision(pendingSphereCollisions, entityB, entityA, hitTime, normalBtoA * -1.0f);
        }
    }


    // PHASE 2:
    //
    // Advance each collidable entity through this timestep.
    //
    // For every slice we choose the earliest event:
    //      sphere collision
    //              OR
    //      wall collision


    for (Entity entity : entities)
    {
        if (isQueuedForDestroy(entity))
        {
            continue;
        }

        if (!transforms.has(entity) || !velocities.has(entity))
        {
            continue;
        }

        Transform& transform = transforms.get(entity);
        Velocity& velocity = velocities.get(entity);
        const SphereCollider& collider = colliders.get(entity);
        float remainingTime = dt;

        for (int iteration = 0; iteration < 2 && remainingTime > 0.0f; ++iteration)
        {
            const cg::Vector3 v = velocity.linear;
            float bestTime = remainingTime + 1.0f;

            cg::Vector3 bestNormal(0.0f, 0.0f, 0.0f);

            enum class CollisionKind{None, Sphere, Plane};

            CollisionKind bestKind = CollisionKind::None;
            Entity bestOtherEntity = 0;

            // Check scheduled sphere collision.
            auto pendingIt = pendingSphereCollisions.find(entity);

            if (pendingIt != pendingSphereCollisions.end())
            {
                const PendingSphereCollision& pending = pendingIt->second;

                if (pending.hasCollision && pending.time > EPS_T && pending.time <= remainingTime)
                {
                    bestTime = pending.time;
                    bestNormal = pending.normal;
                    bestOtherEntity = pending.otherEntity;
                    bestKind = CollisionKind::Sphere;
                }
            }

            // Find earliest room-plane collision.
            for (const CollisionPlane& plane :  ROOM_PLANES)
            {
                const float centerDistance = 
                    plane.normal.x * transform.position.x +
                    plane.normal.y * transform.position.y +
                    plane.normal.z * transform.position.z +
                    plane.d;

                const float approachRate = plane.normal.dot(v);

                // Moving away from or parallel to wall.
                if (approachRate >= 0.0f)
                {
                    continue;
                }

                const float hitTime = (collider.radius - centerDistance) / approachRate;

                if (hitTime > EPS_T && hitTime <= remainingTime && hitTime < bestTime)
                {
                    bestTime = hitTime;
                    bestNormal = plane.normal;
                    bestKind = CollisionKind::Plane;
                }
            }

            // We found an event!!
            if (bestKind != CollisionKind::None)
            {
                // Move exactly to impact.
                transform.position = cg::Point3(
                        transform.position.x + v.x * bestTime,
                        transform.position.y + v.y * bestTime,
                        transform.position.z + v.z * bestTime
                    );

                remainingTime -= bestTime;

                // Projectile VS wall 
                // Projectiles should just dissapear when they hit a room wall instead of 
                // being reflected like the moving targets!
                // If the collision we found is with a plane AND this entity is a projectile, then do the following
                // Does projectiles_ contain Entity 5?
                // If YES -> Then entity 5 is a projectile!
                if (bestKind == CollisionKind::Plane && registry.projectiles().has(entity))
                {
                    queueDestroy(entity);

                    // stop processing the movement of this particle 
                    remainingTime = 0.0f;
                    break;
                }

                // A scheduled sphere collision should only happen once during this step 
                if (bestKind == CollisionKind::Sphere && registry.projectiles().has(entity))
                {
                    queueDestroy(entity);

                    if(bestOtherEntity != 0)
                    {
                        queueDestroy(bestOtherEntity);
                    }

                    // stop processing the movement of this particle 
                    remainingTime = 0.0f;

                    // This collision has been handled 
                    pendingSphereCollisions.erase(entity);
                    
                    break;
                }

                // Normal collision response for targets
                velocity.linear = velocity.linear.reflect(bestNormal);

                // Nudge away from contact surface 
                transform.position = cg::Point3(
                    transform.position.x + bestNormal.x * NUDGE,
                    transform.position.y + bestNormal.y * NUDGE,
                    transform.position.z + bestNormal.z * NUDGE
                );

                if (bestKind == CollisionKind::Sphere)
                {
                    pendingSphereCollisions.erase(entity);
                }
            }
            else
            {
                // No collision. Advance normally.
                transform.position =
                    cg::Point3(
                        transform.position.x + v.x * remainingTime,
                        transform.position.y + v.y * remainingTime,
                        transform.position.z + v.z * remainingTime
                    );

                remainingTime = 0.0f;
            }
        }
    }
    // DEFFERRED ENTITY DESTRUCTION
    // Collision processing is NOW finished, so it is safe to modifiy the component storages
    for (Entity entity : entitiesToDestroy)
    {
        registry.destroy(entity);
    }
}

} // namespace engine