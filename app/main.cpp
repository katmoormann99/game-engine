#include "engine/simulation/simulation.hpp"
#include "engine/rendering/graphics_backend.hpp"

#include "include/engine/factory/entity_factory.hpp"

#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/components/lifetime.hpp"
#include "engine/components/sensor.hpp"
#include "engine/components/renderable.hpp"

#include "engine/systems/render_system.hpp"

int main()
{
    engine::GraphicsBackend graphics;

    if (!graphics.initialize())
    {
        return 1;
    }

    engine::RenderSystem renderSystem;

    // 60 Hz simulation, 10-unit spatial grid cells
    engine::Simulation simulation(1.0 / 60.0, 10.0f);

    engine::Registry& registry = simulation.registry();

    // The factory pattern here is very important - it centralizes the construction of common ECS entity types,
    // ensuring each entity receives the correct set of components without duplicating setup logic throughout the application
    engine::EntityFactory factory(registry);

    engine::Entity sensorEntity = factory.createSensor(cg::Point3(0.0f, 0.0f, 0.0f), 25.0f);

    engine::Entity target1 = factory.createTarget(cg::Point3(-25.0f, 0.0f, 30.0f), cg::Vector3(15.0f, 0.0f, 0.0f), 60.0);
    engine::Entity target2 = factory.createTarget(cg::Point3(25.0f, 0.0f, 30.0f), cg::Vector3(-15.0f, 0.0f, 0.0f), 60.0);
    engine::Entity target3 = factory.createTarget(cg::Point3(0.0f, 20.0f, 40.0f), cg::Vector3(0.0f, -12.0f, -4.0f), 60.0);

    engine::Entity floor = factory.createStaticSurface(cg::Point3(0.0f, 0.0f, 0.0f), cg::Vector3(0.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.22f, 0.20f, 0.18f, 1.0f});
    engine::Entity ceiling = factory.createStaticSurface(cg::Point3(0.0f, 0.0f, 100.0f),cg::Vector3(180.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.20f, 0.25f, 0.34f, 1.0f});
    engine::Entity back_wall = factory.createStaticSurface(cg::Point3(0.0f, 50.0f, 50.0f),cg::Vector3(90.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.34f, 0.22f, 0.25f, 1.0f});
    engine::Entity left_wall = factory.createStaticSurface(cg::Point3(-50.0f, 0.0f, 50.0f),cg::Vector3(0.0f, 90.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.20f, 0.31f, 0.26f, 1.0f});
    engine::Entity right_wall = factory.createStaticSurface(cg::Point3(50.0f, 0.0f, 50.0f),cg::Vector3(0.0f, -90.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.28f, 0.22f, 0.34f, 1.0f});
    // engine::Entity front_wall = factory.createStaticSurface(cg::Point3(0.0f, -50.0f, 50.0f),cg::Vector3(-90.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.7f, 0.7f, 0.7f, 1.0f});

    engine::Entity camera = factory.createCamera(cg::Point3(0.0f, -90.0f, 50.0f), cg::Vector3(0.0f, 0.0f, 0.0f), 70.0f, 1.0f, 1.0f, 200.0f);
    
    bool running = true;

    auto previousTime = std::chrono::steady_clock::now();

    while (running)
    {
        const auto currentTime = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = currentTime - previousTime;
        previousTime = currentTime;
        const double frame_dt = elapsed.count();
        running = graphics.handleEvents();
        simulation.update(frame_dt);
        graphics.beginFrame();

        renderSystem.render(
            registry,
            graphics
        );

        graphics.endFrame();
    }

    graphics.shutdown();

    return 0;
}