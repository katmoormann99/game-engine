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
        // A sensor needs a position in order to detect anything.
        if (!transforms.has(sensorEntity))
        {
            continue;
        }

        const Transform& sensorTransform = transforms.get(sensorEntity);

        const Sensor& sensor = sensors.get(sensorEntity);

        // Broad-phase query.
        // These are only possible detections
        // Give me all the entities in grid cells near this sensor's position, out to this sensor's detection radius
        std::vector<Entity> candidates =
            spatialGrid_.queryNearby(
                sensorTransform.position,
                sensor.detectionRadius
            );


        std::vector<Entity>& detected = detections_[sensorEntity];

        // Step 3: Perform exact distance checks on the candidates.
        const float radiusSquared = sensor.detectionRadius * sensor.detectionRadius;

        for (Entity candidate : candidates)
        {
            // Do not detect yourself.
            if (candidate == sensorEntity)
            {
                continue;
            }

            if (!transforms.has(candidate))
            {
                continue;
            }

            const Transform& targetTransform = transforms.get(candidate);

            // Point3 - Point3 gives us a Vector3.
            cg::Vector3 offset = targetTransform.position - sensorTransform.position;

            // Avoid sqrt by comparing squared distances.
            const float distanceSquared = offset.norm_squared();

            if (distanceSquared <= radiusSquared)
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