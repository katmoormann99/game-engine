//============================================================================
// Author: Kat Moormann
// File: entity_factory.hpp
// Purpose: Centralizes construction of common simulation entity types and
//          assigns the component sets required by each entity.
// Date: June 28 2025
//============================================================================

/**
 * EntityFactory centralizes the creation of common ECS entity types.
 *
 * In the traditional Factory Method pattern, a factory creates different
 * concrete objects that share a common product interface. For example:
 *
 *      VehicleFactory
 *          |
 *          +-- createFourWheeler()
 *          +-- createTwoWheeler()
 *
 * In our ECS, entities are not different C++ subclasses. Instead, an entity's
 * "type" is determined by the components attached to it.
 *
 *      EntityFactory
 *          |
 *          +-- createTarget()
 *          |       Transform
 *          |       Velocity
 *          |       Renderable
 *          |       Lifetime
 *          |
 *          +-- createSensor()
 *                  Transform
 *                  Sensor
 *                  Renderable
 *
 * The factory therefore hides the details of assembling each entity while the
 * Registry remains responsible for entity IDs and component storage.
 */


 #pragma once 

 #include "engine/core/registry.hpp"
 #include "geometry/point3.hpp"
 #include "geometry/vector3.hpp"

 namespace engine 
 {

    class EntityFactory 
    {
        public:
            explicit EntityFactory(Registry &registry);

            Entity createSensor(const cg::Point3 &position, float detectionRadius);

            Entity createTarget(const cg::Point3 &position, const cg::Vector3 &velocity, double lifetime);

            Entity createStaticSurface(const cg::Point3& position, const cg::Vector3& rotation, const cg::Vector3& scale, const Material& material);

            Entity createCamera(const cg::Point3& position, const cg::Vector3& rotation, float fovDegrees, float aspectRatio, float nearPlane, float farPlane);

            Entity createLight(const cg::Point3& position, const cg::Vector3& color, float intensity, float innerCutoffDegrees, float outerCutoffDegrees);

            Entity createWeapon(const cg::Point3& position, float projectileSpeed, float cooldownSeconds);

            Entity createProjectile(Entity owner, const cg::Point3& position, const cg::Vector3& velocity, float damage, double lifetime);

        private:
            Registry &registry_;
    };
 }