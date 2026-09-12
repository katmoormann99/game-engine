//============================================================================
// Author: Kat Moormann
// File: graphics_backend.cpp
// Purpose: Implements the graphics backend using SDL3 and OpenGL for window
//          management, frame setup, and basic mesh draw submission.
// Date: June 28 2025
//============================================================================

#include "engine/rendering/graphics_backend.hpp"
#include "engine/rendering/graphics.hpp"
#include "geometry/matrix.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <SDL3/SDL.h>

#include <iostream>

namespace engine
{

    namespace
    {

    SDL_Window* g_window = nullptr;
    SDL_GLContext g_context = nullptr;

    std::string loadTextFile(const std::string& path)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << path << '\n';
            return {};
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        return buffer.str();
    }


    GLuint compileShader(GLenum type, const std::string& source)
    {
        GLuint shader = glCreateShader(type);

        const char* sourcePtr = source.c_str();

        glShaderSource(shader, 1, &sourcePtr, nullptr);
        glCompileShader(shader);
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char log[1024];
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            std::cerr << "Shader compilation failed:\n" << log << '\n';
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    } // anonymous namespace

    // Initialize SDL, create the window, and create the OpenGL context.
    bool GraphicsBackend::initialize()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            std::cerr << "Error initializing SDL: " << SDL_GetError() << '\n';
            return false;
        }

        // Request a core OpenGL context.
        SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE
        );

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        // Create window properties.
        SDL_PropertiesID props = SDL_CreateProperties();

        if (props == 0)
        {
            std::cerr << "Error creating SDL window properties: " << SDL_GetError() << '\n';
            return false;
        }

        SDL_SetStringProperty(
            props,
            SDL_PROP_WINDOW_CREATE_TITLE_STRING,
            "Kat Moormann ECS Game Engine"
        );

        SDL_SetBooleanProperty(
            props,
            SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN,
            true
        );

        SDL_SetBooleanProperty(
            props,
            SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN,
            true
        );

        SDL_SetNumberProperty(
            props,
            SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER,
            800
        );

        SDL_SetNumberProperty(
            props,
            SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER,
            800
        );

        g_window = SDL_CreateWindowWithProperties(props);

        SDL_DestroyProperties(props);

        if (g_window == nullptr)
        {
            std::cerr << "Error creating SDL window: " << SDL_GetError() << '\n';
            return false;
        }

        // Create the OpenGL rendering context.
        g_context = SDL_GL_CreateContext(g_window);

        if (g_context == nullptr)
        {
            std::cerr << "Error creating OpenGL context: " << SDL_GetError() << '\n';
            return false;
        }

        std::cout << "OpenGL: " << glGetString(GL_VERSION) << '\n';
        std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

        // Default OpenGL state.
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

        glEnable(GL_DEPTH_TEST);

        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

        glViewport(0, 0, 800, 800);

        if (!createShaderProgram())
        {
            return false;
        }

        if (!createUnitSquareMesh())
        {
            return false;
        }

        projection_.set_identity();

        projection_.m00() = 1.428f;
        projection_.m11() = 1.428f;
        projection_.m22() = -1.010f;
        projection_.m23() = -2.010f;
        projection_.m32() = -1.0f;
        projection_.m33() = 0.0f;

        view_.set_identity();

        view_.m00() = 1.0f;
        view_.m11() = 0.0f;
        view_.m12() = 1.0f;
        view_.m13() = -50.0f;
        view_.m21() = -1.0f;
        view_.m22() = 0.0f;
        view_.m23() = -90.0f;
        view_.m33() = 1.0f;

        return true;
    }

    bool GraphicsBackend::createShaderProgram()
    {
        const std::string vertexSource = loadTextFile("shaders/basic.vert");
        const std::string fragmentSource = loadTextFile("shaders/basic.frag");

        if(vertexSource.empty() || fragmentSource.empty())
        {
            return false;
        }

        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);

        if (vertexShader == 0)
        {
            return false; 
        }

        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

        if (fragmentShader == 0)
        {
            glDeleteShader(vertexShader);
            return false;
        }

        shaderProgram_ = glCreateProgram();

        glAttachShader(shaderProgram_, vertexShader);
        glAttachShader(shaderProgram_, fragmentShader);

        glLinkProgram(shaderProgram_);

        GLint success = 0;

        glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        if (!success)
        {
            char log[1024];
            glGetProgramInfoLog(shaderProgram_, sizeof(log), nullptr, log);

            std::cerr << "Shader linking failed: \n" << log << std::endl;

            glDeleteProgram(shaderProgram_);
            shaderProgram_ = 0;

            return false;
        }
        return true;
    }

    bool GraphicsBackend::createUnitSquareMesh()
    {
        // position xyz, normal xyz
        const float vertices[] =
        {
            -0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
            0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f
        };

        const std::uint32_t indices[] = 
        {
            0, 1, 2,
            0, 2, 3
        };

        glGenVertexArrays(1, &unitSquareMesh_.vao);

        glGenBuffers(1, &unitSquareMesh_.vbo);
        glGenBuffers(1, &unitSquareMesh_.ebo);

        glBindVertexArray(unitSquareMesh_.vao);

        // Vertex data
        glBindBuffer(GL_ARRAY_BUFFER, unitSquareMesh_.vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, unitSquareMesh_.ebo);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);

        // Normal
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

        unitSquareMesh_.indexCount = 6;

        glBindVertexArray(0);

        return true;
    }

    // Poll SDL events.
    // Return false when the user wants to close the application.
    bool GraphicsBackend::handleEvents()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    return false;

                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE)
                    {
                        return false;
                    }
                    break;

                case SDL_EVENT_WINDOW_RESIZED:
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    glViewport(
                        0,
                        0,
                        event.window.data1,
                        event.window.data2
                    );
                    break;

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


    // Draw one ECS entity.
    //
    // For now this is only a placeholder.
    // The next step will be:
    // 1. Look up meshId
    // 2. Bind VAO/VBO
    // 3. Build model matrix from Transform
    // 4. Upload model matrix to shader
    // 5. Call glDrawArrays/glDrawElements
    void GraphicsBackend::drawMesh(std::uint32_t meshId, const Transform& transform, const Material& material)
    {
        if (meshId != 1) { return;}

        glUseProgram(shaderProgram_);

        // Build model matrix from ECS Transform.
        cg::Matrix4x4 model;

        model.translate(transform.position.x, transform.position.y, transform.position.z);
        model.rotate_x(transform.rotation.x);
        model.rotate_y(transform.rotation.y);
        model.rotate_z(transform.rotation.z);
        model.scale(transform.scale.x, transform.scale.y, transform.scale.z);

        GLint modelLocation = glGetUniformLocation(shaderProgram_, "uModel");
        GLint viewLocation = glGetUniformLocation(shaderProgram_, "uView");
        GLint projectionLocation = glGetUniformLocation(shaderProgram_, "uProjection");
        GLint colorLocation = glGetUniformLocation(shaderProgram_, "uColor");

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model.get());
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, model.get());
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, model.get());

        glUniform4f(colorLocation,material.r, material.g, material.b, material.a);
        glBindVertexArray(unitSquareMesh_.vao);
        glDrawElements(GL_TRIANGLES, unitSquareMesh_.indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    // Present the completed frame.
    void GraphicsBackend::endFrame()
    {
        SDL_GL_SwapWindow(g_window);
    }


    // Release SDL/OpenGL resources.
    void GraphicsBackend::shutdown()
    {
        if (unitSquareMesh_.ebo != 0)
        {
            glDeleteBuffers(1, &unitSquareMesh_.ebo);
            unitSquareMesh_.ebo = 0;
        }

        if (unitSquareMesh_.vbo != 0)
        {
            glDeleteBuffers(1, &unitSquareMesh_.vbo);
            unitSquareMesh_.vbo = 0;
        }

        if (unitSquareMesh_.vao != 0)
        {
            glDeleteVertexArrays(1, &unitSquareMesh_.vao);
            unitSquareMesh_.vao = 0;
        }

        if (shaderProgram_ != 0)
        {
            glDeleteProgram(shaderProgram_);
            shaderProgram_ = 0;
        }
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