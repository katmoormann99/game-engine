//============================================================================
// Author: Kat Moormann
// File: weapon.hpp
// Date: July 20 2025
//============================================================================


#pragma once

namespace engine
{

struct Weapon
{
    float projectileSpeed = 80.0f;
    float cooldownSeconds = 0.25f;
    float cooldownRemaining = 0.0f;
};

}