//============================================================================
// Author: Kat Moormann
// File: collision_system.cpp
// Purpose: Implements continuous sphere-vs-plane and sphere-vs-sphere
//          collision detection and reflection response.
// Date: July 10 2025
//============================================================================

#include "engine/systems/collision_system.hpp"
#include "engine/events/impact_event.hpp"
#include "engine/components/sphere_collider.hpp"
#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/components/projectile.hpp"

#include "geometry/vector3.hpp"

#include <utility>
#include <array>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <thread>
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
// y = -30 to +30
// z =   0 to 100
const std::array<CollisionPlane, 6> ROOM_PLANES =
{{
    {cg::Vector3( 1.0f,  0.0f,  0.0f), 50.0f},
    {cg::Vector3(-1.0f,  0.0f,  0.0f), 50.0f},

    {cg::Vector3( 0.0f,  1.0f,  0.0f), 30.0f},
    {cg::Vector3( 0.0f, -1.0f,  0.0f), 30.0f},

    {cg::Vector3( 0.0f,  0.0f,  1.0f), 0.0f},
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
    cg::Vector3 relativePosition(centerB, centerA);

    cg::Vector3 relativeVelocity = velocityA - velocityB;

    const float combinedRadius = radiusA + radiusB;

    // Solve:
    //
    // |p + vt|^2 = R^2
    //
    // giving:
    //
    // at^2 + bt + c = 0

    const float a = relativeVelocity.dot(relativeVelocity);
    const float b = 2.0f * relativePosition.dot(relativeVelocity);
    const float c = relativePosition.dot(relativePosition) - combinedRadius * combinedRadius;

    // No meaningful relative movement.
    if (a < 1e-8f)
    {
        return false;
    }

    const float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f)
    {
        return false;
    }

    const float sqrtDiscriminant = std::sqrt(discriminant);
    const float t0 = (-b - sqrtDiscriminant) / (2.0f * a);

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

std::vector<ImpactEvent> CollisionSystem::consumeImpactEvents()
{
    std::vector<ImpactEvent> events = std::move(impactEvents_);
    impactEvents_.clear();
    return events;
}


CollisionSystem::CollisionSystem(float cellSize, unsigned int workerCount)
    : spatialGrid_(cellSize), workerCount_(workerCount)
{
    // Give each worker its own nearby-entity list "scratchpad"
    workerNearby_.resize(workerCount_);

    // Give each workers its own candidate-pair list "scratchpad"
    workerCandidatePairs_.resize(workerCount_);

    // Give each worker its own collision-result list "scratchpad"
    workerResults_.resize(workerCount_);

    // Reserve space for up to 128 nearby entities per worker
    for (auto &nearby : workerNearby_) {nearby.reserve(128);}

    // Preallocates space for each worker thread objects 
    workers_.reserve(workerCount_);

    // Create each worker thread and have it run workerLoop()
    for (unsigned int i = 0; i < workerCount_; ++i)
    {
        // this = a pointer to the current CollisionSystem object 
        workers_.emplace_back(&CollisionSystem::workerLoop, this, i);
    }
}

CollisionSystem::~CollisionSystem()
{
    {
        std::lock_guard<std::mutex> lock(workMutex_);
        stopWorkers_ = true;
    }

    workCv_.notify_all();

    for (std::thread& worker : workers_)
    {
        if (worker.joinable()) { worker.join(); }
    }
}

// THIS IS THE MAIN FUNCTION THAT EACH WORKER THREAD RUNS CONTINUOUSLY
void CollisionSystem::workerLoop(unsigned int workerIndex)
{
    // Tracks the last batch of work this worker processed 
    std::size_t lastGeneration = 0;

    // Keep the worker alive until the CollisionSystem shuts down
    while (true)
    {
        // Lock shared worker state before checking for new work 
        std::unique_lock<std::mutex> lock(workMutex_);

        // Sleep until there is new work or the worker needs to stop
        workCv_.wait(lock, [this, lastGeneration]()
        {
            return stopWorkers_ || workGeneration_ > lastGeneration;
        });

        if (stopWorkers_) { return; }

        // Remember which batch of work this worker is processing
        lastGeneration = workGeneration_;

        // Get access to the Registry being used for this job
        Registry& registry = *activeRegistry_;

        const float dt = activeDt_;
        const WorkType workType = activeWorkType_;

        // Get access to the necessary component data necessary for the physics
        auto& transforms = registry.transforms();
        auto& velocities = registry.velocities();
        auto& colliders = registry.sphereColliders();

        // RELEASE THE MUTEX SO ALL WORKERS CAN DO COLLISION WORK IN PARALLEL
        lock.unlock();

        if (workType == WorkType::BroadPhase)
        {
            const auto& entities = colliders.entities();

            const std::size_t chunkSize = (entities.size() + workerCount_ - 1) / workerCount_;
            const std::size_t begin = workerIndex * chunkSize;
            const std::size_t end = std::min(begin + chunkSize, entities.size());

            auto& nearby = workerNearby_[workerIndex];
            auto& pairs = workerCandidatePairs_[workerIndex];

            pairs.clear();

            for (std::size_t i = begin; i < end; ++i)
            {
                const Entity entityA = entities[i];

                if (!transforms.has(entityA) || !velocities.has(entityA)) { continue; }

                const Transform& transformA = transforms.get(entityA);
                const Velocity& velocityA = velocities.get(entityA);
                const SphereCollider& colliderA = colliders.get(entityA);

                const float movementDistance = velocityA.linear.norm() * dt;
                const float searchRadius = colliderA.radius * 2.0f + movementDistance;

                spatialGrid_.queryNearby(transformA.position, searchRadius, nearby);

                for (Entity entityB : nearby)
                {
                    if (entityB <= entityA) { continue; }
                    if (!transforms.has(entityB) || !velocities.has(entityB) || !colliders.has(entityB)) { continue; }

                    pairs.push_back({entityA, entityB});
                }
            }
        }

        else if (workType == WorkType::NarrowPhase)
        {
            const std::size_t chunkSize = (candidatePairs_.size() + workerCount_ - 1) / workerCount_;
            const std::size_t begin = workerIndex * chunkSize;
            const std::size_t end = std::min(begin + chunkSize, candidatePairs_.size());

            auto& results = workerResults_[workerIndex];
            results.clear();

            for (std::size_t i = begin; i < end; ++i)
            {
                const CollisionPair& pair = candidatePairs_[i];

                const Transform& transformA = transforms.get(pair.a);
                const Transform& transformB = transforms.get(pair.b);
                const Velocity& velocityA = velocities.get(pair.a);
                const Velocity& velocityB = velocities.get(pair.b);
                const SphereCollider& colliderA = colliders.get(pair.a);
                const SphereCollider& colliderB = colliders.get(pair.b);

                cg::Vector3 AB(transformA.position, transformB.position);

                const float minimumDistance = colliderA.radius + colliderB.radius;
                const float distanceSquared = AB.norm_squared();

                if (distanceSquared < minimumDistance * minimumDistance)
                {
                    if (distanceSquared > 1e-6f)
                    {
                        AB.normalize();
                        results.push_back({pair.a, pair.b, true, false, 0.0f, AB});
                    }

                    continue;
                }

                float hitTime = 0.0f;

                if (!sphereSphereTimeOfImpact(transformA.position, velocityA.linear, colliderA.radius, transformB.position, velocityB.linear, colliderB.radius, dt, hitTime)) { continue; }

                const cg::Point3 hitA(transformA.position.x + velocityA.linear.x * hitTime, transformA.position.y + velocityA.linear.y * hitTime, transformA.position.z + velocityA.linear.z * hitTime);
                const cg::Point3 hitB(transformB.position.x + velocityB.linear.x * hitTime, transformB.position.y + velocityB.linear.y * hitTime, transformB.position.z + velocityB.linear.z * hitTime);

                cg::Vector3 normalBtoA(hitB, hitA);

                if (normalBtoA.norm_squared() < 1e-6f) { continue; }

                normalBtoA.normalize();
                results.push_back({pair.a, pair.b, false, true, hitTime, normalBtoA});
            }
        }

        // Lock the shared state again before changing the finished worker 
        lock.lock();

        // Mark this worker as finished with the current batch
        ++workersFinished_;

        // Wake the main thread when the last worker finishes 
        if (workersFinished_ == workerCount_) { doneCv_.notify_one(); }
    }
}

// Gives work to all worker threads and waits for them to finish
void CollisionSystem::runWorkers(WorkType workType, Registry &registry, float dt)
{
    {
        // Lock shared worker data while setting up the new work 
        std::lock_guard<std::mutex> lock(workMutex_);

        // Give workers access to the Registry for component data
        activeRegistry_ = &registry;

        // Give workers the timestep for collision calculations
        activeDt_ = dt;

        // Tell workers which collision phase to perform
        activeWorkType_ = workType;

        // Reset the number of workers that have finished
        workersFinished_ = 0;

        // Mark that a new batch of work is ready
        ++workGeneration_;

    }

    // WAKE UP THE WORKER THREADS SO THAT THEY CAN START WORKING
    workCv_.notify_all();

    // Lock the mutex while waiting for the workers to finish
    std::unique_lock<std::mutex> lock(workMutex_);

    // wait until every worker has finished its work
    doneCv_.wait(lock, [this]()
    {
        return workersFinished_ == workerCount_;
    });
}

void CollisionSystem::update(Registry& registry, float dt)
{
    const auto collisionStart = std::chrono::steady_clock::now();

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
    

    // Create a function called queueDestroy that can access and modify entitiesToDestroy. Give 
    // it an entity. If that entity is NOT already in entities to destroy, add it to the end of the list

    // So for example, queueDestroy(projectile) means "mark the projectile to be detroyed later, unless it is already marked"
    
    // == entitiesToDestroy.end() MEANS I did NOT find it
    // Lambda function = [capture][parameter]
    auto queueDestroy = [&entitiesToDestroy](Entity entity)
    {
        if (std::find(entitiesToDestroy.begin(), entitiesToDestroy.end(), entity) == entitiesToDestroy.end())
        {
            entitiesToDestroy.push_back(entity);
        }
    };

    // Give this function an entity, search the entitiesToDestroy list, and return TRUE if the entity is in the list; otherwise return false
    // If tihs entity is already queued to be destroyed, skip it

    // != entitiesToDestory.ends() MEANS I DID find it
    // Lambda function = [capture][parameter]
    auto isQueuedForDestroy = [&entitiesToDestroy](Entity entity)
    {
        return std::find(entitiesToDestroy.begin(), entitiesToDestroy.end(), entity) != entitiesToDestroy.end();
    };


    // PHASE 1: Find sphere-sphere collision events.
    std::unordered_map<Entity, PendingSphereCollision> pendingSphereCollisions;

    const auto broadPhaseStart = std::chrono::steady_clock::now();

    spatialGrid_.clear();
    candidatePairs_.clear();

    const auto phase1Start = std::chrono::steady_clock::now();

    const auto gridBuildStart = std::chrono::steady_clock::now();

    for (Entity entity : entities)
    {
        if (transforms.has(entity)) { spatialGrid_.insert(entity, transforms.get(entity).position); }
    }

    const auto gridBuildEnd = std::chrono::steady_clock::now();
    
    const auto candidateStart = std::chrono::steady_clock::now();

    // Parallel broad phase.
    runWorkers(WorkType::BroadPhase, registry, dt);

    // Merge worker-local candidate lists.
    candidatePairs_.clear();
    std::size_t totalCandidatePairs = 0;

    for (const auto& pairs : workerCandidatePairs_) 
    { 
        totalCandidatePairs += pairs.size(); 
    }

    candidatePairs_.reserve(totalCandidatePairs);
    for (const auto& pairs : workerCandidatePairs_)
    {
        candidatePairs_.insert(candidatePairs_.end(), pairs.begin(), pairs.end());
    }

    const auto candidateEnd = std::chrono::steady_clock::now();
    const auto broadPhaseEnd = std::chrono::steady_clock::now();

    const auto narrowPhaseStart = std::chrono::steady_clock::now();

    // Parallel narrow phase.
    runWorkers(WorkType::NarrowPhase, registry, dt);

    const auto narrowPhaseEnd = std::chrono::steady_clock::now();
    const auto mergeStart = std::chrono::steady_clock::now();

    // Apply worker results on the main thread.
    for (const auto& results : workerResults_)
    {
        for (const CollisionResult& result : results)
        {
            if (!transforms.has(result.a) || !transforms.has(result.b)) { continue; }
            if (!velocities.has(result.a) || !velocities.has(result.b)) { continue; }

            Transform& transformA = transforms.get(result.a);
            Transform& transformB = transforms.get(result.b);
            Velocity& velocityA = velocities.get(result.a);
            Velocity& velocityB = velocities.get(result.b);

            if (result.overlapping)
            {
                velocityA.linear = velocityA.linear.reflect(result.normal);
                velocityB.linear = velocityB.linear.reflect(result.normal * -1.0f);

                transformA.position = cg::Point3(transformA.position.x - result.normal.x * NUDGE, transformA.position.y - result.normal.y * NUDGE, transformA.position.z - result.normal.z * NUDGE);
                transformB.position = cg::Point3(transformB.position.x + result.normal.x * NUDGE, transformB.position.y + result.normal.y * NUDGE, transformB.position.z + result.normal.z * NUDGE);

                continue;
            }

            if (result.hasCollision)
            {
                scheduleSphereCollision(pendingSphereCollisions, result.a, result.b, result.hitTime, result.normal);
                scheduleSphereCollision(pendingSphereCollisions, result.b, result.a, result.hitTime, result.normal * -1.0f);
            }
        }
    }
    const auto mergeEnd = std::chrono::steady_clock::now();


    const auto phase1End = std::chrono::steady_clock::now();
    const auto phase2Start = std::chrono::steady_clock::now();

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

                // Distance between the outside of the sphere and the wall
                // Positive = sphere is still inside the room
                // Zero     = sphere is touching the wall
                // Negative = sphere is penetrating/outside the wall
                const float seperation = centerDistance - collider.radius;

                // If we are already touching or slightly penetrating the wall
                // AND moving toward/out through it, handle the collision immediately 
                constexpr float CONTACT_EPS = 1e-4f;
                if (seperation <= CONTACT_EPS)
                {
                    if (0.0f < bestTime)
                    {
                        bestTime = 0.0f;
                        bestNormal = plane.normal;
                        bestKind = CollisionKind::Plane;
                    }
                    continue;
                }

                // otherwise calculate when the sphere will reach the wall
                const float hitTime = (collider.radius - centerDistance) / approachRate;


                if (hitTime > 0.0f && hitTime <= remainingTime && hitTime < bestTime)
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

                // If the collision was with another sphere AND the current entity is a projectile AND 
                // the other entity that it hit is a target, then destroy both
                if (bestKind == CollisionKind::Sphere && registry.projectiles().has(entity) && registry.targets().has(bestOtherEntity))
                {
                    std::cout << "[HIT HIT HIT] PROJECTILE ENTITY " << entity << " HIT TARGET ENTITY " << bestOtherEntity << std::endl;

                    impactEvents_.push_back(ImpactEvent{entity, bestOtherEntity, transform.position});

                    queueDestroy(entity);
                    queueDestroy(bestOtherEntity);

                    remainingTime = 0.0f;
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
    const auto phase2End = std::chrono::steady_clock::now();

    const std::chrono::duration<double, std::milli> phase1Ms = phase1End - phase1Start;
    const std::chrono::duration<double, std::milli> phase2Ms = phase2End - phase2Start;
    const std::chrono::duration<double, std::milli> collisionMs = phase2End - collisionStart;

    const std::chrono::duration<double, std::milli> broadPhaseMs = broadPhaseEnd - broadPhaseStart;
    const std::chrono::duration<double, std::milli> narrowPhaseMs = narrowPhaseEnd - narrowPhaseStart;
    const std::chrono::duration<double, std::milli> mergeMs = mergeEnd - mergeStart;
    const std::chrono::duration<double, std::milli> gridBuildMs = gridBuildEnd - gridBuildStart;
    const std::chrono::duration<double, std::milli> candidateMs = candidateEnd - candidateStart;

    static int profileCounter = 0;

    if (++profileCounter >= 60)
    {
        std::cout << "\n[COLLISION PROFILE]\n";
        std::cout << "Broad phase:  " << broadPhaseMs.count() << " ms\n";
        std::cout << "Narrow phase: " << narrowPhaseMs.count() << " ms\n";
        std::cout << "Merge:        " << mergeMs.count() << " ms\n";

        std::cout << "\nGrid build:    " << gridBuildMs.count() << " ms\n";
        std::cout << "Candidate gen: " << candidateMs.count() << " ms\n";

        std::cout << "Candidate pairs: " << totalCandidatePairs << '\n';

        std::cout << "\nPhase 1:      " << phase1Ms.count() << " ms\n";
        std::cout << "Phase 2:      " << phase2Ms.count() << " ms\n";
        std::cout << "Total:        " << collisionMs.count() << " ms\n\n";

        profileCounter = 0;
    }

    for (Entity entity : registry.targets().entities())
    {
        if (!transforms.has(entity) ||
            !velocities.has(entity) ||
            !colliders.has(entity))
        {
            continue;
        }

        const Transform& transform = transforms.get(entity);
        const Velocity& velocity = velocities.get(entity);
        const SphereCollider& collider = colliders.get(entity);

        const float r = collider.radius;

        const bool outOfBounds =
            transform.position.x < -50.0f + r ||
            transform.position.x >  50.0f - r ||
            transform.position.y < -30.0f + r ||
            transform.position.y >  30.0f - r ||
            transform.position.z <   0.0f + r ||
            transform.position.z > 100.0f - r;

        if (outOfBounds)
        {
            std::cout
                << "\n[OUT OF BOUNDS]\n"
                << "Entity: " << entity << '\n'
                << "Position: ("
                << transform.position.x << ", "
                << transform.position.y << ", "
                << transform.position.z << ")\n"
                << "Velocity: ("
                << velocity.linear.x << ", "
                << velocity.linear.y << ", "
                << velocity.linear.z << ")\n"
                << "Radius: " << r
                << "\n\n";
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