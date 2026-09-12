//============================================================================
// Author: Kat Moormann 
// File:  material.hpp
// Date: June 28 2025
//============================================================================

#pragma once 
#include "../../../geometry/point3.hpp"

namespace engine
{
    struct Material
    {
        float r = 1.0f;
        float g = 1.0f;
        float b = 1.0f;
        float a = 1.0f;
    };
}