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
                                       int max_controllers,
                                       int *inout_controller_count,
                                       EntityId player_id,
                                       int *inout_pbj_pickup_collected,
                                       float wildlife_signal_radius,
                                       float collect_radius,
                                       float controller_war_radius,
                                       int controller_war_reinforcements_per_tick,
                                       int controller_war_population_cap);

#ifdef __cplusplus
}
#endif

#endif
