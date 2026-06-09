#ifndef BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_SENTIENCE_DECISION_H
#define BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_SENTIENCE_DECISION_H

#include "../../state/engine_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

RuntimeWarSentienceBehaviorMode runtime_gameplay_resolve_sentience_behavior_mode(
    const EngineRuntimeState *runtime_state,
    int team_count,
    int opposing_team_count);

int runtime_gameplay_humanoid_coordination_comeback_bonus(const EngineRuntimeState *runtime_state,
                                                         int banana_team_count,
                                                         int bean_team_count);

#ifdef __cplusplus
}
#endif

#endif