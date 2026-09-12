//============================================================================
// Author: Kat Moormann 
// File: time.hpp
// Purpose: Defines simulation timing state used to advance the engine at a fixed timestep 
// independent of rendering frame rate
// Date: June 24 2025
//============================================================================

#pragma once 
#include <cstdint>

namespace engine
{
    /**
     * Important to keep this seperate from the render time 
     * PHYSICS:  |----------|----------|----------|
     *      16.67ms    16.67ms    16.67ms
     *
     * RENDER:   |---|---|---|---|---|---|---|---|
     *       ~6.9ms each at 144 FPS
     * 
     * Physics: "Every 16.67 ms, calculate where the car should be"
     * Rendering: "As often as I can, draw the car"
     */
    struct SimulationTime
    {
        // Note to self for future when I forget 
        // FPS is limited by how quickly the computer can produce frames
        // Hz is limited by how quickly the monitor can display refreshes  

        // Fixed amount of simulated time advanced on each simulation tick
        // 1000 ms / 60 = 16.67 ms per frame
        double fixed_dt = 1.0 / 60.0; 

        // Total elapsed simulation time
        double elapsed = 0.0;

        // Number of fixed simulation steps that have executed 
        // unsigned: a tick can NEVER be negative 
        // long long: Could become a very large number over a long-running simulation
        std::uint64_t tick = 0;

        void advance()
        {
            elapsed += fixed_dt;
            ++tick;
        }

    };
}