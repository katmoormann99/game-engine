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

    engine::Entity target1 = factory.createTarget(cg::Point3(0.0f, 0.0f, 30.0f), cg::Vector3(0.0f, 0.0f, 0.0f), 60.0);
    engine::Entity target2 = factory.createTarget(cg::Point3(25.0f, 0.0f, 30.0f), cg::Vector3(-15.0f, 0.0f, 0.0f), 60.0);
    engine::Entity target3 = factory.createTarget(cg::Point3(0.0f, 20.0f, 40.0f), cg::Vector3(0.0f, -12.0f, -4.0f), 60.0);
    engine::Entity target4 = factory.createTarget(cg::Point3(-30.0f, -15.0f, 25.0f), cg::Vector3(12.0f, 5.0f, 7.0f), 60.0);
    engine::Entity target5 = factory.createTarget(cg::Point3(30.0f, 15.0f, 70.0f), cg::Vector3(-10.0f, -6.0f, -8.0f), 60.0);
    engine::Entity target6 = factory.createTarget(cg::Point3(-20.0f, 10.0f, 75.0f), cg::Vector3(8.0f, -7.0f, -10.0f), 60.0);
    engine::Entity target7 = factory.createTarget(cg::Point3(15.0f, -20.0f, 55.0f), cg::Vector3(-6.0f, 10.0f, 5.0f), 60.0);
    engine::Entity target8 = factory.createTarget(cg::Point3(-35.0f, 20.0f, 50.0f), cg::Vector3(14.0f, -8.0f, 3.0f), 60.0);
    engine::Entity target9 = factory.createTarget(cg::Point3(35.0f, -10.0f, 20.0f), cg::Vector3(-12.0f, 7.0f, 9.0f), 60.0);
    engine::Entity target10 = factory.createTarget(cg::Point3(-25.0f, -20.0f, 65.0f), cg::Vector3(10.0f, 11.0f, -6.0f), 60.0);
    engine::Entity target11 = factory.createTarget(cg::Point3(10.0f, 15.0f, 80.0f), cg::Vector3(7.0f, -9.0f, -12.0f), 60.0);
    engine::Entity target12 = factory.createTarget(cg::Point3(-40.0f, 5.0f, 35.0f), cg::Vector3(14.0f, 6.0f, 8.0f), 60.0);
    engine::Entity target13 = factory.createTarget(cg::Point3(20.0f, -15.0f, 50.0f), cg::Vector3(-9.0f, 8.0f, 11.0f), 60.0);

    
    const engine::Material arenaSurfaceMaterial{0.0f, 0.35f, 0.50f, 0.10f};

    engine::Entity floor = factory.createStaticSurface(cg::Point3(0.0f, 0.0f, 0.0f), cg::Vector3(0.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 60.0f, 1.0f), arenaSurfaceMaterial);

    engine::Entity ceiling = factory.createStaticSurface(cg::Point3(0.0f, 0.0f, 100.0f), cg::Vector3(180.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 60.0f, 1.0f), arenaSurfaceMaterial);

    engine::Entity backWall = factory.createStaticSurface(cg::Point3(0.0f, 30.0f, 50.0f), cg::Vector3(90.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), arenaSurfaceMaterial);

    engine::Entity leftWall = factory.createStaticSurface(cg::Point3(-50.0f, 0.0f, 50.0f), cg::Vector3(0.0f, 90.0f, 0.0f), cg::Vector3(100.0f, 60.0f, 1.0f), arenaSurfaceMaterial);

    engine::Entity rightWall = factory.createStaticSurface(cg::Point3(50.0f, 0.0f, 50.0f), cg::Vector3(0.0f, -90.0f, 0.0f), cg::Vector3(100.0f, 60.0f, 1.0f), arenaSurfaceMaterial);

    const engine::Material arenaBackdropMaterial{0.05f, 0.10f, 0.18f, 0.25f};

    engine::Entity arenaBackdrop = factory.createStaticSurface(cg::Point3(0.0f, 30.1f, 50.0f), cg::Vector3(90.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), arenaBackdropMaterial);

    engine::Entity camera = factory.createCamera(cg::Point3(0.0f, -110.0f, 50.0f), cg::Vector3(90.0f, 0.0f, 0.0f), 90.0f, 1.0f, 1.0f, 300.0f);

    engine::Entity light = factory.createLight(cg::Point3(0.0f, -105.0f, 55.0f), cg::Vector3(1.0f, 1.0f, 1.0f), 1.0f, 12.0f, 22.0f);

    engine::Entity weapon = factory.createWeapon(cg::Point3(0.0f, -100.0f, 50.0f), 80.0f, 0.25f);

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