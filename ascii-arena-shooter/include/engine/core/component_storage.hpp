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
            void add(Entity entity, const T &component){}

            // Checks whether or not an entity has this component before trying to access it
            // if (!transforms.has(entityA) || !velocities.has(entityA)) { continue; }
            bool has(Entity entity const){};

            // Gets the component and allows the caller to modify it 
            // MovementSystem getting a Transform so it can update x and y
            // Transform &transform = transforms.get(entity);
            // transform.position = transform.position + velocity.linear * static_cast<float>(dt);
            T& get (Entity entity){};

            // Gets the component for reading ONLY 
            // A system that only needs to inspet data, like checking a target's position
            // without changing it 
            // const Transform& transformA = transforms.get(entityA);
            // spatialGrid_.queryNearby(transformA.position, searchRadius, nearby);
            const T& get(Entity entity){};

            // Gives mutable access to all components of this type 
            // A system processing every Velocity, Position, etc, in a tight loop and modifying them
            std::vector<T>& components();

            // Gives read-only access to all components of this type
            // Profiling, rendering, debugging 
            const std::vector<T>& components() const;

            // Tells you which entity belongs to each element in components_
            // const auto &entities = velocities.entities();
            const std::vector<Entity>& entities() const;

            // Remove this entity's component of type T from this storage
            void remove (Entity entity);

        private:
            std::vector<T> components_;
            std::vector<Entity> entities_;
            std::unordered_map<Entity, std::size_t> entityToIndex_;
    };
}