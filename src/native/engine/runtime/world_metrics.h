#ifndef BANANA_ENGINE_RUNTIME_WORLD_METRICS_H
#define BANANA_ENGINE_RUNTIME_WORLD_METRICS_H

#include "../ai/controller.h"
#include "../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_world_active_player_count(const World *world);

    int runtime_world_entity_state(const World *world,
                                   int entity_index,
                                   ControllerInstance **controllers,
                                   int controller_count);

#ifdef __cplusplus
}
#endif

#endif
