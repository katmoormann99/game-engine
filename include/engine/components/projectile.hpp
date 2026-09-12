//============================================================================
// Author: Kat Moormann
// File: projectile.hpp
// Purpose: Defines projectile-specific gameplay data for entities fired by weapons
// Date: July 20 2025
//============================================================================


#pragma once

#include "engine/core/entity.hpp"

namespace engine
{

struct Projectile
{
    Entity owner = 0;
    float damage = 1.0f;
};

}