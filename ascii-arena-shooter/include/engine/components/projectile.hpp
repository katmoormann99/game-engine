#include "engine/core/entity.hpp"
namespace engine
{
    struct Projectile
    {
        Entity owner = 0;
        float damage = 1.0f;
    }
}