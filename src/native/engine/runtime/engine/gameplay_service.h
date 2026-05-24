#ifndef BANANA_ENGINE_RUNTIME_GAMEPLAY_SERVICE_H
#define BANANA_ENGINE_RUNTIME_GAMEPLAY_SERVICE_H

#include "../../ai/controller.h"
#include "../../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_gameplay_service_tick(World *world,
                                       ControllerInstance **controllers,
                                       int controller_count,
                                       EntityId player_id,
                                       int *inout_pbj_pickup_collected,
                                       float wildlife_signal_radius,
                                       float collect_radius);

#ifdef __cplusplus
}
#endif

#endif
