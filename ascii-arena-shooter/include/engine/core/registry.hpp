#include "engine/core/component_storage.hpp"
namespace engine
{
    class Registry
    {
        public:
            Entity create()
            {
                // generate and entity ID
                return Entity{nextEntity_++};
            }

            void destroy(Entity entity){
                // implement later
            };

        private:
        Entity nextEntity_ = 1;
    };
}