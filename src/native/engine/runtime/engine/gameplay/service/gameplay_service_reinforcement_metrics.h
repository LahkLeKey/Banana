#ifndef BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_REINFORCEMENT_METRICS_H
#define BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_REINFORCEMENT_METRICS_H

#include "../../engine_state.h"
#include "gameplay_service_models.h"

#ifdef __cplusplus
extern "C"
{
#endif

void runtime_gameplay_record_war_reinforcement_spawn(EngineRuntimeState *runtime_state,
                                                    ControllerTeam team,
                                                    RuntimeWarReinforcementFamily family,
                                                    RuntimeWarSentienceBehaviorMode behavior_mode,
                                                    int war_intelligence_stage,
                                                    int biome_index);

#ifdef __cplusplus
}
#endif

#endif