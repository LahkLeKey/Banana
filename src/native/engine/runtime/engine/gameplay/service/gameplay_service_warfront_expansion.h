#ifndef BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_WARFRONT_EXPANSION_H
#define BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_WARFRONT_EXPANSION_H

#include "gameplay_service.h"
#include "../../engine_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

void runtime_gameplay_service_expand_warfront(World *world,
                                              EngineRuntimeState *runtime_state,
                                              ControllerInstance **controllers,
                                              int max_controllers,
                                              int *inout_controller_count,
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