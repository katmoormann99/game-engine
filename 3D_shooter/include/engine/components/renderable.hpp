//============================================================================
// Author: Kat Moormann
// File: renderable.hpp
// Purpose: Defines rendering metadata that associates an entity with a
//          renderable mesh or visual resource.
// Date: June 24 2025
//============================================================================

#pragma once

#include <cstdint>

namespace engine
{
    enum class MeshId : std::uint32_t
    {
        UnitSquare = 1,
        Target = 2,
        Sensor = 3,
        Projectile = 4
    };

    struct Renderable
    {
        MeshId meshId = MeshId::UnitSquare;
    };

}

