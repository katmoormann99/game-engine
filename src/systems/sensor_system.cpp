//============================================================================
// Author: Kat Moormann
// File: sensor_system.cpp
// Purpose: Implements sensor detection using broad-phase spatial queries
//          followed by exact distance checks.
// Date: June 24 2025
//============================================================================

#include "engine/systems/sensor_system.hpp"

namespace engine
{

SensorSystem::SensorSystem(float cellSize)
    : spatialGrid_(cellSize)
{
}


void SensorSystem::update(Registry& registry)
{
    spatialGrid_.clear();
    detections_.clear();

    auto& transforms = registry.transforms();
    auto& sensors = registry.sensors();

    // Step 1: Insert all positioned entities into the spatial grid.
    const auto& transformEntities = transforms.entities();

    for (Entity entity : transformEntities)
    {
        const Transform& transform = transforms.get(entity);

        spatialGrid_.insert(
            entity,
            transform.position
        );
    }


    // Step 2: Process every entity that has a Sensor component
    const auto& sensorEntities = sensors.entities();

    for (Entity sensorEntity : sensorEntities)
    {
        if (!transforms.has(sensorEntity)) { continue; }

        const Transform& sensorTransform = transforms.get(sensorEntity);
        const Sensor& sensor = sensors.get(sensorEntity);

        std::vector<Entity> nearby;
        nearby.reserve(128);

        spatialGrid_.queryNearby(sensorTransform.position, sensor.detectionRadius, nearby);

        std::vector<Entity>& detected = detections_[sensorEntity];
        detected.clear();

        const float radiusSquared = sensor.detectionRadius * sensor.detectionRadius;

        for (Entity candidate : nearby)
        {
            if (candidate == sensorEntity) { continue; }
            if (!transforms.has(candidate)) { continue; }

            const Transform& candidateTransform = transforms.get(candidate);

            cg::Vector3 offset(sensorTransform.position, candidateTransform.position);

            if (offset.norm_squared() <= radiusSquared)
            {
                detected.push_back(candidate);
            }
        }
    }
}


const std::vector<Entity>& SensorSystem::detections(Entity sensorEntity) const
{
    // Shared empty result for entities with no detections.
    static const std::vector<Entity> empty;

    auto it = detections_.find(sensorEntity);

    if (it == detections_.end())
    {
        return empty;
    }

    return it->second;
}

} // namespace engine