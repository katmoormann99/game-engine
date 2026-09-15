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