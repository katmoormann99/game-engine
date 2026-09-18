#pragma once

#include "engine/core/registry.hpp"
#include "engine/spatial/spatial_grid.hpp"

#include "engine/events/impact_event.hpp"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace engine
{

// Detects and handles collisions using a spatial grid and worker threads
class CollisionSystem
{
public:
    // Creates the spatial grid and starts the collision worker threads
    explicit CollisionSystem(float cellSize, unsigned int workerCount = 8);

    // Stops and joins all worker threads
    ~CollisionSystem();

    // Detects and handles collisions for one simulation timestep
    void update(Registry& registry, float dt);
     
    std::vector<ImpactEvent> consumeImpactEvents();

private:
    std::vector<ImpactEvent> impactEvents_;

    // Identifies which collision phase the workers should perform
    enum class WorkType
    {
        BroadPhase,
        NarrowPhase
    };

    // Stores two entities that may collide 
    struct CollisionPair
    {
        Entity a = 0;
        Entity b = 0;
    };

    // Stores the collision information calculated by a worker
    struct CollisionResult
    {
        Entity a = 0;
        Entity b = 0;
        bool overlapping = false;
        bool hasCollision = false;
        float hitTime = 0.0f;
        cg::Vector3 normal{0.0f, 0.0f, 0.0f};
    };

    // Waits for work and processes the portion assigned to this worker
    void workerLoop(unsigned int workerIndex);

    // Must pass registry by reference because the worker threads need to read the 
    // ECS component data while doing their work 
    // Starts the collision phase and waits until all workers finish 
    void runWorkers(WorkType workType, Registry &registry, float dt);

    // Tells workers which collision phase is currently running
    WorkType activeWorkType_ = WorkType::BroadPhase;

    // Stores nearby entities found by each worker
    std::vector<std::vector<Entity>> workerNearby_;
    // Stores candidate collision pairs found by each worker
    std::vector<std::vector<CollisionPair>> workerCandidatePairs_;
    // Stores the collision results produced by each worker
    std::vector<std::vector<CollisionResult>> workerResults_;

    // Organizes entities into cells so nearby objects can be found quickly 
    SpatialGrid spatialGrid_;

    // Number of worker threads used for collision detection
    unsigned int workerCount_ = 8;

    // Persistent threads that perform collision work
    std::vector<std::thread> workers_;

    // Protects shared worker state
    std::mutex workMutex_;

    // Wakes workers when new collision work is ready
    std::condition_variable workCv_;
    // Wakes the main thread when all workers are finished 
    std::condition_variable doneCv_;

    // Tells worker threads to exit during shutdown
    bool stopWorkers_ = false;
    
    // Increases each time a new batch of work is started 
    std::size_t workGeneration_ = 0;

    // Counts how many workers finished the current batch
    std::size_t workersFinished_ = 0;

    // Stores sphere pairs that need precise collision testing 
    std::vector<CollisionPair> candidatePairs_;

    // THe registry currently being read by the worker threads 
    Registry* activeRegistry_ = nullptr;

    // Time step currently being processed by the worker threads 
    float activeDt_ = 0.0f;
};

} // namespace engine