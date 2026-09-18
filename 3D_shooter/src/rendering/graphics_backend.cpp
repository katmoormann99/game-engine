//============================================================================
// Author: Kat Moormann
// File: graphics_backend.cpp
// Purpose: Implements the graphics backend using SDL3 and OpenGL for window
//          management, frame setup, and basic mesh draw submission.
// Date: June 28 2025
//============================================================================

#include "engine/rendering/graphics_backend.hpp"
#include "engine/rendering/graphics.hpp"
#include "engine/rendering/obj_loader.hpp"

#include "geometry/matrix.hpp"
#include "geometry/geometry.hpp"

#include <SDL3/SDL.h>

#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>

namespace engine
{

    namespace
    {
        SDL_Window* g_window = nullptr;
        SDL_GLContext g_context = nullptr;
    } // anonymous namespace

    // Initialize SDL, OpenGL, meshes, shaders, skybox, and arena box.
    bool GraphicsBackend::initialize()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            std::cerr << "Error initializing SDL: " << SDL_GetError() << '\n';
            return false;
        }

        // Request core OpenGL.
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        // Create window properties.
        SDL_PropertiesID props = SDL_CreateProperties();

        if (props == 0)
        {
            std::cerr << "Error creating SDL window properties: " << SDL_GetError() << '\n';
            return false;
        }

        SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Kat Moormann ECS Game Engine");
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 800);
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 800);

        g_window = SDL_CreateWindowWithProperties(props);
        SDL_DestroyProperties(props);

        if (g_window == nullptr)
        {
            std::cerr << "Error creating SDL window: " << SDL_GetError() << '\n';
            return false;
        }

        // Create OpenGL context.
        g_context = SDL_GL_CreateContext(g_window);

        if (g_context == nullptr)
        {
            std::cerr << "Error creating OpenGL context: " << SDL_GetError() << '\n';
            return false;
        }

        std::cout << "OpenGL: " << glGetString(GL_VERSION) << '\n';
        std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

        // Set OpenGL state.
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, 800, 800);

        // Create scene rendering resources.
        if (!shader_.initialize("shaders/basic.vert", "shaders/basic.frag")) { return false; }
        if (!particleShader_.initialize("shaders/particles.vert", "shaders/particles.frag")) { return false; }
        if (!createUnitSquareMesh()) { return false; }
        if (!createTargetMesh()) { return false; }

        // Set default projection.
        projection_.set_identity();
        projection_.m00() = 1.428f;
        projection_.m11() = 1.428f;
        projection_.m22() = -1.010f;
        projection_.m23() = -2.010f;
        projection_.m32() = -1.0f;
        projection_.m33() = 0.0f;

        view_.set_identity();

        // Initialize skybox.
        // if (!skybox_.initialize({
        //     "../assets/skybox/galaxy_3_v1.jpg",
        //     "../assets/skybox/galaxy_3_v1.jpg",
        //     "../assets/skybox/galaxy_3_v1.jpg",
        //     "../assets/skybox/galaxy_3_v1.jpg",
        //     "../assets/skybox/galaxy_3_v1.jpg",
        //     "../assets/skybox/galaxy_3_v1.jpg"
        // }))

        // Initialize skybox.
        if (!skybox_.initialize({
            "../assets/skybox/right.jpg",   // +X
            "../assets/skybox/left.jpg",    // -X
            "../assets/skybox/top.jpg",     // +Y
            "../assets/skybox/bottom.jpg",  // -Y
            "../assets/skybox/front.jpg",   // +Z
            "../assets/skybox/back.jpg"     // -Z
        }))
        {
            std::cerr << "Failed to initialize skybox.\n";
            return false;
        }

        // Initialize arena wireframe.
        if (!arenaBox_.initialize())
        {
            std::cerr << "Failed to initialize arena box.\n";
            return false;
        }

        return true;
    }
    bool GraphicsBackend::createUnitSquareMesh()
    {
        MeshData meshData;

        meshData.vertices =
        {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
        };

        meshData.indices =
        {
            0, 1, 2,
            0, 2, 3
        };

        return unitSquareMesh_.initialize(meshData);
    }

    bool GraphicsBackend::createTargetMesh()
    {
        try {
            MeshData meshData = loadOBJ("../assets/models/space_shuttle.obj");
            std::cout << "Loaded target mesh: " << meshData.vertices.size() << " vertices, " << meshData.indices.size() / 3 << " triangles\n";
            return targetMesh_.initialize(meshData);
        } catch (const std::exception& e){
            std::cerr << "Failed to load target mesh: " << e.what() << '\n';
            return false;
        }
    }

    void GraphicsBackend::setCamera(const Transform& transform, const Camera& camera)
    {
        cg::Matrix4x4 cameraWorld;

        cameraWorld.translate(
            transform.position.x,
            transform.position.y,
            transform.position.z
        );

        cameraWorld.rotate_x(transform.rotation.x);
        cameraWorld.rotate_y(transform.rotation.y);
        cameraWorld.rotate_z(transform.rotation.z);

        // View matrix = inverse of camera's world transform.
        view_ = cameraWorld.get_inverse();

        const float radians = camera.fovDegrees * 3.14159265358979323846f / 180.0f;

        const float f = 1.0f / std::tan(radians * 0.5f);

        const float nearPlane = camera.nearPlane;

        const float farPlane = camera.farPlane;

        projection_.set_identity();
        projection_.m00() = f / camera.aspectRatio;
        projection_.m11() = f;
        projection_.m22() = (farPlane + nearPlane) / (nearPlane - farPlane);
        projection_.m23() = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
        projection_.m32() = -1.0f;
        projection_.m33() = 0.0f;
    }

    void GraphicsBackend::setLight(const Transform& transform, const Light& light)
    {
        shader_.use();

        shader_.setVec3("uLightPosition", transform.position.x, transform.position.y, transform.position.z);
        shader_.setVec3("uLightColor", light.color.x, light.color.y, light.color.z);
        shader_.setFloat("uLightIntensity", light.intensity);
        shader_.setVec3("uLightDirection", light.direction.x, light.direction.y, light.direction.z);

        const float innerRadians = cg::degrees_to_radians(light.innerCutoffDegrees);
        const float outerRadians = cg::degrees_to_radians(light.outerCutoffDegrees);

        shader_.setFloat("uInnerCutoff", std::cos(innerRadians));
        shader_.setFloat("uOuterCutoff", std::cos(outerRadians));
    }

    bool GraphicsBackend::firePressed() const
    {
        return firePressed_;
    }

    // Poll SDL events.
    // Return false when the user wants to close the application.
    bool GraphicsBackend::handleEvents()
    {
        SDL_Event event;

        // Reset every frame.
        // This becomes true only during a frame where SPACE is pressed.
        firePressed_ = false;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    return false;

                case SDL_EVENT_KEY_DOWN:
                {
                    if (event.key.key == SDLK_ESCAPE)
                    {
                        return false;
                    }

                    if (event.key.key == SDLK_SPACE)
                    {
                        firePressed_ = true;
                    }

                    break;
                }

                case SDL_EVENT_WINDOW_RESIZED:
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                {
                    glViewport(
                        0,
                        0,
                        event.window.data1,
                        event.window.data2
                    );

                    break;
                }

                default:
                    break;
            }
        }

        return true;
    }

    // Clear the framebuffer before rendering a new frame.
    void GraphicsBackend::beginFrame()
    {
        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );
    }

    void GraphicsBackend::drawSkybox()
    {
        skybox_.draw(view_.get(), projection_.get());
    }

    void GraphicsBackend::drawArenaBox()
    {
        arenaBox_.draw(view_.get(), projection_.get());
    }


    // Draw one ECS entity.
    //
    // For now this is only a placeholder.
    // The next step will be:
    // 1. Look up meshId
    // 2. Bind VAO/VBO
    // 3. Build model matrix from Transform
    // 4. Upload model matrix to shader
    // 5. Call glDrawArrays/glDrawElements
    void GraphicsBackend::drawMesh(MeshId meshId, const Transform& transform, const Material& material)
    {
        const GPUMesh* mesh = nullptr;

        switch (meshId)
        {
            case MeshId::UnitSquare:
                mesh = &unitSquareMesh_;
                break;

            case MeshId::Target:
                mesh = &targetMesh_;
                break;

            case MeshId::Projectile:
                mesh = &unitSquareMesh_;
                break;

            case MeshId::Sensor:
                // Sensor mesh not implemented yet.
                return;

            default:
                return;
        }

        shader_.use();

        // Build model matrix from ECS Transform.
        cg::Matrix4x4 model;

        model.translate(transform.position.x, transform.position.y, transform.position.z);
        model.rotate_x(transform.rotation.x);
        model.rotate_y(transform.rotation.y);
        model.rotate_z(transform.rotation.z);
        model.scale(transform.scale.x, transform.scale.y, transform.scale.z);

        // Vertex position --> Model Matrix --> Where is the surface?
        shader_.setMatrix4("uModel", model.get());
        shader_.setMatrix4("uView", view_.get());
        shader_.setMatrix4("uProjection", projection_.get());
        shader_.setVec3("uMaterialColor", material.r, material.g, material.b);

        // Model matrix transforms the object. Normal matrix transforms the object's surface
        // directions while keeping them perpindicular to the surface
        // Vertex normal --> Normal Matrix --> Which way is the surface facing?
        // Lambert Lighting: dot(L,N)
        cg::Matrix4x4 normalMatrix = model.get_inverse().get_transpose();
        shader_.setMatrix4("uNormalMatrix", normalMatrix.get());
        shader_.setFloat("uMaterialAlpha", material.a);
        
        mesh->draw();
    }

    void GraphicsBackend::drawParticle(const Transform& transform, const Material& material)
    {
        particleShader_.use();

        // The particle shader constructs the billboard around this
        // world-space center.
        particleShader_.setVec3("uParticlePosition", transform.position.x, transform.position.y, transform.position.z);

        // Particles are uniformly scaled, so one value is enough.
        particleShader_.setFloat("uParticleSize", transform.scale.x);
        particleShader_.setVec3("uParticleColor", material.r, material.g, material.b);

        particleShader_.setFloat("uParticleAlpha", material.a);
        particleShader_.setMatrix4("uView", view_.get());

        particleShader_.setMatrix4("uProjection", projection_.get());

        // Keep depth testing so particles can disappear behind objects,
        // but don't let transparent particles write into the depth buffer.
        glDepthMask(GL_FALSE);
        unitSquareMesh_.draw();
        glDepthMask(GL_TRUE);
    }

    // Present the completed frame.
    void GraphicsBackend::endFrame()
    {
        skybox_.draw(view_.get(), projection_.get());
        SDL_GL_SwapWindow(g_window);
    }


    // Release SDL/OpenGL resources.
    void GraphicsBackend::shutdown()
    {
        unitSquareMesh_.shutdown();
        targetMesh_.shutdown();
        shader_.shutdown();
        particleShader_.shutdown();

        skybox_.shutdown();
        arenaBox_.shutdown();

        if (g_context != nullptr)
        {
            SDL_GL_DestroyContext(g_context);
            g_context = nullptr;
        }

        if (g_window != nullptr)
        {
            SDL_DestroyWindow(g_window);
            g_window = nullptr;
        }

        SDL_Quit();
    }

} // namespace engine