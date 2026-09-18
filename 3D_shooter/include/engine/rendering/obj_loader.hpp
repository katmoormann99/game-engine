//============================================================================
// Author: Kat Moormann
// File: obj_loader.hpp
// Purpose: Loads triangle geometry from Wavefront OBJ files into CPU-side
//          vertex and index buffers for use by the rendering backend.
// Date: June 28 2025
//============================================================================

#pragma once

#include "engine/rendering/mesh.hpp"
#include <string>

namespace engine
{
    MeshData loadOBJ(const std::string& path);
} // namespace engine