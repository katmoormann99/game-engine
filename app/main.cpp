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
    std::cout << "[MAIN] Graphics intialized" << std::endl;

    engine::RenderSystem renderSystem;

    // 60 Hz simulation, 10-unit spatial grid cells
    engine::Simulation simulation(1.0 / 60.0, 10.0f);
    std::cout << "[MAIN] Simulation Created! " << std::endl;

    engine::Registry& registry = simulation.registry();
    std::cout << "[MAIN] Registry aquired " << std::endl;

    // The factory pattern here is very important - it centralizes the construction of common ECS entity types,
    // ensuring each entity receives the correct set of components without duplicating setup logic throughout the application
    engine::EntityFactory factory(registry);

    std::cout << "\nBUILDING SCENE" << std::endl;

    engine::Entity sensorEntity = factory.createSensor(cg::Point3(0.0f, 0.0f, 0.0f), 25.0f);

    engine::Entity target1 = factory.createTarget( cg::Point3(0.0f, 0.0f, 30.0f), cg::Vector3(0.0f, 0.0f, 0.0f), 60.0);
    engine::Entity target2 = factory.createTarget(cg::Point3(25.0f, 0.0f, 30.0f), cg::Vector3(-15.0f, 0.0f, 0.0f), 60.0);
    engine::Entity target3 = factory.createTarget(cg::Point3(0.0f, 20.0f, 40.0f), cg::Vector3(0.0f, -12.0f, -4.0f), 60.0);

    // engine::Entity floor = factory.createStaticSurface(cg::Point3(0.0f, 0.0f, 0.0f), cg::Vector3(0.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.22f, 0.20f, 0.18f, 1.0f});
    // engine::Entity ceiling = factory.createStaticSurface(cg::Point3(0.0f, 0.0f, 100.0f),cg::Vector3(180.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.20f, 0.25f, 0.34f, 1.0f});
    // engine::Entity back_wall = factory.createStaticSurface(cg::Point3(0.0f, 50.0f, 50.0f),cg::Vector3(90.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.34f, 0.22f, 0.25f, 1.0f});
    // engine::Entity left_wall = factory.createStaticSurface(cg::Point3(-50.0f, 0.0f, 50.0f),cg::Vector3(0.0f, 90.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.20f, 0.31f, 0.26f, 1.0f});
    // engine::Entity right_wall = factory.createStaticSurface(cg::Point3(50.0f, 0.0f, 50.0f),cg::Vector3(0.0f, -90.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.28f, 0.22f, 0.34f, 1.0f});
    // engine::Entity front_wall = factory.createStaticSurface(cg::Point3(0.0f, -50.0f, 50.0f),cg::Vector3(-90.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), engine::Material{0.7f, 0.7f, 0.7f, 1.0f});
    
    
    const engine::Material arenaBackdropMaterial{0.05f, 0.10f, 0.18f, 0.25f};

    engine::Entity arenaBackdrop = factory.createStaticSurface(cg::Point3(0.0f, 50.1f, 50.0f), cg::Vector3(90.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), arenaBackdropMaterial);
        
    engine::Entity camera = factory.createCamera(cg::Point3(0.0f, -90.0f, 50.0f), cg::Vector3(90.0f, 0.0f, 0.0f), 90.0f, 1.0f, 1.0f, 200.0f);
    engine::Entity light = factory.createLight(cg::Point3(0.0f, -100.0f, 50.0f), cg::Vector3(1.0f, 1.0f, 1.0f), 1.0f, 12.0f, 22.0f);
    engine::Entity weapon = factory.createWeapon(cg::Point3(0.0f, -88.0f, 30.0f), 80.0f, 0.25f);

    std::cout << "\nSCENE COMPLETE\n";

    std::cout << "Sensor:   Entity " << sensorEntity << '\n';
    std::cout << "Target 1: Entity " << target1 << '\n';
    std::cout << "Target 2: Entity " << target2 << '\n';
    std::cout << "Target 3: Entity " << target3 << '\n';
    std::cout << "Backdrop: Entity " << arenaBackdrop << '\n';
    std::cout << "Camera:   Entity " << camera << '\n';
    std::cout << "Light:    Entity " << light << '\n';
    std::cout << "Weapon:   Entity " << weapon << '\n';

    std::cout << "\nGAME LOOP START\n\n";

    bool running = true;
    bool firstFrame = true;

    auto previousTime = std::chrono::steady_clock::now();


    while (running)
    {
        const auto currentTime = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = currentTime - previousTime;
        previousTime = currentTime;
        const double frame_dt = elapsed.count();

        running = graphics.handleEvents();

        if (graphics.firePressed())
        {
            std::cout << "\nFIRE EVENT\n";
            std::cout << "[INPUT] SPACE pressed\n";
            std::cout << "[WEAPON] Weapon Entity " << weapon
                    << " attempting to fire\n";

            const engine::Transform& weaponTransform =
                registry.transforms().get(weapon);

            simulation.fireWeapon(
                factory,
                weapon,
                camera,
                weaponTransform.position,
                cg::Vector3(0.0f, 1.0f, 0.0f)
            );
        }

        simulation.update(frame_dt);
        if (firstFrame)
        {
            std::cout << "[LOOP] graphics.beginFrame()" << std::endl;
            std::cout << "[LOOP] renderSystem.render()" << std::endl;
        }

        graphics.beginFrame();

        renderSystem.render(
            registry,
            graphics
        );

        graphics.endFrame();

        if (firstFrame)
        {
            std::cout << "[LOOP] graphics.endFrame()\n";
            std::cout << "\n[LOOP] First frame complete.\n";
            std::cout << "       Per-frame logging disabled.\n\n";

            firstFrame = false;
        }
    }

    graphics.shutdown();

    return 0;
}