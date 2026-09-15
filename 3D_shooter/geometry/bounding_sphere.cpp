#include "geometry/bounding_sphere.hpp"
#include "geometry/geometry.hpp"
#include "geometry/vector3.hpp"
#include <cmath>

namespace cg
{

BoundingSphere::BoundingSphere() : center{0.0f, 0.0f, 0.0f}, radius(1.0f) {}

BoundingSphere::BoundingSphere(const BoundingSphere &s) : center(s.center), radius(s.radius) {}

BoundingSphere::BoundingSphere(const Point3 &c, float r) : center(c), radius(r) {}

BoundingSphere::BoundingSphere(std::vector<Point3> &vertex_list)
{
    // Construct a sphere given a vertex list. Method by Ritter.

    // handling an empty list 
    if(vertex_list.empty())
    {
        center = Point3(0.0f, 0.0f, 0.0f);
        radius = 0.0f;
        return;
    }

    // picking an initial pair of far-apart points
    // starting from an arbitrary point
    Point3 p0 = vertex_list[0];

    // Find point p1 farthest from p0
    Point3 p1 = p0;
    float max_d2 = 0.0f;
    for(const auto &p : vertex_list)
    {
        float d2 = (p - p0).norm_squared();
        if(d2 > max_d2)
        {
            max_d2 = d2;
            p1 = p;
        }
    }

    // Find point p2 farthest from p1
    Point3 p2 = p1;
    max_d2 = 0.0f;
    for(const auto &p : vertex_list)
    {
        float d2 = (p - p1).norm_squared();
        if(d2 > max_d2)
        {
            max_d2 = d2;
            p2 = p;
        }
    }

    // initialize sphere to the one having segment p1-p2 as diameter
    center = p1.mid_point(p2); // midpoint of p1 and p2
    radius = std::sqrt((p2 - center).norm_squared()); // distance from center to p2

    // expand sphere to include any points outside it
    for(const auto &p : vertex_list)
    {
        Vector3 c_to_p(center, p);
        float dist = c_to_p.norm();

        // if point is already inside (or on) the sphere, nothing to do
        if(dist <= radius + EPSILON) continue;

        // point is outside: expanding sphere minimally to include it
        // new radius is halfway between old radius and the point distance
        float new_radius = 0.5f * (radius + dist);

        // moving center toward the point so that point lies on the new sphere
        // amount to move is proportional to how far outside the point is
        float move = (new_radius - radius) / dist;  // dist > radius here, so dist > 0
        center = center + c_to_p * move;

        radius = new_radius;
    }

}

BoundingSphere &BoundingSphere::merge_with(const BoundingSphere &s2)
{
    // this function should return the smallest sphere that contains both spheres 
    // vector from this center to the other center
    Vector3 c1_to_c2(center, s2.center);
    float dist = c1_to_c2.norm();

    // if centers are the same, just take the larger radius
    if(dist <= EPSILON)
    {
        if(s2.radius > radius) radius = s2.radius;
        return *this;
    }

    // check if this sphere fully contains s2
    if(radius >= dist + s2.radius)
    {
        return *this;
    }

    // check if s2 fully contains this sphere
    if(s2.radius >= dist + radius)
    {
        center = s2.center;
        radius = s2.radius;
        return *this;
    }

    // Otherwise, spheres partially overlap or are separate:
    // new radius is half the span from one far side to the other far side
    float new_radius = 0.5f * (dist + radius + s2.radius);

    // Move center toward s2.center by an amount that balances the radii
    float move = (new_radius - radius) / dist;
    center = center + c1_to_c2 * move;

    radius = new_radius;
    return *this;
}

BoundingSphere BoundingSphere::merge(const BoundingSphere &s2) const
{
    BoundingSphere combined(*this);
    combined.merge_with(s2);
    return combined;
}

} // namespace cg
