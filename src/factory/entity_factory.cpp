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

namespace engine 
{
    EntityFactory::EntityFactory(Registry &registry) : registry_(registry){}

    Entity EntityFactory::createSensor(const cg::Point3 &position, float detectionRadius) 
    {
        Entity entity = registry_.create();
        registry_.transforms().add(entity, Transform{position});
        registry_.sensors().add(entity, Sensor{detectionRadius});
        registry_.renderables().add(entity, Renderable{2});
        registry_.materials().add(entity, Material{0.2f, 0.8f, 1.0f, 1.0f});

        return entity;
    }

    Entity EntityFactory::createTarget(const cg::Point3 &position, const cg::Vector3 &velocity, double lifetime)
    {
        Entity entity = registry_.create();
        registry_.transforms().add(entity, Transform{position});
        registry_.velocities().add(entity, Velocity{velocity});
        registry_.renderables().add(entity, Renderable{1});
        registry_.lifetimes().add(entity, Lifetime{lifetime});
        registry_.materials().add(entity, Material{1.0f, 0.3f, 0.2f, 1.0f});
        registry_.sphereColliders().add(entity, SphereCollider{2.0f});

        return entity;
    }

    Entity EntityFactory::createStaticSurface(const cg::Point3& position, const cg::Vector3& rotation, const cg::Vector3& scale, const Material& material)
    {
        Entity entity = registry_.create();
        
        registry_.transforms().add(entity,
            Transform{
                position,
                rotation,
                scale
            }
        );

        registry_.renderables().add(
            entity,
            Renderable{1}
        );

        registry_.materials().add(
            entity,
            material
        );

        return entity;
    }

    Entity EntityFactory::createCamera(const cg::Point3& position, const cg::Vector3& rotation, float fovDegrees, float aspectRatio, float nearPlane, float farPlane)
    {
        Entity entity = registry_.create();

        registry_.transforms().add(
            entity,
            Transform{
                position,
                rotation,
                cg::Vector3(1.0f, 1.0f, 1.0f)
            }
        );

        registry_.cameras().add(
            entity,
            Camera{
                fovDegrees,
                aspectRatio,
                nearPlane,
                farPlane,
                true
            }
        );

        return entity;
    }

}