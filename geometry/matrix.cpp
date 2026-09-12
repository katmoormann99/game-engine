#include "geometry/matrix.hpp"

#include "common/logging.hpp"
#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

// Forward declare logging function
void logmsg(const char *message, ...);

Matrix4x4::Matrix4x4() { set_identity(); }

void Matrix4x4::set_identity()
{
    a_[0] = 1.0f;
    a_[4] = 0.0f;
    a_[8] = 0.0f;
    a_[12] = 0.0f;
    a_[1] = 0.0f;
    a_[5] = 1.0f;
    a_[9] = 0.0f;
    a_[13] = 0.0f;
    a_[2] = 0.0f;
    a_[6] = 0.0f;
    a_[10] = 1.0f;
    a_[14] = 0.0f;
    a_[3] = 0.0f;
    a_[7] = 0.0f;
    a_[11] = 0.0f;
    a_[15] = 1.0f;
}

Matrix4x4::Matrix4x4(const Matrix4x4 &n)
{
    m00() = n.m00();
    m01() = n.m01();
    m02() = n.m02();
    m03() = n.m03();
    m10() = n.m10();
    m11() = n.m11();
    m12() = n.m12();
    m13() = n.m13();
    m20() = n.m20();
    m21() = n.m21();
    m22() = n.m22();
    m23() = n.m23();
    m30() = n.m30();
    m31() = n.m31();
    m32() = n.m32();
    m33() = n.m33();
}

Matrix4x4 &Matrix4x4::operator=(const Matrix4x4 &n)
{
    m00() = n.m00();
    m01() = n.m01();
    m02() = n.m02();
    m03() = n.m03();
    m10() = n.m10();
    m11() = n.m11();
    m12() = n.m12();
    m13() = n.m13();
    m20() = n.m20();
    m21() = n.m21();
    m22() = n.m22();
    m23() = n.m23();
    m30() = n.m30();
    m31() = n.m31();
    m32() = n.m32();
    m33() = n.m33();
    return *this;
}

bool Matrix4x4::operator==(const Matrix4x4 &n) const
{
    return (m00() == n.m00() && m01() == n.m01() && m02() == n.m02() && m03() == n.m03() &&
            m10() == n.m10() && m11() == n.m11() && m12() == n.m12() && m13() == n.m13() &&
            m20() == n.m20() && m21() == n.m21() && m22() == n.m22() && m23() == n.m23() &&
            m30() == n.m30() && m31() == n.m31() && m32() == n.m32() && m33() == n.m33());
}

void Matrix4x4::set(const float *m)
{
    for(size_t i = 0; i < 16; i++) a_[i] = m[i];
}

const float *Matrix4x4::get() const { return a_.data(); }

// Read-only access functions
float Matrix4x4::m00() const { return a_[0]; }
float Matrix4x4::m01() const { return a_[4]; }
float Matrix4x4::m02() const { return a_[8]; }
float Matrix4x4::m03() const { return a_[12]; }
float Matrix4x4::m10() const { return a_[1]; }
float Matrix4x4::m11() const { return a_[5]; }
float Matrix4x4::m12() const { return a_[9]; }
float Matrix4x4::m13() const { return a_[13]; }
float Matrix4x4::m20() const { return a_[2]; }
float Matrix4x4::m21() const { return a_[6]; }
float Matrix4x4::m22() const { return a_[10]; }
float Matrix4x4::m23() const { return a_[14]; }
float Matrix4x4::m30() const { return a_[3]; }
float Matrix4x4::m31() const { return a_[7]; }
float Matrix4x4::m32() const { return a_[11]; }
float Matrix4x4::m33() const { return a_[15]; }

// Read-write access functions
float &Matrix4x4::m00() { return a_[0]; }
float &Matrix4x4::m01() { return a_[4]; }
float &Matrix4x4::m02() { return a_[8]; }
float &Matrix4x4::m03() { return a_[12]; }
float &Matrix4x4::m10() { return a_[1]; }
float &Matrix4x4::m11() { return a_[5]; }
float &Matrix4x4::m12() { return a_[9]; }
float &Matrix4x4::m13() { return a_[13]; }
float &Matrix4x4::m20() { return a_[2]; }
float &Matrix4x4::m21() { return a_[6]; }
float &Matrix4x4::m22() { return a_[10]; }
float &Matrix4x4::m23() { return a_[14]; }
float &Matrix4x4::m30() { return a_[3]; }
float &Matrix4x4::m31() { return a_[7]; }
float &Matrix4x4::m32() { return a_[11]; }
float &Matrix4x4::m33() { return a_[15]; }

float Matrix4x4::m(uint32_t row, uint32_t col) const
{
    return (row < 4 && col < 4) ? a_[col * 4 + row] : 0.0f;
}

