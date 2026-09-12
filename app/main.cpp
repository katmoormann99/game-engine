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

    engine::EntityFactory factory(registry);

    engine::Entity sensorEntity = factory.createSensor(cg::Point3(0.0f, 0.0f, 0.0f), 25.0f);
    engine::Entity targetEntity = factory.createTarget(cg::Point3(40.0f, 0.0f, 0.0f), cg::Vector3(-10.0f, 0.0f, 0.0f), 10.0);

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