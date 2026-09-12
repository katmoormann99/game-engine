#include "engine/simulation/simulation.hpp"
#include "engine/rendering/graphics_backend.hpp"

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

    // Sensor entity
    engine::Entity sensorEntity = registry.create();

    registry.transforms().add(
        sensorEntity,
        engine::Transform{
            cg::Point3(0.0f, 0.0f, 0.0f)
        }
    );

    registry.sensors().add(
        sensorEntity,
        engine::Sensor{25.0f}
    );

    // Moving target
    engine::Entity targetEntity = registry.create();

    registry.transforms().add(
        targetEntity,
        engine::Transform{
            cg::Point3(40.0f, 0.0f, 0.0f)
        }
    );

    registry.velocities().add(
        targetEntity,
        engine::Velocity{
            cg::Vector3(-10.0f, 0.0f, 0.0f)
        }
    );

    registry.renderables().add(
        targetEntity,
        engine::Renderable{1}
    );

    registry.lifetimes().add(
        targetEntity,
        engine::Lifetime{10.0}
    );

    bool running = true;

    while (running)
    {
        // Process window / keyboard events
        running = graphics.handleEvents();

        // Advance simulation
        simulation.update(1.0 / 60.0);

        // Render current simulation state
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