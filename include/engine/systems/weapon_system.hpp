//============================================================================
// Author: Kat Moormann
// File: weapon_system.hpp
// Purpose: Updates weapon cooldown state and creates projectile entities when
//          weapons are fired.
// Date: July 20 2025
//============================================================================

#pragma once

#include "engine/core/registry.hpp"
#include "engine/factory/entity_factory.hpp"

#include "geometry/point3.hpp"
#include "geometry/vector3.hpp"

namespace engine
{

class WeaponSystem
{
public:
    void update(Registry& registry, float dt);

    // TODO
    // Revisit this weapon system dependency design 
    // fire() currently receives both Registry& and EntityFactory&, even though EntityFactory alraedy owns a reference to the SAME registry
    // This is intentially explicit for now while the shooting system is being build, but I definitely need to revist this 
    // to avoid the redundant dependencies and clarify ownership between gameplay systems, entity construction and the Registry
    Entity fire(Registry& registry, 
        EntityFactory& factory, 
        Entity weaponEntity, 
        Entity owner, 
        const cg::Point3& position, 
        const cg::Vector3& direction
    );
};

} // namespace engine