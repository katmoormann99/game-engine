#include "engine/core/entity.hpp"
#include <vector>
#include <unordered_map>
#include <cstddef>

// Think of ComponentStorage<T> as answering two kinds of questions
// 1. Entity Focused - Does Entity 7 have a Position? Give me its Position.
// 2. System Focused - Give me all Positions so I can process them efficiently 

namespace engine {

    template <typename T> 
    class ComponentStorage{
        public:
            // Gives an entity a component of type T, or replaces the existing one
            // registry_.transforms().add(entity, Transform{position});
            void add(Entity entity, const T &component)
            {
                auto it = entityToIndex_.find(entity);

                if (it != entityToIndex_.end())
                {
                    // replace the current entity component data 
                    components_[it->second] = component;
                    return;
                }

                const std::size_t index = components_.size();

                components_.push_back(component);
                entities_.push_back(entity);
                entityToIndex_[entity] = index;
            }

            // Checks whether or not an entity has this component before trying to access it
            // if (!transforms.has(entityA) || !velocities.has(entityA)) { continue; }
            bool has(Entity entity) const{
                auto it = entityToIndex_.find(entity);
                if (it != entityToIndex_.end())
                {
                    return true;
                }
                return false;
            };

            // Gets the component and allows the caller to modify it 
            // MovementSystem getting a Transform so it can update x and y
            // Transform &transform = transforms.get(entity);
            // transform.position = transform.position + velocity.linear * static_cast<float>(dt);
            T& get (Entity entity){
                const std::size_t index = entityToIndex_.at(entity);
                // this allows for something like
                // positions.get(player).position.x = 10.0f;
                return components_.at(index);
            };

            // Gets the component for reading ONLY 
            // A system that only needs to inspet data, like checking a target's position
            // without changing it 
            // const Transform& transformA = transforms.get(entityA);
            // spatialGrid_.queryNearby(transformA.position, searchRadius, nearby);
            const T& get(Entity entity) const{
                const std::size_t index = entityToIndex_.at(entity);
                // this allows for something like
                // const Position &p = positions.get(player)
                return components_.at(index);
            };

            // Gives mutable access to all components of this type 
            // A system processing every Velocity, Position, etc, in a tight loop and modifying them
            std::vector<T>& components()
            {
                return components_;
            };

            // Gives read-only access to all components of this type
            // Profiling, rendering, debugging 
            const std::vector<T>& components() const
            {
                return components_;
            };

            // Tells you which entity belongs to each element in components_
            // const auto &entities = velocities.entities();
            const std::vector<Entity>& entities() const
            {
                return entities_;
            }

            // Remove this entity's component of type T from this storage
            void remove (Entity entity)
            {
                auto it = entityToIndex_.find(entity);

                if (it == entityToIndex_.end())
                {
                    return;
                }
                const std::size_t removedIndex = it->second;
                const std::size_t lastIndex = components_.size() - 1;

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
            std::unordered_map<Entity, std::size_t> entityToIndex_;
    };
}