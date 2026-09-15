#include "engine/core/registry.hpp"
#include "engine/core/time.hpp"
namespace engine
{
    Class Simulation
    {
        public:
            explicit Simulation(double fixed_dt = 1.0/60.0);
            Registry& registry();
            const Registry& registry() const;

            const SimulationTime& time() const;

            void update(doublt frame_dt);

            Entity fireWeapon(EntityFactory& factory, Entity weaponEntity, Entity owner, const cg::Point3& position, const cg::Vector3& direction);


        private:
            void step();

            Registry registry_;
            SimulationTime time_;

            double accumulator_ = 0.0;
    }
}