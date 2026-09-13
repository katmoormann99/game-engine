//============================================================================
// Author: Kat Moormann 
// File: component_storage.hpp
// Purpose: Defines type specific component storage that keeps component data 
// contiguous in memory for efficient iteration and lookup
// Date: June 24 2025
//============================================================================

/**
 * The core idea for this entity class is
 * 1. Each Entity can own one component of type T
 * 2. Components are stored tightly in a std::vector<T>
 * 3. Entities are stored in parallel std::vector<Entity>
 * 4. An unordered_map lets us quickly find which vector index belongs to an entity 
 * 
 * So if T is Position, this becomes storage for Position components. 
 * If T is Velocity, it becomes a completely separate storage for velocities 
 */


/** 
 * Suppose we have 10,000 entites that all have the Velocity component 
 * Instead of having 10,000 scattered heap-allocated objects such as: 
 * Entity* -> Object -> Velocity 
 * This class allows us to have component storage 
 * Velocity storage
 *   [Velocity]
 *   [Velocity]
 *   [Velocity]
 *   [Velocity]
 *   [Velocity]
 * This means the movement system can iterate sequentially through memory, which is generally much nicer for CPU caches! 
 * Data oriented design! Data oriented design! Data oriented design! 
*/

#pragma once 

#include "entity.hpp"
#include <cstddef>
#include <unordered_map>
#include <vector>
#include <utility>

#include <iostream>
#include <typeinfo>

namespace engine {


    /**
     * Component storage is NOT one concrete class - it is a class template 
     * Thinking of T as a PLACEHOLDER
     * 
     * Down the road we might need to define: 
     * 
     * struct Position {
     *      float x;
     *      float y;
     * };
     * 
     * struct Position {
     *      float x;
     *      float y;
     * };
     * 
     * Then: 
     * ComponentStorage<Position> positionStorage;
     * ComponentStorage<Velocity> velocityStorage;
     * 
     * The compiler will generate two different classes for us 
     * 
     * TEMPLATES LET ONE IMPLEMENTATION WORK FOR ARBITRARY COMPONENT TYPES
     */
    template <typename T>
    class ComponentStorage{
        public:
            // const T &component: Give me a reference to a T, and I promose not to modify it
            // prevents a copy which is inefficient
            void add(Entity entity, const T &component)
            {   
                // Find an existing entity 
                auto it = entityToIndex_.find(entity);
                
                // Replace component 
                // if (entity exists in map)
                if (it != entityToIndex_.end()){
                    // adding a component to an entity that already has one replaces the old component
                    components_[it->second] = component;

                    std::cout << "[STORAGE] Entity" << entity << " replaced component " << typeid(T).name() << " at index " << it->second << std::endl;
                    return;
                }

                // Adding a new component - if the entity wasn't found 
                // suppose the vector currently contains 4 components then components_.size() returns 4
                // that is the exact index where the next pushed element will appear 
                const std::size_t index = components_.size();

                // Parallel array relationship
                components_.push_back(component);
                entities_.push_back(entity);

                // store the lookup entity -> 4
                entityToIndex_[entity] = index;
                std::cout << "[STORAGE] Entity" << entity << " -> " << typeid(T).name() << " storage[index " << index << "]" << std::endl;
            }

            bool has(Entity entity) const
            {
                // positionStorage.has(player) - Does player have Position? True
                // velocityStorage.has(player) - Does player have Velocity? True
                auto it = entityToIndex_.find(entity);
                
                if (it != entityToIndex_.end()){
                    // this entity does have this component 
                    return true;
                }
                return false;
            }
 
            // returning a reference to the actual stored component 
            T& get (Entity entity)
            {
                // this function is for mutable storage so that 
                // storage.get(entity).x = 50 works 

                std::size_t index = entityToIndex_.at(entity);
                T &component = components_.at(index);
                return component;
            }

            const T& get(Entity entity) const
            {
                // nonmutable storage 
                // so something like 'float x = storage.get(entity).' is allowed but NOT 
                // something like 'storage.get(entity).x = 50'
                std::size_t index = entityToIndex_.at(entity);
                const T &component = components_.at(index);
                return component;
            }


            /**
             * Example of why a modifiable reference would be needed 
             * for (Position& pos : positions.components()) {
             *      pos.x += 5;
             *  }
             */
            std::vector<T>& components()
            {
                // give me a modifiable reference to the vector 
                return components_;
            }

            /*
            * Example of a non-modifable reference
            *    void render(const ComponentStorage<Position>& positions)
            *    {
            *        for (const Position& pos : positions.components()) {
            *            drawSprite(pos.x, pos.y);
            *        }
            *    }
            */
            // You could technically use a modifable reference everywhere but you'd lose protection against accidental changes
            const std::vector<T>& components() const
            {
                // give me a read only reference to the vector 
                return components_;
            }

            const std::vector<Entity>& entities() const
            {
                return entities_;
            }

            // Swap and pop approach for removal 
            // This approach gives us O(1)-average removal while preserving dense 
            // component storage, but it does NOT preserve component ordering
            void remove(Entity entity)
            {
                auto it = entityToIndex_.find(entity);

                // Entity does not have component of this type 
                if (it == entityToIndex_.end())
                {
                    return;
                }

                const std::size_t removedIndex = it->second;
                const std::size_t lastIndex = components_.size() - 1;

                /**
                 *   INDEX        ENTITY       COMPONENT
                 *   -----        ------       ---------
                 *   0            10          Velocity A
                 *   1            42          Velocity B
                 *   2            71          Velocity C
                 *   3            99          Velocity D
                 * 
                 * and the entityToIndex_ map contains 
                 * 10 -> 0
                 * 42 -> 1
                 * 71 -> 2
                 * 99 -> 3
                 * 
                 * Hypothetically we call velocities.remove(42)
                 * 
                 * removedIndex = it->second = 1
                 * lastIndex = components_.size() - 1 = 3
                 * 
                 * components_[1] = std::move(components_[3])
                 * Entity movedEntity = entities_[3] = 99
                 * 
                 * Substitute the values:
                 * entities_[1] = 99
                 * 
                 *   BEFORE
                 *   components_: [A]  [B]  [C]  [D]
                 *   entities_:   [10] [42] [71] [99]
                 *   
                 *   AFTER MOVING D
                 *   components_: [A]  [D]  [C]  [D*]
                 *   entities_:   [10] [99] [71] [99]
                 * 
                 * entityToIndex_[99] = 1
                */

                // If we're not removing the last component, move the last component into the removed slot
                if (removedIndex != lastIndex)
                {
                    components_[removedIndex] = std::move(components_[lastIndex]);

                    Entity movedEntity = entities_[lastIndex];
                    entities_[removedIndex] = movedEntity;

                    // The moved entity now lives at removedIndex
                    entityToIndex_[movedEntity] = removedIndex;
                }

                components_.pop_back();
                entities_.pop_back();

                entityToIndex_.erase(it);
            }


        private:
            std::vector<T> components_;
            std::vector<Entity> entities_;
            // Key, value
            // Entity 7 -> index 0
            // Entity 12 -> index 1
            // Entity 99 -> index 2
            std::unordered_map<Entity, std::size_t> entityToIndex_;

    };

}