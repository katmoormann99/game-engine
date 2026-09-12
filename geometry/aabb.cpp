#include "geometry/aabb.hpp"
#include "geometry/geometry.hpp"

namespace cg
{

AABB::AABB() // AABB: axis - aligned bounding box
: min(0.0f, 0.0f, 0.0f),
    max(0.0f, 0.0f, 0.0f),
    center(0.0f, 0.0f, 0.0f),
    half_diagonal(0.0f, 0.0f, 0.0f)
{
}

AABB::AABB(const Point3 &imin, const Point3 &imax)
{
    update(imin, imax);
}

AABB::AABB(const std::vector<Point3> &vertex_list)
{
    create(vertex_list);
}

void AABB::create(const std::vector<Point3> &vertex_list)
{
    // Handling an empty list 
    if (vertex_list.empty())
    {
        update(Point3(0.0f, 0.0f, 0.0f), Point3(0.0f, 0.0f, 0.0f));
        return;
    }

    // initialize min/max to first vertex
    Point3 mn = vertex_list[0];
    Point3 mx = vertex_list[0];

    // Expand bounds to include every vertex 
    for (const auto &p : vertex_list)
    {
        if (p.x < mn.x) mn.x = p.x;
        if (p.y < mn.y) mn.y = p.y;
        if (p.z < mn.z) mn.z = p.z;

        if (p.x > mx.x) mx.x = p.x;
        if (p.y > mx.y) mx.y = p.y;
        if (p.z > mx.z) mx.z = p.z;
    }

    update(mn, mx);
}

void AABB::update(const Point3 &imin, const Point3 &imax)
{
    min = imin;
    max = imax;
    compute_center();
}

void AABB::merge(const AABB &box)
{
    Point3 mn = min;
    Point3 mx = max;

    if(box.min.x < mn.x) mn.x = box.min.x;
    if(box.min.y < mn.y) mn.y = box.min.y;
    if(box.min.z < mn.z) mn.z = box.min.z;

    if(box.max.x > mx.x) mx.x = box.max.x;
    if(box.max.y > mx.y) mx.y = box.max.y;
    if(box.max.z > mx.z) mx.z = box.max.z;

    update(mn, mx);
}

Point3 AABB::min_pt() const
{
    return min;
}

Point3 AABB::max_pt() const
{
    return max;
}

void AABB::compute_center()
{
    // Center is midpoint of min/max corners 
    center = Point3(
        0.5f * (min.x + max.x),
        0.5f * (min.y + max.y),
        0.5f * (min.z + max.z)
    );
    
    // Half-diagonal is half the size in each axis 
    half_diagonal = Vector3(
        0.5f * (max.x - min.x),
        0.5f * (max.y - min.y),
        0.5f * (max.z - min.z)
    );
}

} // namespace cg
