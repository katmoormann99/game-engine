#include "geometry/vector3.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

Vector3::Vector3(const Point3 &p) : x(p.x), y(p.y), z(p.z) {}

Vector3::Vector3(float ix, float iy, float iz) : x(ix), y(iy), z(iz) {}

Vector3::Vector3(const Point3 &from, const Point3 &to) :
    x(to.x - from.x), y(to.y - from.y), z(to.z - from.z)
{
}

Vector3::Vector3(const Vector3 &w) : x(w.x), y(w.y), z(w.z) {}

Vector3 &Vector3::operator=(const Vector3 &w)
{
    x = w.x;
    y = w.y;
    z = w.z;
    return *this;
}

void Vector3::set(float ix, float iy, float iz)
{
    x = ix;
    y = iy;
    z = iz;
}

void Vector3::set(const Point3 &p)
{
    x = p.x;
    y = p.y;
    z = p.z;
}

void Vector3::set(const Point3 &from, const Point3 &to)
{
    x = to.x - from.x;
    y = to.y - from.y;
    z = to.z - from.z;
}

Vector3 Vector3::operator+(const Vector3 &w) const { return Vector3(x + w.x, y + w.y, z + w.z); }

Vector3 &Vector3::operator+=(const Vector3 &w)
{
    x += w.x;
    y += w.y;
    z += w.z;
    return *this;
}

Vector3 Vector3::operator-(const Vector3 &w) const { return Vector3(x - w.x, y - w.y, z - w.z); }

Vector3 &Vector3::operator-=(const Vector3 &w)
{
    x -= w.x;
    y -= w.y;
    z -= w.z;
    return *this;
}

Vector3 Vector3::operator*(float scalar) const
{
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 &Vector3::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

bool Vector3::operator==(const Vector3 &w) const { return (x == w.x && y == w.y && z == w.z); }

float Vector3::dot(const Vector3 &w) const { return (x * w.x + y * w.y + z * w.z); }

Vector3 Vector3::cross(const Vector3 &w) const
{
    return Vector3(y * w.z - z * w.y, z * w.x - x * w.z, x * w.y - y * w.x);
}

float Vector3::norm() const { return std::sqrt(norm_squared()); }

float Vector3::norm_squared() const { return (dot(*this)); }

Vector3 &Vector3::normalize()
{
    // Normalize the vector if the norm is not 0 or 1
    float n = norm();
    if(n > EPSILON && n != 1.0f)
    {
        float inv = 1.0f / n;
        x *= inv;
        y *= inv;
        z *= inv;
    }
    return *this;
}

float Vector3::component(const Vector3 &w) const
{
    float n = w.dot(w);
    return (n != 0.0f) ? (dot(w) / n) : 0.0f;
}

Vector3 Vector3::projection(const Vector3 &w) const { return w * component(w); }

float Vector3::angle_between(const Vector3 &w) const
{
    return std::acos(dot(w) / (norm() * w.norm()));
}

Vector3 Vector3::reflect(const Vector3 &normal) const
{
    Vector3 d = *this;
    return (d - (normal * (2.0f * (d.dot(normal)))));
}

Vector3 operator*(float s, const Vector3 &v) { return Vector3(v.x * s, v.y * s, v.z * s); }

} // namespace cg
