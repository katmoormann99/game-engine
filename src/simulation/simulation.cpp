//============================================================================
// Author: Kat Moormann 
// File: simulation.cpp
// Purpose: Coordinates the simulation loop, including fixed-timestep advancement and ordered
// execution of simulated systems
// Date: June 24 2025
//============================================================================

#include "engine/simulation/simulation.hpp"

namespace engine
{

    // Constructor: defaults simulation to 60 updates per second
    Simulation::Simulation(double fixed_dt, float spatialCellSize) : sensorSystem_(spatialCellSize)
    {
        time_.fixed_dt = fixed_dt;
    }

    // Gives outside code access to the Registry so it can create entitites and add/change their components
    // Example: simulation.registry().create();
    Registry& Simulation::registry()
    {
        return registry_;

    }

    const std::vector<Entity> &Simulation::detections(Entity sensorEntity) const
    {
        return sensorSystem_.detections(sensorEntity);
    }

    // Read-only access to the Registry for code that only needs to inspect entities/components without changing them
    const Registry& Simulation::registry() const
    {
        return registry_;
    }

    // Read-only access to the current simulation timing state
    const SimulationTime& Simulation::time() const
    {
        return time_;
    }

    void Simulation::update(double frame_dt)
    {
        // Collect real time until enough has passed for a simulation step
        accumulator_ += frame_dt;

        while(accumulator_ >= time_.fixed_dt)
        {
            step();
            accumulator_ -= time_.fixed_dt;
        }
    }

    Entity Simulation::fireWeapon(
        EntityFactory& factory,
        Entity weaponEntity,
        Entity owner,
        const cg::Point3& position,
        const cg::Vector3& direction
    )
    {
        return weaponSystem_.fire(
            registry_,
            factory,
            weaponEntity,
            owner,
            position,
            direction
        );
    }


    void Simulation::step()
    {
        const float dt = static_cast<float>(time_.fixed_dt);

        weaponSystem_.update(registry_, dt);
        movementSystem_.update(registry_, dt);
        collisionSystem_.update(registry_, dt);
        sensorSystem_.update(registry_);
        lifetimeSystem_.update(registry_, dt);

        time_.advance();
    }

    // Owns all entities and their component storage
    Registry registry_;

    SimulationTime time_;
    MovementSystem movementSystem_;

    // Leftover real time waiting to become a full simulation step
    double accumulator_ = 0.0;

    
}