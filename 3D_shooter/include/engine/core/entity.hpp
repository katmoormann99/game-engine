//============================================================================
// Author: Kat Moormann 
// File: entity.hpp
// Purpose: Defines the lightweight entity ID used to assoicate entites with their component data in the ECS
// Date: June 24 2025
//============================================================================


#pragma once 

#include <cstdint> 

/**
 * In en entity component system - the entity itself should be LIGHTWEIGHT
 * The REAL data lives in component storage 
 * This is what allows us to avoid a giant inheritance tree like 
 * GameObject -> Vehicle -> Aircraft -> Missile 
 * and INSTEAD compare behavior from DATA! 
 */
namespace engine {
    using Entity = std::uint32_t;
    constexpr Entity INVALID_ENTITY = 0;
}