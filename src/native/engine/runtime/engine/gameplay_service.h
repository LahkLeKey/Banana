#ifndef BANANA_ENGINE_RUNTIME_GAMEPLAY_SERVICE_H
#define BANANA_ENGINE_RUNTIME_GAMEPLAY_SERVICE_H

#include "../../ai/controller.h"
#include "../../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct EngineRuntimeState;
    typedef struct EngineRuntimeState EngineRuntimeState;

    void runtime_gameplay_service_tick(World *world,
                                       EngineRuntimeState *runtime_state,
                                       ControllerInstance **controllers,
                                       int max_controllers,
                                       int *inout_controller_count,
                                       EntityId player_id,
                                       int *inout_pbj_pickup_collected,
                                       float wildlife_signal_radius,
                                       float collect_radius,
                                       float controller_war_radius,
                                       int controller_war_reinforcements_per_tick,
                                       int controller_war_population_cap,
                                       int war_escalation_tier,
                                       int war_intelligence_stage,
                                       int war_biome_stage_index);

#ifdef __cplusplus
}
#endif

#endif
