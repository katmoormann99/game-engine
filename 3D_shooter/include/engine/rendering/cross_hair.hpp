#pragma once

#include "engine/rendering/shader.hpp"

#include <cstdint>

namespace engine
{

class Crosshair
{
public:
    bool initialize();
    void draw(float x, float y);
    void shutdown();

private:
    std::uint32_t vao_ = 0;
    std::uint32_t vbo_ = 0;
    Shader shader_;
};

} // namespace engine