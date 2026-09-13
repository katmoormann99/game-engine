//============================================================================
// Author: Kat Moormann
// File: arena_box.cpp
// Purpose: Implements a wireframe arena renderer that draws the 12 edges of
//          the 3D collision volume using OpenGL line primitives.
// Date: August 05 2025
//============================================================================

#include "engine/rendering/arena_box.hpp"
#include "engine/rendering/graphics.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace engine
{

namespace
{

const float ARENA_VERTICES[] =
{
    -50.0f, -30.0f,   0.0f,
     50.0f, -30.0f,   0.0f,
     50.0f,  30.0f,   0.0f,
    -50.0f,  30.0f,   0.0f,

    -50.0f, -30.0f, 100.0f,
     50.0f, -30.0f, 100.0f,
     50.0f,  30.0f, 100.0f,
    -50.0f,  30.0f, 100.0f
};

const std::uint32_t ARENA_INDICES[] =
{
    0, 1,
    1, 2,
    2, 3,
    3, 0,

    4, 5,
    5, 6,
    6, 7,
    7, 4,

    0, 4,
    1, 5,
    2, 6,
    3, 7
};

std::string loadTextFile(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cerr << "Failed to open arena shader: " << path << '\n';
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

        std::cerr << "Arena shader compilation failed:\n" << log << '\n';

        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

} // anonymous namespace

bool ArenaBox::initialize()
{
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ARENA_VERTICES), ARENA_VERTICES, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ARENA_INDICES), ARENA_INDICES, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glBindVertexArray(0);

    const std::string vertexSource = loadTextFile("shaders/arena_box.vert");
    const std::string fragmentSource = loadTextFile("shaders/arena_box.frag");

    if (vertexSource.empty() || fragmentSource.empty()) { return false; }

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);

    if (vertexShader == 0) { return false; }

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

        std::cerr << "Arena shader linking failed:\n" << log << '\n';

        glDeleteProgram(shaderProgram_);
        shaderProgram_ = 0;

        return false;
    }

    return true;
}

void ArenaBox::draw(const float* view, const float* projection)
{
    glUseProgram(shaderProgram_);

    GLint viewLocation = glGetUniformLocation(shaderProgram_, "uView");
    GLint projectionLocation = glGetUniformLocation(shaderProgram_, "uProjection");

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, view);
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection);

    glBindVertexArray(vao_);

    glLineWidth(2.0f);

    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

void ArenaBox::shutdown()
{
    if (ebo_ != 0)
    {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }

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
}

} // namespace engine