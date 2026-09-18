//============================================================================
// Author: Kat Moormann
// File: impact_event.hpp
// Purpose: Describes a projectile impact produced by the collision system.
//============================================================================

#pragma once

#include "engine/core/entity.hpp"
#include "geometry/point3.hpp"

namespace engine
{

struct ImpactEvent
{
    Entity projectile = INVALID_ENTITY;
    Entity target = INVALID_ENTITY;
    cg::Point3 position{0.0f, 0.0f, 0.0f};
};

} // namespace engine
