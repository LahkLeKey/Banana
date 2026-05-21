#ifndef BANANA_ENGINE_RUNTIME_WILDLIFE_GAMEPLAY_H
#define BANANA_ENGINE_RUNTIME_WILDLIFE_GAMEPLAY_H

#include "../ai/controller.h"
#include "../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_wildlife_signal_player_nearby(World *world,
                                              ControllerInstance **controllers,
                                              int controller_count,
                                              EntityId player_id,
                                              float trigger_radius);

#ifdef __cplusplus
}
#endif

#endif
