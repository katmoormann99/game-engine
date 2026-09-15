//============================================================================
// Author: Kat Moormann
// File: sensor_system.hpp
// Purpose: Uses the spatial grid to find nearby entities and performs
//          exact distance checks for sensor detection.
// Date: June 24 2025
//============================================================================

#pragma once

#include "engine/core/registry.hpp"
#include "engine/spatial/spatial_grid.hpp"

#include <unordered_map>
#include <vector>

namespace engine
{

class SensorSystem
{
public:
    // cellSize controls the spatial grid resolution.
    explicit SensorSystem(float cellSize);

    // Rebuild the grid and update which entities each sensor detects.
    void update(Registry& registry);

    // Return the entities detected by a specific sensor entity.
    const std::vector<Entity>& detections(Entity sensorEntity) const;

private:
    // Spatial grid used for broad-phase nearby queries.
    SpatialGrid spatialGrid_;

    // Maps each sensor entity to the entities it currently detects.
    std::unordered_map<Entity, std::vector<Entity>> detections_;
};

} // namespace engine