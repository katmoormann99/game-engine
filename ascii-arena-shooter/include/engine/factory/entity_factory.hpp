 #include "engine/core/registry.hpp"
 #include "geometry/point3.hpp"
 #include "geometry/vector3.hpp"

 namespace engine
 {
    public:
        explicit EntityFactory(Registry &registry);

    private:
        Registry &registry_;

 }