float &Matrix4x4::m(uint32_t row, uint32_t col)
{
    return (row < 4 && col < 4) ? a_[col * 4 + row] : a_[0];
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &n) const
{
    // Unroll the loop, do 1 row at a time.
    Matrix4x4 t;
    float     a0 = m00();
    float     a1 = m01();
    float     a2 = m02();
    float     a3 = m03();
    t.m00() = a0 * n.m00() + a1 * n.m10() + a2 * n.m20() + a3 * n.m30();
    t.m01() = a0 * n.m01() + a1 * n.m11() + a2 * n.m21() + a3 * n.m31();
    t.m02() = a0 * n.m02() + a1 * n.m12() + a2 * n.m22() + a3 * n.m32();
    t.m03() = a0 * n.m03() + a1 * n.m13() + a2 * n.m23() + a3 * n.m33();
    a0 = m10();
    a1 = m11();
    a2 = m12();
    a3 = m13();
    t.m10() = a0 * n.m00() + a1 * n.m10() + a2 * n.m20() + a3 * n.m30();
    t.m11() = a0 * n.m01() + a1 * n.m11() + a2 * n.m21() + a3 * n.m31();
    t.m12() = a0 * n.m02() + a1 * n.m12() + a2 * n.m22() + a3 * n.m32();
    t.m13() = a0 * n.m03() + a1 * n.m13() + a2 * n.m23() + a3 * n.m33();
    a0 = m20();
    a1 = m21();
    a2 = m22();
    a3 = m23();
    t.m20() = a0 * n.m00() + a1 * n.m10() + a2 * n.m20() + a3 * n.m30();
    t.m21() = a0 * n.m01() + a1 * n.m11() + a2 * n.m21() + a3 * n.m31();
    t.m22() = a0 * n.m02() + a1 * n.m12() + a2 * n.m22() + a3 * n.m32();
    t.m23() = a0 * n.m03() + a1 * n.m13() + a2 * n.m23() + a3 * n.m33();
    a0 = m30();
    a1 = m31();
    a2 = m32();
    a3 = m33();
    t.m30() = a0 * n.m00() + a1 * n.m10() + a2 * n.m20() + a3 * n.m30();
    t.m31() = a0 * n.m01() + a1 * n.m11() + a2 * n.m21() + a3 * n.m31();
    t.m32() = a0 * n.m02() + a1 * n.m12() + a2 * n.m22() + a3 * n.m32();
    t.m33() = a0 * n.m03() + a1 * n.m13() + a2 * n.m23() + a3 * n.m33();
    return t;
}

Matrix4x4 &Matrix4x4::operator*=(const Matrix4x4 &n)
{
    *this = *this * n;
    return *this;
}

Matrix4x4 &Matrix4x4::operator*=(float s)
{
    for(size_t r = 0; r < 16; r++) a_[r] *= s;
    return *this;
}

HPoint3 Matrix4x4::operator*(const HPoint3 &v) const
{
    return HPoint3((a_[0] * v.x + a_[4] * v.y + a_[8] * v.z + a_[12] * v.w),
                   (a_[1] * v.x + a_[5] * v.y + a_[9] * v.z + a_[13] * v.w),
                   (a_[2] * v.x + a_[6] * v.y + a_[10] * v.z + a_[14] * v.w),
                   (a_[3] * v.x + a_[7] * v.y + a_[11] * v.z + a_[15] * v.w));
}

HPoint3 Matrix4x4::operator*(const Point3 &v) const
{
    return HPoint3((a_[0] * v.x + a_[4] * v.y + a_[8] * v.z + a_[12]),
                   (a_[1] * v.x + a_[5] * v.y + a_[9] * v.z + a_[13]),
                   (a_[2] * v.x + a_[6] * v.y + a_[10] * v.z + a_[14]),
                   (a_[3] * v.x + a_[7] * v.y + a_[11] * v.z + a_[15]));
}

Vector3 Matrix4x4::operator*(const Vector3 &v) const
{
    return Vector3((a_[0] * v.x + a_[4] * v.y + a_[8] * v.z),
                   (a_[1] * v.x + a_[5] * v.y + a_[9] * v.z),
                   (a_[2] * v.x + a_[6] * v.y + a_[10] * v.z));
}

Ray3 Matrix4x4::operator*(const Ray3 &ray) const { return Ray3(*this * ray.o, *this * ray.d); }

Matrix4x4 &Matrix4x4::transpose()
{
    *this = get_transpose();
    return *this;
}

Matrix4x4 Matrix4x4::get_transpose() const
{
    Matrix4x4 t;
    t.m00() = m00();
    t.m01() = m10();
    t.m02() = m20();
    t.m03() = m30();
    t.m10() = m01();
    t.m11() = m11();
    t.m12() = m21();
    t.m13() = m31();
    t.m20() = m02();
    t.m21() = m12();
    t.m22() = m22();
    t.m23() = m32();
    t.m30() = m03();
    t.m31() = m13();
    t.m32() = m23();
    t.m33() = m33();
    return t;
}

void Matrix4x4::translate(float x, float y, float z)
{
    // The translation matrix
    Matrix4x4 t;
    t.m03() = x;
    t.m13() = y;
    t.m23() = z;

    // Right-multiply the current matrix
    *this *= t;
}

void Matrix4x4::scale(float x, float y, float z)
{
    // The scaling matrix
    Matrix4x4 s;
    s.m00() = x;
    s.m11() = y;
    s.m22() = z;

    // Right-multiply the current matrix
    *this *= s;
}

