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
    Simulation::Simulation(double fixed_dt, float spatialCellSize) : sensorSystem_(spatialCellSize), collisionSystem_(spatialCellSize)
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
        const float fixedDt_ = static_cast<float>(time_.fixed_dt);

        weaponSystem_.update(registry_, fixedDt_);

        const auto movementStart = std::chrono::steady_clock::now();
        movementSystem_.update(registry_, static_cast<float>(fixedDt_));
        const auto movementEnd = std::chrono::steady_clock::now();

        const auto collisionStart = std::chrono::steady_clock::now();
        collisionSystem_.update(registry_, static_cast<float>(fixedDt_));
        const auto collisionEnd = std::chrono::steady_clock::now();

        const auto sensorStart = std::chrono::steady_clock::now();
        sensorSystem_.update(registry_);
        const auto sensorEnd = std::chrono::steady_clock::now();

        const auto lifetimeStart = std::chrono::steady_clock::now();
        lifetimeSystem_.update(registry_, static_cast<float>(fixedDt_));
        const auto lifetimeEnd = std::chrono::steady_clock::now();

        const std::chrono::duration<double, std::milli> movementMs = movementEnd - movementStart;
        const std::chrono::duration<double, std::milli> collisionMs = collisionEnd - collisionStart;
        const std::chrono::duration<double, std::milli> sensorMs = sensorEnd - sensorStart;
        const std::chrono::duration<double, std::milli> lifetimeMs = lifetimeEnd - lifetimeStart;

        static int profileSamples = 0;
        static double movementTotal = 0.0;
        static double collisionTotal = 0.0;
        static double sensorTotal = 0.0;
        static double lifetimeTotal = 0.0;

        movementTotal += movementMs.count();
        collisionTotal += collisionMs.count();
        sensorTotal += sensorMs.count();
        lifetimeTotal += lifetimeMs.count();

        ++profileSamples;

        if (profileSamples >= 60)
        {
            std::cout << "\n[PROFILE AVERAGE OVER 60 TICKS]\n";
            std::cout << "Movement:  " << movementTotal / profileSamples << " ms\n";
            std::cout << "Collision: " << collisionTotal / profileSamples << " ms\n";
            std::cout << "Sensor:    " << sensorTotal / profileSamples << " ms\n";
            std::cout << "Lifetime:  " << lifetimeTotal / profileSamples << " ms\n\n";

            profileSamples = 0;
            movementTotal = 0.0;
            collisionTotal = 0.0;
            sensorTotal = 0.0;
            lifetimeTotal = 0.0;
        }
        time_.advance();
    }
    
}