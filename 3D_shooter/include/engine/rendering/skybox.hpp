#pragma once

#include "engine/rendering/shader.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace engine
{

class Skybox
{
public:
    bool initialize(const std::vector<std::string>& faces);
    void draw(const float* view, const float* projection);
    void shutdown();

private:
    std::uint32_t vao_ = 0;
    std::uint32_t vbo_ = 0;
    std::uint32_t cubemapTexture_ = 0;
    Shader shader_;
};

} // namespace engine