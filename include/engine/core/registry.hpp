//============================================================================
// Author: Kat Moormann 
// File: registry.hpp
// Purpose: Manages entity creation, destruction, and centralized access to
//          component storage within the ECS.
// Date: June 24 2025
//============================================================================

#pragma once

#include "component_storage.hpp"

#include "../components/transform.hpp"
#include "../components/velocity.hpp"
#include "../components/lifetime.hpp"
#include "../components/sensor.hpp"
#include "../components/renderable.hpp"
#include "../components/material.hpp"

namespace engine
{

class Registry
{
public:

    Entity create() {return nextEntity_++;}

    void destroy(Entity entity)
    {
        transforms_.remove(entity);
        velocities_.remove(entity);
        lifetimes_.remove(entity);
        sensors_.remove(entity);
        renderables_.remove(entity);
        materials_.remove(entity);
    }

    ComponentStorage<Transform>& transforms() {return transforms_;}
    const ComponentStorage<Transform>& transforms() const {return transforms_;}

    ComponentStorage<Velocity>& velocities(){return velocities_;}

    ComponentStorage<Lifetime>& lifetimes(){return lifetimes_;}

    ComponentStorage<Sensor>& sensors(){return sensors_;}

    ComponentStorage<Renderable>& renderables(){return renderables_;}
    const ComponentStorage<Renderable>& renderables() const{return renderables_;}

    ComponentStorage<Material>& materials() {return materials_;}
    const ComponentStorage<Material>& materials() const {return materials_;};

private:

    Entity nextEntity_ = 1;

    ComponentStorage<Transform> transforms_;
    ComponentStorage<Velocity> velocities_;
    ComponentStorage<Lifetime> lifetimes_;
    ComponentStorage<Sensor> sensors_;
    ComponentStorage<Renderable> renderables_;
    ComponentStorage<Material> materials_;
};

}