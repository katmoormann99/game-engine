//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin

//	Original Author: David W. Nesbitt
//	Modified by Kat Moormann
//	File:    matrix.hpp
//	Purpose: Matrix class
//============================================================================

#ifndef __GEOMETRY_MATRIX_HPP__
#define __GEOMETRY_MATRIX_HPP__

#include "hpoint3.hpp"
#include "point3.hpp"
#include "ray3.hpp"
#include "vector3.hpp"

#include <array>
#include <cstdint>

namespace cg
{

/**
 * 4x4 matrix. All matrix elements (row, col) are indexed base 0.
 */
class Matrix4x4
{
  public:
    /**
     * Constructor.  Sets the matrix to the identity matrix
     */
    Matrix4x4();

    /**
     * Sets the matrix to the identity matrix.
     */
    void set_identity();

    /**
     * Copy constructor
     * @param  n  Matrix to copy
     */
    Matrix4x4(const Matrix4x4 &n);

    /**
     * Assignment operator
     * @param   n  Matrix to assign to this matrix
     * @return  Returns the address of this matrix.
     */
    Matrix4x4 &operator=(const Matrix4x4 &n);

    /**
     * Equality operator
     * @param   n  Matrix to test for equality with this matrix.
     * @return  Returns true if hte matrices are equal, false otherwise..
     */
    bool operator==(const Matrix4x4 &n) const;

    /**
     * Set the matrix to the values specified in the array.
     * @param  m  Array of float values to fill in this matrix. The
     *            elements are arranged in column order.
     */
    void set(const float *m);

    /**
     * Gets the matrix (can be passed to OpenGL - GLSL mat4)
     * @return   Returns the elements of this matrix in column order.
     */
    const float *get() const;

    // Read-only access functions
    float m00() const;
    float m01() const;
    float m02() const;
    float m03() const;
    float m10() const;
    float m11() const;
    float m12() const;
    float m13() const;
    float m20() const;
    float m21() const;
    float m22() const;
    float m23() const;
    float m30() const;
    float m31() const;
    float m32() const;
    float m33() const;

    // Read-write access functions
    float &m00();
    float &m01();
    float &m02();
    float &m03();
    float &m10();
    float &m11();
    float &m12();
    float &m13();
    float &m20();
    float &m21();
    float &m22();
    float &m23();
    float &m30();
    float &m31();
    float &m32();
    float &m33();

    /**
     * Gets a matrix element given by row,column.
     * @param  row   Matrix row
     * @param  col   Matrix column
     * @return Returns the element at the specified row,col.
     */
    float m(uint32_t row, uint32_t col) const;

    /**
     * Gets a matrix element given by row,column.
     * @param  row   Matrix row (0-based)
     * @param  col   Matrix col (0-based)
     * @return Returns the address of the element at the specified row,col.
     */
    float &m(uint32_t row, uint32_t col);

    /**
     * Matrix multiplication.  Multiplies the current matrix by the matrix n
     * ( m' = m n ) and returns the resulting 4x4 matrix.
     * @param   n   Matrix to multiply the current matrix by
     * @return  Returns the product of the current matrix and the supplied matrix.
     */
    Matrix4x4 operator*(const Matrix4x4 &n) const;

    /**
     * Matrix multiplication.  Multiplies the current matrix by the matrix n
     * ( m = m n ) and stores the result in m.
     * @param   n  Matrix to multiply the current matrix by
     * @return  Returns the address of the current matrix.
     */
    Matrix4x4 &operator*=(const Matrix4x4 &n);

    /**
     * Matrix multiplication by a scalar.  Multiplies the current matrix
     * by the scalar s.
     * @param  s   Scalar to multiply the current matrix by
     * @return     Returns the address of the current matrix.
     */
    Matrix4x4 &operator*=(float s);

    /**
     * Transforms a coordinate by the matrix.
     * @param   v  Homogeneous point.
     * @return  Returns the transformed homogeneous coordinate position.
     */
    HPoint3 operator*(const HPoint3 &v) const;

    /**
     * Transforms a coordinate by the matrix.  Returns a homogeneous
     * coordinate position.  Assumes the w coordinate is 1.
     * @param   v  3D point to transform.
     * @return  Returns the transformed point.
     */
    HPoint3 operator*(const Point3 &v) const;

    /**
     * Transforms a vector (normal or direction) by the matrix.
     * Only the upper 3x3 portion of the matrix is used (no translation).
     * @param   v  3D vector to transform.
     * @return  Returns the transformed direction.
     */
    Vector3 operator*(const Vector3 &v) const;

    /**
     * Transforms a ray by the matrix.  Transforms the ray origin and
     * ray direction.
     * @param   ray   Ray to transform
     * @return  Returns the transformed ray.
     */
    Ray3 operator*(const Ray3 &ray) const;

    /**
     * Transposes the current matrix.
     * @return   Returns the address of the current matrix.
     */
    Matrix4x4 &transpose();

    /**
     * Calculates the transpose of the current 4x4 matrix and returns it.
     * The current matrix is unchanged.
     * @return   Returns the transpose of the current matrix.
     */
    Matrix4x4 get_transpose() const;

    // The following convenience methods allow creation of a composite
    // modeling transfomation. Each method right-multiplies the curent
    // matrix (similar to OpenGL)

    /**
     * Applies a translation to the current transformation matrix.
     * Instead of doing a standard matrix multiplication call, we just reset
     * the elements of the matrix that will change due to the translation.
     * @param	x	   x translation
     * @param	y	   y translation
     * @param	z	   z translation
     */
    void translate(float x, float y, float z);

    /**
     * Applies a scaling to the current transformation matrix.
     * Right-multiplies the current matrix.
     * @param	x	   x scaling
     * @param	y	   y scaling
     * @param	z	   z scaling
     */
    void scale(float x, float y, float z);

    /**
     * Performs a counterclockwise rotation about the specified axis.
     * Right-multiplies the current matrix.
     * @param   angle    Angle (degrees) for the rotation.
     * @param   x        x coordinate of the axis of rotation
     * @param   y        y coordinate of the axis of rotation
     * @param   z        z coordinate of the axis of rotation
     */
    void rotate(float angle, float x, float y, float z);

    /**
     * Performs a counterclockwise rotation about the x axis.
     * Right-multiplies the current matrix.
     * @param   angle    Angle (degrees) for the rotation.
     */
    void rotate_x(float angle);

    /**
     * Performs a counterclockwise rotation about the x axis.
     * Right-multiplies the current matrix.
     * @param   angle    Angle (degrees) for the rotation.
     */
    void rotate_y(float angle);

    /**
     * Performs a counterclockwise rotation about the x axis.
     * Right-multiplies the current matrix.
     * @param   angle    Angle (degrees) for the rotation.
     */
    void rotate_z(float angle);

    /**
     * Calculates the inverse of the current 4x4 matrix and returns it.
     * @return  Returns the inverse of the current matrix.
     */
    Matrix4x4 get_inverse() const;

    /**
     * Logs a message followed by the matrix.
     * @param   str   String to print to log file
     */
    void log(const char *str) const;

  private:
    // Elements of the matrix. Column order.
    std::array<float, 16> a_;
};

} // namespace cg

#endif
