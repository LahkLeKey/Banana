#include "gameplay_service_reinforcement_metrics.h"

static int runtime_gameplay_clamp_war_intelligence_stage_bucket(int war_intelligence_stage)
{
    if (war_intelligence_stage < 0)
        return 0;
    if (war_intelligence_stage >= BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS)
        return BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS - 1;

    return war_intelligence_stage;
}

void runtime_gameplay_record_war_reinforcement_spawn(EngineRuntimeState *runtime_state,
                                                    ControllerTeam team,
                                                    RuntimeWarReinforcementFamily family,
                                                    RuntimeWarSentienceBehaviorMode behavior_mode,
                                                    int war_intelligence_stage,
                                                    int biome_index)
{
    int stage_bucket = 0;
    int mode_index = 0;

    if (!runtime_state)
        return;
    if (biome_index < 0 || biome_index >= BANANA_ENGINE_TERRAIN_MAX_LAYERS)
        return;

    stage_bucket = runtime_gameplay_clamp_war_intelligence_stage_bucket(war_intelligence_stage);
    mode_index = runtime_gameplay_clamp_sentience_mode_index(behavior_mode);

    runtime_state->war_reinforcement_spawns_total += 1;
    runtime_state->war_reinforcement_biome_hits[biome_index] += 1;

    if (team == CONTROLLER_TEAM_BANANA)
        runtime_state->war_sentience_spawn_mode_hits_banana[mode_index] += 1;
    else if (team == CONTROLLER_TEAM_BEAN)
        runtime_state->war_sentience_spawn_mode_hits_bean[mode_index] += 1;

    switch (family)
    {
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT:
            runtime_state->war_reinforcement_banana_scout_hits[biome_index] += 1;
            if (war_intelligence_stage >= 5)
            {
                runtime_state->war_reinforcement_banana_mythic_hits[biome_index] += 1;
                runtime_state->war_reinforcement_banana_mythic_stage_hits[stage_bucket] += 1;
            }
            else if (war_intelligence_stage >= 3)
            {
                runtime_state->war_reinforcement_banana_apex_hits[biome_index] += 1;
                runtime_state->war_reinforcement_banana_apex_stage_hits[stage_bucket] += 1;
            }
            break;
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE:
            runtime_state->war_reinforcement_banana_siege_hits[biome_index] += 1;
            if (war_intelligence_stage >= 5)
            {
                runtime_state->war_reinforcement_banana_mythic_hits[biome_index] += 1;
                runtime_state->war_reinforcement_banana_mythic_stage_hits[stage_bucket] += 1;
            }
            else if (war_intelligence_stage >= 3)
            {
                runtime_state->war_reinforcement_banana_apex_hits[biome_index] += 1;
                runtime_state->war_reinforcement_banana_apex_stage_hits[stage_bucket] += 1;
            }
            break;
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER:
            runtime_state->war_reinforcement_bean_raider_hits[biome_index] += 1;
            if (war_intelligence_stage >= 5)
            {
                runtime_state->war_reinforcement_bean_mythic_hits[biome_index] += 1;
                runtime_state->war_reinforcement_bean_mythic_stage_hits[stage_bucket] += 1;
            }
            else if (war_intelligence_stage >= 3)
            {
                runtime_state->war_reinforcement_bean_apex_hits[biome_index] += 1;
                runtime_state->war_reinforcement_bean_apex_stage_hits[stage_bucket] += 1;
            }
            break;
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE:
            runtime_state->war_reinforcement_bean_warbrute_hits[biome_index] += 1;
            if (war_intelligence_stage >= 5)
            {
                runtime_state->war_reinforcement_bean_mythic_hits[biome_index] += 1;
                runtime_state->war_reinforcement_bean_mythic_stage_hits[stage_bucket] += 1;
            }
            else if (war_intelligence_stage >= 3)
            {
                runtime_state->war_reinforcement_bean_apex_hits[biome_index] += 1;
                runtime_state->war_reinforcement_bean_apex_stage_hits[stage_bucket] += 1;
            }
            break;
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE:
        default:
            break;
    }
}