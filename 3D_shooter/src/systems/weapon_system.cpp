//============================================================================
// Author: Kat Moormann
// File: weapon_system.cpp
// Purpose: Implements weapon cooldown handling and projectile creation.
// Date: July 20 2025
//============================================================================

#include "engine/systems/weapon_system.hpp"

#include "engine/components/weapon.hpp"

namespace engine
{

void WeaponSystem::update(Registry& registry, float dt)
{
    auto& weapons = registry.weapons();
    for (Entity entity : weapons.entities())
    {
        Weapon& weapon = weapons.get(entity);
        if (weapon.cooldownRemaining > 0.0f)
        {
            weapon.cooldownRemaining -= dt;
            if (weapon.cooldownRemaining < 0.0f)
            {
                weapon.cooldownRemaining = 0.0f;
            }
        }
    }
}

Entity WeaponSystem::fire(
    Registry& registry,
    EntityFactory& factory,
    Entity weaponEntity,
    Entity owner,
    const cg::Point3& position,
    const cg::Vector3& direction
)
{
    auto& weapons = registry.weapons();

    if (!weapons.has(weaponEntity))
    {
        return 0;
    }

    Weapon& weapon = weapons.get(weaponEntity);

    if (weapon.cooldownRemaining > 0.0f)
    {
        return 0;
    }

    cg::Vector3 shotDirection = direction;

    shotDirection.normalize();

    cg::Vector3 projectileVelocity = shotDirection * weapon.projectileSpeed;

    Entity projectile =
        factory.createProjectile(
            owner,
            position,
            projectileVelocity,
            1.0f,
            6.0
        );

    weapon.cooldownRemaining = weapon.cooldownSeconds;

    return projectile;
}

} // namespace engine