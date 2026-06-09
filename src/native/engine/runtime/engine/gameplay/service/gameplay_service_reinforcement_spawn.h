#ifndef BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_REINFORCEMENT_SPAWN_H
#define BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_REINFORCEMENT_SPAWN_H

#include "gameplay_service.h"
#include "../../engine_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

int runtime_gameplay_spawn_war_reinforcement(World *world,
                                             EngineRuntimeState *runtime_state,
                                             ControllerInstance **controllers,
                                             int max_controllers,
                                             int *inout_controller_count,
                                             ControllerTeam team,
                                             float anchor_x,
                                             float anchor_y,
                                             float anchor_z,
                                             float direction_x,
                                             float direction_z,
                                             float lateral_sign,
                                             int ordinal,
                                             RuntimeWarSentienceBehaviorMode behavior_mode,
                                             int war_escalation_tier,
                                             int war_intelligence_stage,
                                             int war_biome_stage_index);

#ifdef __cplusplus
}
#endif

#endif