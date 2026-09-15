//============================================================================
// Author: Kat Moormann 
// File: velocity.hpp
// Purpose: Defines linear velocity data used by simulations systems to update entity positions over time
// Date: June 24 2025
//============================================================================

#pragma once 
#include "../../../geometry/vector3.hpp"

namespace engine
{
    struct Velocity
    {
        cg::Vector3 linear;
    };
}