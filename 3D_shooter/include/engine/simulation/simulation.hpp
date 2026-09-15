//============================================================================
// Author: Kat Moormann
// File: simulation.hpp
// Purpose: Defines the simulation runtime responsible for fixed-timestep
//          advancement and ordered execution of simulation systems.
//============================================================================

#pragma once

#include "engine/core/registry.hpp"
#include "engine/core/time.hpp"
#include "engine/systems/movement_system.hpp"
#include "engine/systems/lifetime_system.hpp"
#include "engine/systems/sensor_system.hpp"
#include "engine/systems/collision_system.hpp"
#include "engine/systems/weapon_system.hpp"

namespace engine
{

class Simulation
{
public:
    explicit Simulation(double fixed_dt = 1.0 / 60.0, float spatialCellSize = 100.0f);

    Registry& registry();
    const Registry& registry() const;

    const SimulationTime& time() const;

    const std::vector<Entity> &detections(Entity sensorEntity) const;

    void update(double frame_dt);

    Entity fireWeapon(EntityFactory& factory, Entity weaponEntity, Entity owner, const cg::Point3& position, const cg::Vector3& direction);

private:
    void step();

    Registry registry_;
    SimulationTime time_;

    MovementSystem movementSystem_;
    LifetimeSystem lifetimeSystem_;
    SensorSystem sensorSystem_;
    CollisionSystem collisionSystem_;
    WeaponSystem weaponSystem_;

    double accumulator_ = 0.0;
};

} // namespace engine