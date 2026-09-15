//============================================================================
// Author: Kat Moormann
// File: camera.hpp
// Purpose: Defines camera projection properties for an ECS camera entity.
// Date: July 15 2025
//============================================================================

#pragma once

namespace engine
{

struct Camera
{
    float fovDegrees = 70.0f;
    float aspectRatio = 1.0f;
    float nearPlane = 1.0f;
    float farPlane = 200.0f;

    bool active = true;
};

} // namespace engine