#pragma once

namespace engine
{

struct Particle
{
    float age = 0.0f;
    float duration = 0.75f;

    float initialSize = 0.15f;
    float finalSize = 0.02f;

    float initialAlpha = 1.0f;
    float finalAlpha = 0.0f;
};

} // namespace engine