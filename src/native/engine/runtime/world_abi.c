#include "world_abi.h"

uint32_t runtime_world_abi_spawn(World *world, int entity_type, float x, float y, float z)
{
    if (!world)
        return 0;

    return world_spawn_entity(world, (EntityType)entity_type, x, y, z);
}

void runtime_world_abi_tick(World *world, float dt)
{
    if (!world)
        return;

    world_tick(world, dt);
}
