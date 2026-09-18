#pragma once 

namespace engine
{
    // Remember the system does not own the entity - the registry does 
    // The system must access the entity by a non owning reference
    class Registry;

    class ParticleSystem
    {
        public:
            void updat(Registry &registry, double dt);
    };
} // namespace engine