//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Original Author: David W. Nesbitt
//	Modified by Kat Moormann
//	File:    bounding_sphere.hpp
//	Purpose: Bounding Sphere.
//
//============================================================================

#ifndef __GEOMETRY_BOUNDING_SPHERE_HPP__
#define __GEOMETRY_BOUNDING_SPHERE_HPP__

#include "geometry/point3.hpp"

#include <vector>

namespace cg
{

/**
 * Sphere: center and radius.
 */
struct BoundingSphere
{
    Point3 center;
    float  radius;

    /**
     * Default constructor.
     */
    BoundingSphere();

    /**
     * Copy constructor
     * @param   s   Sphere to copy.
     */
    BoundingSphere(const BoundingSphere &s);

    /**
     * Constructor given a center point and radius.
     * @param  c  Center point.
     * @param  r  Radius.
     */
    BoundingSphere(const Point3 &c, float r);

    /**
     * Construct a sphere given a vertex list. Method by Ritter.
     * @param  vertex_list  Vertex list to surround with the sphere.
     */
    BoundingSphere(std::vector<Point3> &vertex_list);

    /**
     * Merge this bounding sphere with another to create the smallest sphere
     * containing the 2.
     * @param  s2  Sphere to merge with this sphere.
     * @return Reference to this ovbect sphere which combines this with s2
     */
    BoundingSphere &merge_with(const BoundingSphere &s2);

    /**
     * Merge this bounding sphere with another to create the smallest sphere
     * containing the 2.
     * @param  s2  Sphere to merge with this sphere.
     * @return BoundingSphere sphere which combines this with s2
     */
    BoundingSphere merge(const BoundingSphere &s2) const;
};

} // namespace cg

#endif
