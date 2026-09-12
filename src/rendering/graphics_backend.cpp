//============================================================================
// Author: Kat Moormann
// File: graphics_backend.cpp
// Purpose: Implements the graphics backend using SDL3 and OpenGL for window
//          management, frame setup, and basic mesh draw submission.
// Date: June 24 2025
//============================================================================

#include "engine/rendering/graphics_backend.hpp"

#include "engine/rendering/graphics.hpp"
#include <SDL3/SDL.h>

#include <iostream>

namespace engine
{

namespace
{
    SDL_Window* g_window = nullptr;
    SDL_GLContext g_context = nullptr;
}

namespace
{

GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);

        std::cerr
            << "Shader compilation failed:\n"
            << log
            << '\n';

        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

} 

// Initialize SDL, create the window, and create the OpenGL context.
bool GraphicsBackend::initialize()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr
            << "Error initializing SDL: "
            << SDL_GetError()
            << '\n';

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
        std::cerr
            << "Error creating SDL window properties: "
            << SDL_GetError()
            << '\n';

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
        std::cerr
            << "Error creating SDL window: "
            << SDL_GetError()
            << '\n';

        return false;
    }

    // Create the OpenGL rendering context.
    g_context = SDL_GL_CreateContext(g_window);

    if (g_context == nullptr)
    {
        std::cerr
            << "Error creating OpenGL context: "
            << SDL_GetError()
            << '\n';

        return false;
    }

    std::cout
        << "OpenGL: "
        << glGetString(GL_VERSION)
        << '\n';

    std::cout
        << "GLSL: "
        << glGetString(GL_SHADING_LANGUAGE_VERSION)
        << '\n';

    // Default OpenGL state.
    glClearColor(
        0.05f,
        0.05f,
        0.05f,
        1.0f
    );

    glEnable(GL_DEPTH_TEST);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glViewport(
        0,
        0,
        800,
        800
    );

    if (!createShaderProgram())
    {
        return false;
    }

    if (!createTestMesh())
    {
        return false;
    }

    return true;
}

bool GraphicsBackend::createShaderProgram()
{
    const char* vertexShaderSource = R"(
        #version 410 core

        layout(location = 0) in vec3 position;

        uniform vec3 uTranslation;

        void main()
        {
            vec3 translatedPosition =
                position + uTranslation;

            gl_Position =
                vec4(translatedPosition, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 410 core

        out vec4 fragColor;

        void main()
        {
            fragColor =
                vec4(1.0, 0.4, 0.2, 1.0);
        }
    )";

    GLuint vertexShader =
        compileShader(
            GL_VERTEX_SHADER,
            vertexShaderSource
        );

    if (vertexShader == 0)
    {
        return false;
    }

    GLuint fragmentShader =
        compileShader(
            GL_FRAGMENT_SHADER,
            fragmentShaderSource
        );

    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return false;
    }

    shaderProgram_ =
        glCreateProgram();

    glAttachShader(
        shaderProgram_,
        vertexShader
    );

    glAttachShader(
        shaderProgram_,
        fragmentShader
    );

    glLinkProgram(shaderProgram_);

    GLint success = 0;

    glGetProgramiv(
        shaderProgram_,
        GL_LINK_STATUS,
        &success
    );

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!success)
    {
        char log[1024];

        glGetProgramInfoLog(
            shaderProgram_,
            sizeof(log),
            nullptr,
            log
        );

        std::cerr
            << "Shader linking failed:\n"
            << log
            << '\n';

        return false;
    }

    return true;
}

bool GraphicsBackend::createTestMesh()
{
    const float vertices[] =
    {
        -0.1f, -0.1f, 0.0f,
         0.1f, -0.1f, 0.0f,
         0.0f,  0.1f, 0.0f
    };

    glGenVertexArrays(
        1,
        &vao_
    );

    glGenBuffers(
        1,
        &vbo_
    );

    glBindVertexArray(vao_);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        vbo_
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        nullptr
    );

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
void GraphicsBackend::drawMesh(
    std::uint32_t meshId,
    const Transform& transform
)
{
    if (meshId != 1)
    {
        return;
    }

    glUseProgram(shaderProgram_);

    GLint translationLocation =
        glGetUniformLocation(
            shaderProgram_,
            "uTranslation"
        );

    // Scale the simulation coordinates down temporarily
    // so they fit inside OpenGL clip space.
    const float scale = 0.02f;

    glUniform3f(
        translationLocation,
        transform.position.x * scale,
        transform.position.y * scale,
        transform.position.z * scale
    );

    glBindVertexArray(vao_);

    glDrawArrays(
        GL_TRIANGLES,
        0,
        3
    );

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
    if (vbo_ != 0)
    {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }

    if (vao_ != 0)
    {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
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