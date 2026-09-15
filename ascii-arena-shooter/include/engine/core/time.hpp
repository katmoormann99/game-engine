#include <cstdint>
namespace engine
{
    struct SimulationTime
    {
        double fixed_dt = 1.0/60.0; // 60Hz
        double elapsed = 0.0;

        std::uint64_t tick = 0;

        void advance()
        {
            elapsed += fixed_dt;
            ++tick;
        }
        


    };
}