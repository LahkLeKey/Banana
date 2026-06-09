#ifndef BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_MODELS_H
#define BANANA_ENGINE_RUNTIME_ENGINE_GAMEPLAY_SERVICE_MODELS_H

#include "engine_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum RuntimeWarReinforcementFamily
{
    RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE = 0,
    RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT = 1,
    RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE = 2,
    RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER = 3,
    RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE = 4,
} RuntimeWarReinforcementFamily;

typedef enum RuntimeWarReinforcementVisualTier
{
    RUNTIME_WAR_REINFORCEMENT_VISUAL_TIER_BASE = 0,
    RUNTIME_WAR_REINFORCEMENT_VISUAL_TIER_APEX = 1,
    RUNTIME_WAR_REINFORCEMENT_VISUAL_TIER_MYTHIC = 2,
} RuntimeWarReinforcementVisualTier;

int runtime_gameplay_clamp_biome_index(int war_biome_stage_index);
int runtime_gameplay_clamp_sentience_mode_index(RuntimeWarSentienceBehaviorMode behavior_mode);

RuntimeWarReinforcementFamily runtime_gameplay_reinforcement_family_for_team(ControllerTeam team,
                                                                             int war_escalation_tier,
                                                                             int war_intelligence_stage);

RuntimeWarReinforcementVisualTier runtime_gameplay_reinforcement_visual_tier_for_family(
    RuntimeWarReinforcementFamily family,
    int war_intelligence_stage);

const char *runtime_gameplay_reinforcement_model_id_for_family(RuntimeWarReinforcementFamily family,
                                                               RuntimeWarSentienceBehaviorMode behavior_mode,
                                                               int war_intelligence_stage,
                                                               int biome_index);

#ifdef __cplusplus
}
#endif

#endif