void Matrix4x4::rotate(float angle, float x, float y, float z)
{
    // Handle simple cases of rotation about a single axis
    if(x > 0.0f && y == 0.0f && z == 0.0f)
    {
        rotate_x(angle);
        return;
    }
    else if(y > 0.0f && x == 0.0f && z == 0.0f)
    {
        rotate_y(angle);
        return;
    }
    else if(z > 0.0f && x == 0.0f && y == 0.0f)
    {
        rotate_z(angle);
        return;
    }

    // Set up the standard rotation using quaternions

    // Scalar part
    float s = std::cos(degrees_to_radians(angle * 0.5f));

    // Vector part: a normalized direction vector
    Vector3 v(x, y, z);
    v.normalize();
    v *= std::sin(degrees_to_radians(angle * 0.5f));
    float a = v.x;
    float b = v.y;
    float c = v.z;

    // Set the composite rotation matrix
    Matrix4x4 r;
    r.m00() = 1.0f - 2.0f * b * b - 2.0f * c * c;
    r.m01() = 2.0f * a * b - 2.0f * s * c;
    r.m02() = 2.0f * a * c + 2.0f * s * b;
    r.m10() = 2.0f * a * b + 2.0f * s * c;
    r.m11() = 1.0f - 2.0f * a * a - 2.0f * c * c;
    r.m12() = 2.0f * b * c - 2.0f * s * a;
    r.m20() = 2.0f * a * c - 2.0f * s * b;
    r.m21() = 2.0f * b * c + 2.0f * s * a;
    r.m22() = 1.0f - 2.0f * a * a - 2.0f * b * b;
    r.m33() = 1.0f;

    // Right-multiply the current matrix
    *this *= r;
}

void Matrix4x4::rotate_x(float angle)
{
    // The rotation matrix
    Matrix4x4 r;
    float     radians = degrees_to_radians(angle);
    float     cosa = std::cos(radians);
    float     sina = std::sin(radians);
    r.m11() = cosa;
    r.m12() = -sina;
    r.m21() = sina;
    r.m22() = cosa;

    // Right-multiply the current matrix
    *this *= r;
}

void Matrix4x4::rotate_y(float angle)
{
    // The rotation matrix
    Matrix4x4 r;
    float     radians = degrees_to_radians(angle);
    float     cosa = std::cos(radians);
    float     sina = std::sin(radians);
    r.m00() = cosa;
    r.m02() = sina;
    r.m20() = -sina;
    r.m22() = cosa;

    // Right-multiply the current matrix
    *this *= r;
}

void Matrix4x4::rotate_z(float angle)
{
    // The rotation matrix
    Matrix4x4 r;
    float     radians = degrees_to_radians(angle);
    float     cosa = std::cos(radians);
    float     sina = std::sin(radians);
    r.m00() = cosa;
    r.m01() = -sina;
    r.m10() = sina;
    r.m11() = cosa;

    // Right-multiply the current matrix
    *this *= r;
}

Matrix4x4 Matrix4x4::get_inverse() const
{
    int32_t   j, k;
    int32_t   ind;
    float     v1, v2;
    Matrix4x4 t = *this;
    Matrix4x4 b;
    for(int32_t i = 0; i < 4; i++)
    {
        // Find pivot
        v1 = t.m(i, i);
        ind = i;
        for(j = i + 1; j < 4; j++)
        {
            if(std::abs(t.m(j, i)) > std::abs(v1))
            {
                ind = j;
                v1 = t.m(j, i);
            }
        }

        // Swap columns
        if(ind != i)
        {
            for(j = 0; j < 4; j++)
            {
                v2 = b.m(i, j);
                b.m(i, j) = b.m(ind, j);
                b.m(ind, j) = v2;
                v2 = t.m(i, j);
                t.m(i, j) = t.m(ind, j);
                t.m(ind, j) = v2;
            }
        }

        // The matrix is singular (has no inverse), set the inverse
        // to the identity matrix.
        if(v1 == 0.0f)
        {
            log_msg("InvertMatrix: Singular matrix");
            b.set_identity();
            return b;
        }

        for(j = 0; j < 4; j++)
        {
            t.m(i, j) /= v1;
            b.m(i, j) /= v1;
        }

        // Eliminate column
        for(j = 0; j < 4; j++)
        {
            if(j == i) continue;

            v1 = t.m(j, i);
            for(k = 0; k < 4; k++)
            {
                t.m(j, k) -= t.m(i, k) * v1;
                b.m(j, k) -= b.m(i, k) * v1;
            }
        }
    }
    return b;
}

void Matrix4x4::log(const char *str) const
{
    log_msg("  %s", str);
    log_msg("%.3f %.3f %.3f %.3f", m00(), m01(), m02(), m03());
    log_msg("%.3f %.3f %.3f %.3f", m10(), m11(), m12(), m13());
    log_msg("%.3f %.3f %.3f %.3f", m20(), m21(), m22(), m23());
    log_msg("%.3f %.3f %.3f %.3f", m30(), m31(), m32(), m33());
}

} // namespace cg
