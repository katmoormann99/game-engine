//============================================================================
// Author: Kat Moormann
// File: light.hpp
// Purpose: Defines light properties for an ECS light entity.
// Date: July 18 2025
//============================================================================

#pragma once

#include "geometry/vector3.hpp"

namespace engine
{

struct Light
{
    cg::Vector3 color{
        1.0f,
        1.0f,
        1.0f
    };

    float intensity = 1.0f;

        cg::Vector3 direction{
        0.0f,
        1.0f,
        0.0f
    };

    float innerCutoffDegrees = 12.0f;
    float outerCutoffDegrees = 20.0f;

    bool active = true;
};

} // namespace engine