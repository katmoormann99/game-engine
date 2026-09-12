//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Original Author: David W. Nesbitt
//	Modified by Kat Moormann
//	File:    point3.hpp
//	Purpose: Structures to hold 3D coordinates and homogenous coordinates
//
//============================================================================

#pragma once

#include <vector>

namespace cg
{

// Forward Declarations
struct Vector3;

/**
 * 3D Point (cartesian)
 */
struct Point3
{
    float x;
    float y;
    float z;

    /**
     * Default constructor
     */
    Point3();

    /**
     * Constructor with initial values for x,y,z.
     * @param   ix   x coordinate position.
     * @param   iy   y coordinate position.
     * @param   iz   z coordinate position.
     */
    Point3(float ix, float iy, float iz);

    /**
     * Copy constructor.
     * @param   p   Point to copy to the new point.
     */
    Point3(const Point3 &p);


    /**
     * Assignment operator
     * @param   p   Point to assign to this point.
     * @return   Returns the address of this point.
     */
    Point3 &operator=(const Point3 &p);

    /**
     * Set the coordinate components to the specified values.
     * @param   ix   x coordinate position.
     * @param   iy   y coordinate position.
     * @param   iz   z coordinate position.
     */
    void set(float ix, float iy, float iz);

    /**
     * Equality operator.
     * @param   p  Point to compare to the current point.
     * @return  Returns true if two points are equal, false otherwise.
     */
    bool operator==(const Point3 &p) const;

    /**
     * Affine combination of this point with another point. 2 scalars are provided
     * (a0 and a1) and the must add to 1.
     * @param  a0  Scalar for this point
     * @param  a1  Scalar for p1
     * @param  p1  Point 1
     */
    Point3 affine_combination(float a0, float a1, const Point3 &p1) const;

    /**
     * Gets the midpoint on a line segment between this point and point p1.
     * @param  p1  Point
     * @return  Returns the midpoint between this point and p1.
     */
    Point3 mid_point(const Point3 &p1) const;

    /**
     * Test if a point is inside a 3D polygon. Uses the normal to the
     * polygon to project to one of the 2D axis planes and solves in that
     * space. This method assumes that the point is on the plane of the
     * polygon (generally found by intersecting a ray with a plane).
     * @param   polygon  Polygon to test
     * @param   n        Normal to the polygon
     * @return  Returns true if the point is inside the polygon, false if not.
     */
    bool is_in_polygon(const std::vector<Point3> &polygon, const Vector3 &n) const;

    /**
     * Add a vector to the current point.
     * @param   v  Vector to add to the current point.
     * @return  Returns a new point: the result of the current point
     *          plus the specified vector.
     */
    Point3 operator+(const Vector3 &v) const;

    /**
     * Subtract a vector from the current point.
     * @param   v  Vector to subtract from the current point.
     * @return  Returns a new point: the result of the current point
     *          minus the specified vector.
     */
    Point3 operator-(const Vector3 &v) const;

    /**
     * Subtraction of a point from the current point.
     * @param   p to subtract from the current point.
     * @return  Returns a vector.
     */
    Vector3 operator-(const Point3 &p) const;

  protected:
    // Test if point is inside polygon: drop the z component when making the
    // tests. Shoots a test ray along +x axis.  The strategy is to compare
    // polygon vertex y values to the test point y and quickly discard edges
    // entirely to one side of the test ray.
    bool is_in_polygon_XY(const std::vector<Point3> &polygon) const;

    // Test if point is inside polygon: drop the y component when making the
    // tests. Shoots a test ray along +x axis.  The strategy is to compare
    // polygon vertex z values to the test point z and quickly discard edges
    // entirely to one side of the test ray.
    bool is_in_polygon_XZ(const std::vector<Point3> &polygon) const;

    // Test if point is inside polygon: drop the x component when making the
    // tests. Shoots a test ray along +y axis.  The strategy is to compare
    // polygon vertex z values to the test point z and quickly discard edges
    // entirely to one side of the test ray.
    bool is_in_polygon_YZ(const std::vector<Point3> &polygon) const;
};

} // namespace cg


