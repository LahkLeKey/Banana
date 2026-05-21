#ifndef BANANA_ENGINE_RUNTIME_CONTROLLER_SYNC_H
#define BANANA_ENGINE_RUNTIME_CONTROLLER_SYNC_H

#include "../ai/controller.h"
#include "../world/world.h"


#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_sync_controller_positions(World *world,
                                           ControllerInstance **controllers,
                                           int controller_count,
                                           float dt);

    int runtime_controller_sync_parallel_available(void);

#ifdef __cplusplus
}
#endif

#endif