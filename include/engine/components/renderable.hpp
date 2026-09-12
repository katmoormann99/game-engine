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

struct Renderable
{
    std::uint32_t meshId = 0;
};

}

