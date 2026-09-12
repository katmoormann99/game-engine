//============================================================================
// Author: Kat Moormann
// File: lifetime.hpp
// Purpose: Defines remaining simulation lifetime for temporary entities so
//          they can be removed when their lifetime expires.
//============================================================================

#pragma once

namespace engine
{

struct Lifetime
{
    double remaining = 0.0;
};

}