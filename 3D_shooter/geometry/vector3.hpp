//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Original Author: David W. Nesbitt
//	Modified by Kat Moormann
//	File:    vector3.hpp
//	Purpose: 3D Vector class.
//
//============================================================================

#ifndef __GEOMETRY_VECTOR3_HPP__
#define __GEOMETRY_VECTOR3_HPP__

#include "geometry/point3.hpp"

namespace cg
{

/**
 * 3D vector class
 */
struct Vector3
{
    float x;
    float y;
    float z;

    /**
     * Default constructor
     */
    Vector3();

    /**
     * Constructor given a point.  Essentially a vector from the
     * origin to the point.
     * @param   p  Point.
     */
    Vector3(const Point3 &p);

    /**
     * Constructor given 3 components of the vector.
     * @param   ix   x component of the vector.
     * @param   iy   y component of the vector.
     * @param   iz   z component of the vector.
     */
    Vector3(float ix, float iy, float iz);

    /**
     * Constructor from one point to another.
     * @param   from  Point at origin of the vector.
     * @param   to    Point at end of vector
     */
    Vector3(const Point3 &from, const Point3 &to);

    /**
     * Copy constructor.
     * @param   w  Vector to copy to the new vector.
     */
    Vector3(const Vector3 &w);

    /**
     * Assignment operator
     * @param   w  Vector to copy to the current vector.
     * @return  Returns the address of the current vector.
     */
    Vector3 &operator=(const Vector3 &w);

    /**
     * Set the current vector to the specified components.
     * @param   ix   x component of the vector.
     * @param   iy   y component of the vector.
     * @param   iz   z component of the vector.
     */
    void set(float ix, float iy, float iz);

    /**
     * Set the vector components to those of a point.  Essentially a
     * vector from the origin to the point.
     * @param   p  Point.
     */
    void set(const Point3 &p);

    /**
     * Set the current vector to be from one point to another.
     * @param   from  Point at origin of the vector.
     * @param   to    Point at end of vector
     */
    void set(const Point3 &from, const Point3 &to);

    /**
     * Creates a new vector that is the current vector plus the
     * specified vector.
     * @param   w  Vector to add to the current vector.
     * @return   Returns the resulting vector.
     */
    Vector3 operator+(const Vector3 &w) const;

    /**
     * Adds vector w to the current vector.
     * @param   w  Vector to add to the current vector.
     * @return  Returns the address of the current vector.
     */
    Vector3 &operator+=(const Vector3 &w);

    /**
     * Creates a new vector that is the current vector minus the
     * specified vector.
     * @param   w  Vector to subtract from the current vector.
     * @return   Returns the resulting vector.
     */
    Vector3 operator-(const Vector3 &w) const;

    /**
     * Subtracts vector w from the current vector.
     * @param   w  Vector to subtract from the current vector.
     * @return  Returns the address of the current vector.
     */
    Vector3 &operator-=(const Vector3 &w);

    /**
     * Creates a new vector that is the current vector multiplied
     * with the specified scalar.
     * @param   scalar   Scalar to muliply the vector with.
     * @return  Returns the resulting vector
     */
    Vector3 operator*(float scalar) const;

    /**
     * Multiplies the current vector by a scalar
     * @param   scalar   Scalar to muliply the vector with.
     * @return  Returns the address of the current vector.
     */
    Vector3 &operator*=(float scalar);

    /**
     * Equality operator.
     * @param   w  Vector to test if equal to the current vector.
     * @return  Returns true if vector w equals the current vector,
     *          false otherwise.
     */
    bool operator==(const Vector3 &w) const;

    /**
     * Computes the dot product of the current vector with the
     * specified vector.
     * @param   w  Vector
     * @return  Returns the dot product (a scalar).
     */
    float dot(const Vector3 &w) const;

    /**
     * Computes the cross product of current vector with w
     * @param   w  Vector to take the cross product with (current X w)
     * @return  Returns the resulting vector.
     */
    Vector3 cross(const Vector3 &w) const;

    /**
     * Computes the norm (length) of the current vector.
     * @return  Returns the length of the vector.
     */
    float norm() const;

    /**
     * Computes the squared norm of a vector
     * (Useful when absolute distance is not required)
     * @return  Returns the length squared of the vector.
     */
    float norm_squared() const;

    /**
     * Normalizes the vector.
     * @return  Returns the address of the current vector.
     */
    Vector3 &normalize();

    /**
     * Calculates the component of the current vector along the
     * specified vector
     * @param   w  Vector to determine component along.
     * @return  Returns the component of the current vector along w.
     */
    float component(const Vector3 &w) const;

    /**
     * Creates a new vector that is the projection of the current
     * vector along the specified vector.
     * @param   w  Vector to determine projection along.
     * @return  Returns the new vector.
     */
    Vector3 projection(const Vector3 &w) const;

    /**
     * Calculates the angle (radians) between the current vector and
     * the specified vector.
     * @param   w  Vector to determine angle from current vector.
     * @return  Returns the angle in radians between the two vectors.
     */
    float angle_between(const Vector3 &w) const;

    /**
     * Reflects the current vector given a normal to the reflecting surface.
     * Assumes the normal is unit length.  Note that if done properly the
     * magnitude of the reflected vector will equal the magnitude of the
     * incoming vector.
     * @param   normal   unit length normal to the plane where reflection occurs
     * @return  Returns the reflected vector
     */
    Vector3 reflect(const Vector3 &normal) const;
};

/**
 * Overloading: allows float * Vector3
 */
Vector3 operator*(float s, const Vector3 &v);

} // namespace cg

#endif
