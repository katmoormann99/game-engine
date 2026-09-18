#include "engine/rendering/cross_hair.hpp"
#include "engine/rendering/graphics.hpp"

namespace engine
{

namespace
{

const float CROSSHAIR_VERTICES[] =
{
    -0.02f, 0.0f,
     0.02f, 0.0f,

     0.0f, -0.02f,
     0.0f,  0.02f
};

} // anonymous namespace

bool Crosshair::initialize()
{
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CROSSHAIR_VERTICES), CROSSHAIR_VERTICES, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    glBindVertexArray(0);

    if (!shader_.initialize("shaders/cross_hair.vert", "shaders/cross_hair.frag"))
    {
        shutdown();
        return false;
    }

    return true;
}

void Crosshair::draw(float x, float y)
{
    glDisable(GL_DEPTH_TEST);

    shader_.use();
    shader_.setVec2("uCrosshairPosition", x, y);

    glBindVertexArray(vao_);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void Crosshair::shutdown()
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

    shader_.shutdown();
}

} // namespace engine