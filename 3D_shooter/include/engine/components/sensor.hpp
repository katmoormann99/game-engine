//============================================================================
// Author: Kat Moormann
// File: sensor.hpp
// Purpose: Defines sensor properties used to detect nearby entities within
//          the simulation environment.
//============================================================================

#pragma once

namespace engine
{

struct Sensor
{
    float detectionRadius = 0.0f;
};

} // namespace engine