#ifndef BANANA_ENGINE_RUNTIME_WORLD_TELEMETRY_H
#define BANANA_ENGINE_RUNTIME_WORLD_TELEMETRY_H

#include "../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_world_entity_count(const World *world);
    float runtime_world_entity_x(const World *world, int entity_index);
    float runtime_world_entity_z(const World *world, int entity_index);

#ifdef __cplusplus
}
#endif

#endif
