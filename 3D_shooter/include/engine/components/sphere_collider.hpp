//============================================================================
// Author: Kat Moormann
// File: sphere_collider.hpp
// Purpose: Defines a simple spherical collision volume used for broad and
//          narrow collision tests in the simulation.
// Date: July 10 2025
//============================================================================

#pragma once

namespace engine
{

struct SphereCollider
{
    float radius = 1.0f;
};

} // namespace engine