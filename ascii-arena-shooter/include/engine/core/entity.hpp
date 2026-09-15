#include <cstdint>

namespace engine
{
    struct Entity
    {
        std::uint32_t id = 0;
    };
    constexpr Entity INVALID_ENTITY = 0;
}