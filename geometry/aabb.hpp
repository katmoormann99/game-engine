//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Original Author: David W. Nesbitt
//	Modified by Kat Moormann
//	File:    aabb.hpp
//	Purpose: Axis Aligned Bounding Box (3D).
//============================================================================

#ifndef __GEOMETRY_AABB_HPP__
#define __GEOMETRY_AABB_HPP__

#include "geometry/point3.hpp"
#include "geometry/vector3.hpp"

#include <vector>

namespace cg
{

/**
 * Axis Aligned Bounding Box.
 */
struct AABB
{

    Point3 min; // Minimum x y z corner
    Point3 max; // Maximum x y z corner
    Point3 center; // Center of the box
    Vector3 half_diagonal; // Half diagonal (extent from center)

    /**
     * Default constructor.
     */
    AABB();

    /**
     * Construct an AABB given a minimum and maximum point.
     * @param  min  Minimum point (x,y,z)
     * @param  max  Maximum point (x,y,z)
     */
    AABB(const Point3 &min, const Point3 &max);

    /**
     * Construct an AABB given a vertex list.
     * @param  vertex_list  Vertex list.
     */
    AABB(const std::vector<Point3> &vertex_list);

    /**
     * Creates an AABB given a vertex list.
     * @param  vertex_list  Vertex list.
     */
    void create(const std::vector<Point3> &vertex_list);

    /**
     * Updates the AABB given new minimum and maximum points.
     * @param  min  Minimum point (x,y,z)
     * @param  max  Maximum point (x,y,z)
     */
    void update(const Point3 &min, const Point3 &max);

    /**
     * Merge this bbox with another another. Update this box.
     * @param  box  Other box to merge.
     */
    void merge(const AABB &box);

    /**
     * Get the point at the minimum x,y,z.
     * @return  Returns the min. point.
     */
    Point3 min_pt() const;

    /**
     * Get the point at the maximum x,y,z.
     * @return  Returns the max. point.
     */
    Point3 max_pt() const;

    /**
     * Compute center and half diagonal
     */
    void compute_center();
};

} // namespace cg

#endif
