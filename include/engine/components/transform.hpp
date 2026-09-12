//============================================================================
// Author: Kat Moormann 
// File: transform.hpp
// Purpose: Defines spatial state for an entity, including its position and 
// eventually its orientation and scale
// Date: June 24 2025
//============================================================================

#pragma once 
#include "../../../geometry/point3.hpp"

namespace engine
{
    struct Transform
    {
        cg::Point3 position{0.0f, 0.0f, 0.0f};
        cg::Point3 rotation{0.0f, 0.0f, 0.0f};
        cg::Point3 scale{1.0f, 1.0f, 1.0f};
    };
}