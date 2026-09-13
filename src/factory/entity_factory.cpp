//============================================================================
// Author: Kat Moormann
// File: entity_factory.cpp
// Purpose: Implements construction of common ECS entities by creating an
//          entity ID and assigning the appropriate component composition.
// Date: June 24 2025
//============================================================================

#include "engine/factory/entity_factory.hpp"

#include "engine/components/lifetime.hpp"
#include "engine/components/renderable.hpp"
#include "engine/components/sensor.hpp"
#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/components/material.hpp"
#include "engine/components/sphere_collider.hpp"
#include "engine/components/camera.hpp"
#include "engine/components/light.hpp"
#include "engine/components/target.hpp"


namespace engine 
{
    EntityFactory::EntityFactory(Registry &registry) : registry_(registry){}

    Entity EntityFactory::createSensor(const cg::Point3 &position, float detectionRadius) 
    {
        Entity entity = registry_.create();
        registry_.transforms().add(entity, Transform{position});
        registry_.sensors().add(entity, Sensor{detectionRadius});
        registry_.renderables().add(entity, Renderable{MeshId::Sensor});
        registry_.materials().add(entity, Material{0.2f, 0.8f, 1.0f, 1.0f});

        return entity;
    }

    Entity EntityFactory::createTarget(const cg::Point3 &position, const cg::Vector3 &velocity, double lifetime)
    {
        Entity entity = registry_.create();
        std::cout << "\n[FACTORY] Creating Target | Entity " << entity << std::endl;
        registry_.transforms().add(entity, Transform{position, cg::Vector3(0.0f, 0.0f, 0.0f), cg::Vector3(0.03f, 0.03f, 0.03f)});
        registry_.velocities().add(entity, Velocity{velocity});
        registry_.renderables().add(entity, Renderable{MeshId::Target});
        registry_.lifetimes().add(entity, Lifetime{lifetime});
        registry_.materials().add(entity, Material{1.0f, 0.3f, 0.2f, 1.0f});
        registry_.sphereColliders().add(entity, SphereCollider{2.0f});

        // Gameplay identity: this entity is explicitly a target
        registry_.targets().add(entity, Target{});
        return entity;
    }

    Entity EntityFactory::createStaticSurface(const cg::Point3& position, const cg::Vector3& rotation, const cg::Vector3& scale, const Material& material)
    {
        Entity entity = registry_.create();
        std::cout << "\n[FACTORY] Creating createStaticSurface | Entity " << entity << std::endl;
        
        registry_.transforms().add(entity, Transform{position, rotation, scale});
        registry_.renderables().add(entity, Renderable{MeshId::UnitSquare});
        registry_.materials().add(entity, material);

        return entity;
    }

    Entity EntityFactory::createCamera(const cg::Point3& position, const cg::Vector3& rotation, float fovDegrees, float aspectRatio, float nearPlane, float farPlane)
    {
        Entity entity = registry_.create();
        std::cout << "\n[FACTORY] Creating createCamera | Entity " << entity << std::endl;

        registry_.transforms().add(entity, Transform{position, rotation,cg::Vector3(1.0f, 1.0f, 1.0f)});
        registry_.cameras().add(entity, Camera{fovDegrees, aspectRatio, nearPlane, farPlane, true});

        return entity;
    }

    Entity EntityFactory::createLight(const cg::Point3& position, const cg::Vector3& color, float intensity, float innerCutoffDegrees, float outerCutoffDegrees)
    {
        Entity entity = registry_.create();
        std::cout << "\n[FACTORY] Creating createLight | Entity " << entity << std::endl;

        registry_.transforms().add(entity, Transform{position,cg::Vector3(0.0f, 0.0f, 0.0f), cg::Vector3(1.0f, 1.0f, 1.0f)});
        registry_.lights().add(entity, Light{color, intensity, cg::Vector3(0.0f, 1.0f, 0.0f), innerCutoffDegrees, outerCutoffDegrees, true});

        return entity;
    }

    Entity EntityFactory::createWeapon(const cg::Point3& position, float projectileSpeed, float cooldownSeconds)
    {
        Entity entity = registry_.create();
        std::cout << "\n[FACTORY] Creating createWeapon | Entity " << entity << std::endl;

        registry_.transforms().add(entity, Transform{position, cg::Vector3(0.0f, 0.0f, 0.0f), cg::Vector3(1.0f, 1.0f, 1.0f)});
        registry_.weapons().add(entity, Weapon{projectileSpeed, cooldownSeconds, 0.0f});

        return entity;
    }

    Entity EntityFactory::createProjectile(Entity owner, const cg::Point3& position, const cg::Vector3& velocity, float damage, double lifetime)
    {
        Entity entity = registry_.create();
        std::cout << "\n[FACTORY] Creating createProjectile | Entity " << entity << std::endl;

        registry_.transforms().add(entity, Transform{position, cg::Vector3(90.0f, 0.0f, 0.0f), cg::Vector3(0.25f, 1.5f, 0.25f)});

        registry_.velocities().add(entity, Velocity{velocity});
        registry_.sphereColliders().add(entity,SphereCollider{0.5f});

        registry_.projectiles().add(entity, Projectile{owner, damage});

        registry_.lifetimes().add(entity, Lifetime{lifetime});

        // We will give this its own mesh shortly.
        registry_.renderables().add(entity, Renderable{MeshId::Projectile});

        registry_.materials().add(entity, Material{1.0f, 0.85f, 0.15f, 1.0f});

        return entity;
    }

}