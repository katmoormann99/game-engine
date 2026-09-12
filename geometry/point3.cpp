#include "geometry/point3.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

Point3::Point3() : x(0.0f), y(0.0f), z(0.0f) {}

Point3::Point3(float ix, float iy, float iz) : x(ix), y(iy), z(iz) {}

Point3::Point3(const Point3 &p) : x(p.x), y(p.y), z(p.z) {}

Point3 &Point3::operator=(const Point3 &p)
{
    x = p.x;
    y = p.y;
    z = p.z;
    return *this;
}

void Point3::set(float ix, float iy, float iz)
{
    x = ix;
    y = iy;
    z = iz;
}

bool Point3::operator==(const Point3 &p) const { return (x == p.x && y == p.y && z == p.z); }

Point3 Point3::affine_combination(float a0, float a1, const Point3 &p1) const
{
    return Point3(a0 * x + a1 * p1.x, a0 * y + a1 * p1.y, a0 * z + a1 * p1.z);
}

Point3 Point3::mid_point(const Point3 &p1) const
{
    return Point3(0.5f * x + 0.5f * p1.x, 0.5f * y + 0.5f * p1.y, 0.5f * z + 0.5f * p1.z);
}

bool Point3::is_in_polygon(const std::vector<Point3> &polygon, const Vector3 &n) const
{
    if(std::abs(n.x) >= std::abs(n.y) && std::abs(n.x) >= std::abs(n.z))
        return is_in_polygon_YZ(polygon); // Drop the x component
    else if(std::abs(n.y) >= std::abs(n.x) && std::abs(n.y) >= std::abs(n.z))
        return is_in_polygon_XZ(polygon);  // Drop the y component
    else return is_in_polygon_XY(polygon); // Drop the z component
}

Point3 Point3::operator+(const Vector3 &v) const { return Point3(x + v.x, y + v.y, z + v.z); }

Point3 Point3::operator-(const Vector3 &v) const { return Point3(x - v.x, y - v.y, z - v.z); }

Vector3 Point3::operator-(const Point3 &p) const { return Vector3(x - p.x, y - p.y, z - p.z); }

bool Point3::is_in_polygon_XY(const std::vector<Point3> &polygon) const
{
    bool inside = false;
    auto pt1 = polygon.end() - 1;
    auto pt2 = polygon.begin();
    bool y1 = (pt1->y >= y); //  Is vertex above or below test point
    bool y2;
    for(; pt2 != polygon.end(); pt1 = pt2, pt2++)
    {
        // Check if endpoints straddle ray. If so, +x ray could intersect
        // this edge.
        y2 = (pt2->y >= y);
        if(y1 != y2)
        {
            if((((pt2->y - y) * (pt1->x - pt2->x)) >= ((pt2->x - x) * (pt1->y - pt2->y))) == y2)
                inside = !inside;
        }
        y1 = y2;
    }
    return inside;
}

bool Point3::is_in_polygon_XZ(const std::vector<Point3> &polygon) const
{
    bool inside = false;
    auto pt1 = polygon.end() - 1;
    auto pt2 = polygon.begin();
    bool z1 = (pt1->z >= z); //  Is vertex above or below test point
    bool z2;
    for(; pt2 != polygon.end(); pt1 = pt2, pt2++)
    {
        // Check if endpoints straddle ray. If so, +x ray could intersect
        // this edge.
        z2 = (pt2->z >= z);
        if(z1 != z2)
        {
            if((((pt2->z - z) * (pt1->x - pt2->x)) >= ((pt2->x - x) * (pt1->z - pt2->z))) == z2)
                inside = !inside;
        }
        z1 = z2;
    }
    return inside;
}

bool Point3::is_in_polygon_YZ(const std::vector<Point3> &polygon) const
{
    bool inside = false;
    auto pt1 = polygon.end() - 1;
    auto pt2 = polygon.begin();
    bool z1 = (pt1->z >= z); //  Is vertex above or below test point
    bool z2;
    for(; pt2 != polygon.end(); pt1 = pt2, pt2++)
    {
        // Check if endpoints straddle ray. If so, +y ray could intersect
        // this edge.
        z2 = (pt2->z >= z);
        if(z1 != z2)
        {
            if((((pt2->z - z) * (pt1->y - pt2->y)) >= ((pt2->y - y) * (pt1->z - pt2->z))) == z2)
                inside = !inside;
        }
        z1 = z2;
    }
    return inside;
}

} // namespace cg
