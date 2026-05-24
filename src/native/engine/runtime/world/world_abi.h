#ifndef BANANA_ENGINE_RUNTIME_WORLD_ABI_H
#define BANANA_ENGINE_RUNTIME_WORLD_ABI_H

#include "../../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    uint32_t runtime_world_abi_spawn(World *world, int entity_type, float x, float y, float z);

    void runtime_world_abi_tick(World *world, float dt);

#ifdef __cplusplus
}
#endif

#endif
