//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Original Author: David W. Nesbitt
//	Modified by Kat Moormann
//	File:    geometry.hpp
//	Purpose: Geometric types used in the lab.
//============================================================================

#ifndef __GEOMETRY_GEOMETRY_HPP__
#define __GEOMETRY_GEOMETRY_HPP__

#include <cmath>

namespace cg
{

#ifndef CG_MATH_CONSTANTS
#define CG_MATH_CONSTANTS
#define CG_PI 3.141592653589793115997963468544185161590576171875
#define CG_PHI 1.6180339887498948482072100296669248109537875279784202576
#define CG_PHI_INV 0.6180339887498948482072100296669248109537875279784202576
#endif

constexpr float PI = static_cast<float>(CG_PI);
constexpr float PHI = static_cast<float>(CG_PHI);
constexpr float PHI_INV = static_cast<float>(CG_PHI_INV);
constexpr float EPSILON = 0.000001f;
constexpr float RADIANS_PER_DEGREE = static_cast<float>(180.0 / CG_PI);
constexpr float DEGREES_PER_RADIAN = static_cast<float>(CG_PI / 180.0);

/**
 * Degrees to radians conversion
 * @param   d   Angle in degrees.
 * @return  Returns the angle in radians.
 */
float degrees_to_radians(float d);

/**
 * Radians to degrees conversion
 * @param   r   Angle in radians.
 * @return  Returns the angle in degrees.
 */
float radians_to_degrees(float r);

/**
 * Get a random number between 0 and 1.
 * return  Returns a random floating point number betwen 0 and 1.
 */
float rand_0_1();

/**
 * Fast inverse sqrt method. Originally used in Quake III
 * @param  x  Value to find inverse sqrt for
 * @return  Returns 1/sqrt(x)
 */
float fast_inv_sqrt(float x);

} // namespace cg

// Include individual geometry files
// clang-format off
// #include "geometry/hpoint2.hpp"
// #include "geometry/point2.hpp"
// #include "geometry/hpoint3.hpp"
#include "geometry/point3.hpp"
// #include "geometry/vector2.hpp"
#include "geometry/vector3.hpp"
// #include "geometry/segment2.hpp"
// #include "geometry/segment3.hpp"
// #include "geometry/plane.hpp"
#include "geometry/aabb.hpp"
#include "geometry/bounding_sphere.hpp"
// #include "geometry/ray3.hpp"
// #include "geometry/noise.hpp"
// #include "geometry/matrix.hpp"
// #include "geometry/types.hpp"
// clang-format on

#endif
