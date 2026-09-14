#pragma once

#include "engine/core/registry.hpp"
#include "engine/spatial/spatial_grid.hpp"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace engine
{

class CollisionSystem
{
public:
    explicit CollisionSystem(float cellSize, unsigned int workerCount = 8);
    ~CollisionSystem();

    void update(Registry& registry, float dt);

private:
    struct CollisionPair
    {
        Entity a = 0;
        Entity b = 0;
    };

    struct CollisionResult
    {
        Entity a = 0;
        Entity b = 0;
        bool overlapping = false;
        bool hasCollision = false;
        float hitTime = 0.0f;
        cg::Vector3 normal{0.0f, 0.0f, 0.0f};
    };

    void workerLoop(unsigned int workerIndex);

    SpatialGrid spatialGrid_;

    unsigned int workerCount_ = 8;
    std::vector<std::thread> workers_;

    std::mutex workMutex_;
    std::condition_variable workCv_;
    std::condition_variable doneCv_;

    bool stopWorkers_ = false;
    
    std::size_t workGeneration_ = 0;
    std::size_t workersFinished_ = 0;

    std::vector<CollisionPair> candidatePairs_;
    std::vector<std::vector<CollisionResult>> workerResults_;

    Registry* activeRegistry_ = nullptr;
    float activeDt_ = 0.0f;
};

} // namespace engine