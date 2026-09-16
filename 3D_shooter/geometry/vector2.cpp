#include "geometry/vector2.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

Vector2::Vector2() : x(0.0f), y(0.0f) {}

Vector2::Vector2(const Point2 &p) : x(p.x), y(p.y) {}

Vector2::Vector2(float ix, float iy) : x(ix), y(iy) {}

Vector2::Vector2(const Point2 &from, const Point2 &to) : x(to.x - from.x), y(to.y - from.y) {}

Vector2::Vector2(const Vector2 &w) : x(w.x), y(w.y) {}

Vector2 &Vector2::operator=(const Vector2 &w)
{
    x = w.x;
    y = w.y;
    return *this;
}

void Vector2::set(float ix, float iy)
{
    x = ix;
    y = iy;
}

void Vector2::set(const Point2 &p)
{
    x = p.x;
    y = p.y;
}

void Vector2::set(const Point2 &from, const Point2 &to)
{
    x = to.x - from.x;
    y = to.y - from.y;
}

Vector2 Vector2::operator+(const Vector2 &w) const { return Vector2(x + w.x, y + w.y); }

Vector2 &Vector2::operator+=(const Vector2 &w)
{
    x += w.x;
    y += w.y;
    return *this;
}

Vector2 Vector2::operator-(const Vector2 &w) const { return Vector2(x - w.x, y - w.y); }

Vector2 &Vector2::operator-=(const Vector2 &w)
{
    x -= w.x;
    y -= w.y;
    return *this;
}

Vector2 Vector2::operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }

Vector2 &Vector2::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

bool Vector2::operator==(const Vector2 &w) const { return (x == w.x && y == w.y); }

float Vector2::dot(const Vector2 &w) const { return (x * w.x + y * w.y); }

float Vector2::cross(const Vector2 &w) const { return (x * w.y - y * w.x); }

Vector2 Vector2::get_perpendicular(bool clockwise) const
{
    return (clockwise) ? Vector2(y, -x) : Vector2(-y, x);
}

float Vector2::norm() const { return std::sqrt(norm_squared()); }

float Vector2::norm_squared() const { return dot(*this); }

Vector2 &Vector2::normalize()
{
    // Normalize the vector if the norm is not 0 or 1
    float n = norm();
    if(n > EPSILON && n != 1.0f)
    {
        x /= n;
        y /= n;
    }
    return *this;
}

float Vector2::component(const Vector2 &w) const
{
    float n = w.dot(w);
    return (n != 0.0f) ? (dot(w) / n) : 0.0f;
}

Vector2 Vector2::projection(const Vector2 &w) const { return w * component(w); }

float Vector2::angle_between(const Vector2 &w) const
{
    return std::acos(dot(w) / (norm() * w.norm()));
}

Vector2 Vector2::reflect(const Vector2 &normal) const
{
    Vector2 d = *this;
    return (d - (normal * (2.0f * (d.dot(normal)))));
}

Vector2 operator*(float s, const Vector2 &v) { return Vector2(v.x * s, v.y * s); }

} // namespace cg
