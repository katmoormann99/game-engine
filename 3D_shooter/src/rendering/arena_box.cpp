//============================================================================
// Author: Kat Moormann
// File: arena_box.cpp
// Purpose: Implements a wireframe arena renderer that draws the 12 edges of
//          the 3D collision volume using OpenGL line primitives.
// Date: August 05 2025
//============================================================================

#include "engine/rendering/arena_box.hpp"
#include "engine/rendering/graphics.hpp"

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

    if (!shader_.initialize("shaders/arena_box.vert", "shaders/arena_box.frag"))
    {
        shutdown();
        return false;
    }

    return true;
}

void ArenaBox::draw(const float* view, const float* projection)
{
    shader_.use();
    shader_.setMatrix4("uView", view);
    shader_.setMatrix4("uProjection", projection);

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

    shader_.shutdown();
}

} // namespace engine