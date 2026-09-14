//============================================================================
// Author: Kat Moormann
// File: spatial_grid.hpp
// Purpose: Partitions the simulation world into uniform 3D cells so nearby
//          entities can be found without scanning every entity.
// Date: June 24 2025
//============================================================================

#pragma once 

#include "engine/core/entity.hpp"
#include "geometry/point3.hpp"

#include <unordered_map>
#include <vector>


namespace engine
{
    // Integer coordinate of one grid cell 
    struct CellCoord
    {
        int x; 
        int y;
        int z;

        // Is this CellCoord the same as one I already have?
        bool operator==(const CellCoord& other) const
        {
            // return true only if x, y and z all match 
            return x == other.x && y == other.y && z == other.z;
        }


    };

    // Allows CellCoord to be used as a key in std::unordered_map
    struct CellCoordHash
    {
        std::size_t operator()(const CellCoord &cell) const;
    };

    class SpatialGrid
    {
        public:
            // cellSize controls the width/height/depth of every grid cell
            explicit SpatialGrid(float cellSize);

            // remove all entites from the grid
            void clear();

            // Add an entity to the cell containing its position
            void insert(Entity entity, const cg::Point3 &position);

            // Find entities stored in cells near a position
            void queryNearby(const cg::Point3& position, float radius, std::vector<Entity>& results) const; 

        private: 
            // Convert a world-space position into a grid cell coordinate
            CellCoord positionToCell(const cg::Point3 &position) const;

            float cellSize_;

            // Each cell stores the entities currently inside it
            std::unordered_map<CellCoord, std::vector<Entity>, CellCoordHash> cells_;

    };
}