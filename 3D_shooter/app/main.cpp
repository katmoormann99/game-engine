#include "engine/simulation/simulation.hpp"
#include "engine/rendering/graphics_backend.hpp"
#include "engine/rendering/particle_emitter.hpp"
#include "include/engine/factory/entity_factory.hpp"

#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/components/lifetime.hpp"
#include "engine/components/sensor.hpp"
#include "engine/components/renderable.hpp"
#include "engine/components/particle.hpp"
#include "engine/components/material.hpp"

#include "engine/systems/render_system.hpp"
#include "engine/systems/weapon_system.hpp"

int main()
{
    engine::GraphicsBackend graphics;

    if (!graphics.initialize())
    {
        return 1;
    }
    std::cout << "[MAIN] Graphics intialized" << std::endl;

    engine::RenderSystem renderSystem;
    engine::WeaponSystem weaponSystem;

    // 60 Hz simulation, 4.5-unit spatial grid cells
    engine::Simulation simulation(1.0 / 60.0, 4.5f);
    std::cout << "[MAIN] Simulation Created! " << std::endl;

    engine::Registry& registry = simulation.registry();
    std::cout << "[MAIN] Registry aquired " << std::endl;

    // The factory pattern here is very important - it centralizes the construction of common ECS entity types,
    // ensuring each entity receives the correct set of components without duplicating setup logic throughout the application
    engine::EntityFactory factory(registry);
    engine::ParticleEmitter particleEmitter;

    std::cout << "\nBUILDING SCENE" << std::endl;

    engine::Entity sensorEntity = factory.createSensor(cg::Point3(0.0f, 0.0f, 0.0f), 25.0f);

    for (int i = 0; i < 1000; ++i)
    {
        const float x = -40.0f + static_cast<float>((i * 13) % 80);
        const float y = -20.0f + static_cast<float>((i * 7) % 40);
        const float z = 10.0f + static_cast<float>((i * 11) % 80);

        const float vx = (i % 2 == 0) ? 8.0f : -8.0f;
        const float vy = (i % 3 == 0) ? 5.0f : -5.0f;
        const float vz = (i % 4 == 0) ? 6.0f : -6.0f;

        factory.createTarget(cg::Point3(x, y, z), cg::Vector3(vx, vy, vz), 160.0);
    }

    
    const engine::Material arenaSurfaceMaterial{0.0f, 0.35f, 0.50f, 0.10f};
    engine::Entity floor = factory.createStaticSurface(cg::Point3(0.0f, 0.0f, 0.0f), cg::Vector3(0.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 60.0f, 1.0f), arenaSurfaceMaterial);
    engine::Entity ceiling = factory.createStaticSurface(cg::Point3(0.0f, 0.0f, 100.0f), cg::Vector3(180.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 60.0f, 1.0f), arenaSurfaceMaterial);
    engine::Entity backWall = factory.createStaticSurface(cg::Point3(0.0f, 30.0f, 50.0f), cg::Vector3(90.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), arenaSurfaceMaterial);
    engine::Entity leftWall = factory.createStaticSurface(cg::Point3(-50.0f, 0.0f, 50.0f), cg::Vector3(0.0f, 90.0f, 0.0f), cg::Vector3(100.0f, 60.0f, 1.0f), arenaSurfaceMaterial);
    engine::Entity rightWall = factory.createStaticSurface(cg::Point3(50.0f, 0.0f, 50.0f), cg::Vector3(0.0f, -90.0f, 0.0f), cg::Vector3(100.0f, 60.0f, 1.0f), arenaSurfaceMaterial);
    
    const engine::Material arenaBackdropMaterial{0.05f, 0.10f, 0.18f, 0.25f};
    engine::Entity arenaBackdrop = factory.createStaticSurface(cg::Point3(0.0f, 30.1f, 50.0f), cg::Vector3(90.0f, 0.0f, 0.0f), cg::Vector3(100.0f, 100.0f, 1.0f), arenaBackdropMaterial);
    engine::Entity camera = factory.createCamera(cg::Point3(0.0f, -90.0f, 50.0f), cg::Vector3(90.0f, 0.0f, 0.0f), 90.0f, 1.0f, 1.0f, 300.0f);
    engine::Entity light = factory.createLight(cg::Point3(0.0f, -95.0f, 55.0f), cg::Vector3(1.0f, 1.0f, 1.0f), 1.0f, 12.0f, 22.0f);
    engine::Entity weapon = factory.createWeapon(cg::Point3(0.0f, -100.0f, 40.0f), 150.0f, 0.25f);

    std::cout << "\nSCENE COMPLETE\n";

    std::cout << "Sensor:   Entity " << sensorEntity << '\n';
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
        weaponSystem.aim(registry, weapon, graphics.mouseDeltaX(), graphics.mouseDeltaY());

        if (graphics.firePressed())
        {
            std::cout << "\nFIRE EVENT\n";
            std::cout << "[INPUT] SPACE pressed\n";
            std::cout << "[WEAPON] Weapon Entity " << weapon  << " attempting to fire\n";

            const engine::Transform& weaponTransform = registry.transforms().get(weapon);
            simulation.fireWeapon(factory, weapon, camera, weaponTransform.position, cg::Vector3(0.0f, 1.0f, 0.0f));
        }

        // const auto simulationStart = std::chrono::steady_clock::now();
        simulation.update(frame_dt);
        // const auto simulationEnd = std::chrono::steady_clock::now();

        for (const engine::ImpactEvent& impact : simulation.consumeImpactEvents())
        {
            particleEmitter.emitExplosion(factory, impact.position, 75);
        }
                // const std::chrono::duration<double, std::milli> simulationElapsed = simulationEnd - simulationStart;
        // std::cout << "[PROFILE] Simulation update: " << simulationElapsed.count() << "ms" << std::endl;
        if (firstFrame)
        {
            std::cout << "[LOOP] graphics.beginFrame()" << std::endl;
            std::cout << "[LOOP] renderSystem.render()" << std::endl;
        }

        graphics.beginFrame();

        renderSystem.render(registry,graphics);
        graphics.drawCrosshair();

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