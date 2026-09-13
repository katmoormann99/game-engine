//============================================================================
// Author: Kat Moormann
// File: spatial_grid.cpp
// Purpose: Implements uniform-grid insertion and nearby-entity queries for
//          efficient spatial lookup.
// Date: June 24 2025
//============================================================================

#include "engine/spatial/spatial_grid.hpp"

#include <cmath>
#include <cstddef>


namespace engine
{
    std::size_t CellCoordHash::operator() (const CellCoord &cell) const
    {

        /**
         * cell.x = 2  → hx = 2
         * cell.y = 5  → hy = 5
         * cell.z = 1  → hz = 1
         * 
         * hx = 2
         * hy << 1 = 5 shifted left 1 bit = 10
         * hz << 2 = 1 shifted left 2 bits = 4
         * 
         * 2^10^4 = 12
         * 
         * 1. Hash {2,5,1} into a number
         * 2. Use that number to jump to the right area
         * 3. Use operator== to confirm it found {2,5,1}
         * 4. Return the entities stored there
        */

        // Combine the hashes of the three integer coordinates
        std::size_t hx = std::hash<int>{}(cell.x);
        std::size_t hy = std::hash<int>{}(cell.y);
        std::size_t hz = std::hash<int>{}(cell.z);

        return hx ^ (hy << 1) ^ (hz << 2);
    }

    SpatialGrid::SpatialGrid(float cellSize) : cellSize_(cellSize){};

    void SpatialGrid::clear()
    {
        cells_.clear();
    }

    CellCoord SpatialGrid::positionToCell(const cg::Point3 &position) const 
    {
        // floor() matters for negative coordinates
        return {
            static_cast<int>(std::floor(position.x/cellSize_)),
            static_cast<int>(std::floor(position.y/cellSize_)),
            static_cast<int>(std::floor(position.z/cellSize_))
        };

    };

    void SpatialGrid::insert(Entity entity,const cg::Point3& position)
    {
        CellCoord cell = positionToCell(position);

        cells_[cell].push_back(entity);
    }


    std::vector<Entity> SpatialGrid::queryNearby(const cg::Point3& position,float radius) const
    {
        std::vector<Entity> results;

        CellCoord center = positionToCell(position);

        // Determine how many cells the search radius reaches.
        int cellRadius = static_cast<int>(std::ceil(radius / cellSize_));

        // Search the neighboring cells.
        for (int x = center.x - cellRadius; x <= center.x + cellRadius; ++x){
            for (int y = center.y - cellRadius; y <= center.y + cellRadius; ++y){
                for (int z = center.z - cellRadius; z <= center.z + cellRadius; ++z){
                    CellCoord cell{x, y, z};

                    auto it = cells_.find(cell);

                    if (it == cells_.end())
                    {
                        continue;
                    }

                    // Add entities from this cell as possible candidates.
                    for (Entity entity : it->second)
                    {
                        results.push_back(entity);
                    }
                }
            }
        }

        return results;
    };

}