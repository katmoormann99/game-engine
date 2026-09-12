#include "geometry/geometry.hpp"

#include <algorithm>

namespace cg
{

float degrees_to_radians(float d) { return d * DEGREES_PER_RADIAN; }

float radians_to_degrees(float r) { return r * RADIANS_PER_DEGREE; }

float rand_0_1() { return (float)std::rand() / (float)RAND_MAX; }

float fast_inv_sqrt(float x)
{
    float xhalf = 0.5f * x;
    int   i = *(int *)&x;              // get bits for floating value
    i = 0x5f3759df - (i >> 1);         // give initial guess y0
    x = *(float *)&i;                  // convert bits back to float
    return x * (1.5f - xhalf * x * x); // newton step
    // x *= 1.5f - xhalf*x*x;     // repeating step increases accuracy
}

} // namespace cg
