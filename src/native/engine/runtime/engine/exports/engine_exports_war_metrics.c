#include "../../../engine.h"

#include "engine_runtime_context.h"

#include "../../controller/runtime/controller_runtime.h"
#include "../../tick/tick_budget_policy.h"

static int runtime_engine_count_active_team_members(ControllerTeam team)
{
    int count = 0;

    if (!s_engine_state.world)
        return 0;

    for (int i = 0; i < s_engine_state.world->entity_count; i++)
    {
        Entity *entity = s_engine_state.world->entities[i];
        ControllerInstance *controller = NULL;

        if (!entity || !entity->active)
            continue;
        if (entity->type != ENTITY_TYPE_NPC)
            continue;
        if (entity->controller_id == 0)
            continue;

        controller = runtime_controller_find_by_id(s_engine_state.controllers,
                                                   s_engine_state.controller_count,
                                                   entity->controller_id);
        if (!controller)
            continue;
        if (controller->team == team)
            count += 1;
    }

    return count;
}

int engine_get_team_banana_count(void)
{
    return runtime_engine_count_active_team_members(CONTROLLER_TEAM_BANANA);
}

int engine_get_team_bean_count(void)
{
    return runtime_engine_count_active_team_members(CONTROLLER_TEAM_BEAN);
}

float engine_get_controller_war_radius(void)
{
    return runtime_tick_budget_policy_controller_war_radius();
}

int engine_get_controller_war_reinforcements_per_tick(void)
{
    return runtime_tick_budget_policy_controller_war_reinforcements_per_tick();
}

int engine_get_controller_war_population_cap(void)
{
    return runtime_tick_budget_policy_controller_war_population_cap();
}

int engine_get_controller_war_escalation_tier(void)
{
    return (int)s_engine_state.war_escalation_tier;
}

int engine_get_controller_war_effective_reinforcements_per_tick(void)
{
    if (!s_engine_state.engine_initialized)
        return runtime_tick_budget_policy_controller_war_reinforcements_per_tick();

    return s_engine_state.war_effective_reinforcements_per_tick;
}

int engine_get_controller_war_intelligence_stage(void)
{
    return s_engine_state.war_intelligence_stage;
}

int engine_get_controller_war_biome_unlock_count(void)
{
    int count = 0;

    for (int i = 0; i < BANANA_ENGINE_TERRAIN_MAX_LAYERS; i++)
    {
        if ((s_engine_state.war_biome_unlock_mask & (1u << i)) != 0u)
            count += 1;
    }

    return count;
}

int engine_get_controller_war_frontier_chunks(void)
{
    return s_engine_state.war_frontier_chunks;
}

int engine_get_controller_war_biome_stage_index(void)
{
    return s_engine_state.war_biome_stage_index;
}

int engine_get_controller_war_overcrowd_pct(void)
{
    return runtime_tick_budget_policy_controller_war_overcrowd_pct();
}

int engine_get_controller_war_overcrowd_expand_bonus_chunks(void)
{
    return runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks();
}

int engine_get_controller_war_overcrowd_intelligence_bonus_per_stage(void)
{
    return runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage();
}

int engine_get_controller_war_life_tick_interval(void)
{
    return runtime_tick_budget_policy_controller_war_life_tick_interval();
}

int engine_get_controller_war_life_intelligence_bonus_max(void)
{
    return runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max();
}

int engine_get_controller_war_procgen_biome_variance(void)
{
    return runtime_tick_budget_policy_controller_war_procgen_biome_variance();
}

int engine_get_controller_war_sentience_gain_per_tick(void)
{
    return runtime_tick_budget_policy_controller_war_sentience_gain_per_tick();
}

int engine_get_controller_war_sentience_comeback_bonus_per_coordination(void)
{
    return runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination();
}

int engine_get_controller_war_apex_feature_active(void)
{
    return s_engine_state.war_intelligence_stage >= 3 ? 1 : 0;
}

int engine_get_controller_war_mythic_feature_active(void)
{
    return s_engine_state.war_intelligence_stage >= 5 ? 1 : 0;
}

int engine_get_controller_war_life_generation(void)
{
    return s_engine_state.war_life_generation;
}

int engine_get_controller_war_life_alive_cells(void)
{
    return s_engine_state.war_life_alive_cells;
}

int engine_get_controller_war_life_frontline_cells(void)
{
    return s_engine_state.war_life_frontline_cells;
}

int engine_get_controller_war_procgen_biome_bias(void)
{
    return s_engine_state.war_procgen_biome_bias;
}

int engine_get_controller_war_sentience_humanoid_index(void)
{
    return s_engine_state.war_sentience_humanoid_index;
}

int engine_get_controller_war_sentience_coordination_level(void)
{
    return s_engine_state.war_sentience_coordination_level;
}

int engine_get_controller_war_sentience_empathy_level(void)
{
    return s_engine_state.war_sentience_empathy_level;
}

int engine_get_controller_war_sentience_behavior_mode_banana(void)
{
    return (int)s_engine_state.war_sentience_behavior_banana;
}

int engine_get_controller_war_sentience_behavior_mode_bean(void)
{
    return (int)s_engine_state.war_sentience_behavior_bean;
}

static int runtime_engine_war_sentience_mode_hits_at(const int *hits, int mode)
{
    if (!hits)
        return 0;
    if (mode < 0 || mode >= BANANA_ENGINE_WAR_SENTIENCE_MODE_COUNT)
        return 0;

    return hits[mode];
}

int engine_get_controller_war_sentience_spawn_mode_hits_banana(int mode)
{
    return runtime_engine_war_sentience_mode_hits_at(s_engine_state.war_sentience_spawn_mode_hits_banana,
                                                     mode);
}

int engine_get_controller_war_sentience_spawn_mode_hits_bean(int mode)
{
    return runtime_engine_war_sentience_mode_hits_at(s_engine_state.war_sentience_spawn_mode_hits_bean,
                                                     mode);
}

int engine_get_controller_war_sentience_negotiate_streak_ticks(void)
{
    return s_engine_state.war_sentience_negotiate_streak_ticks;
}

int engine_get_controller_war_sentience_negotiate_deescalation_trim_last_tick(void)
{
    return s_engine_state.war_sentience_negotiate_deescalation_trim_last_tick;
}

int engine_get_controller_war_sentience_comeback_bonus_last_tick(void)
{
    return s_engine_state.war_sentience_comeback_bonus_last_tick;
}

static int runtime_engine_sum_war_reinforcement_hits(const int *hits)
{
    int total = 0;

    if (!hits)
        return 0;

    for (int i = 0; i < BANANA_ENGINE_TERRAIN_MAX_LAYERS; i++)
        total += hits[i];

    return total;
}

static int runtime_engine_war_reinforcement_stage_hits_at(const int *hits, int stage_index)
{
    if (!hits)
        return 0;
    if (stage_index < 0 || stage_index >= BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS)
        return 0;

    return hits[stage_index];
}

int engine_get_controller_war_reinforcement_hits_total(void)
{
    return s_engine_state.war_reinforcement_spawns_total;
}

int engine_get_controller_war_reinforcement_hits_biome(int biome_index)
{
    if (biome_index < 0 || biome_index >= BANANA_ENGINE_TERRAIN_MAX_LAYERS)
        return 0;

    return s_engine_state.war_reinforcement_biome_hits[biome_index];
}

int engine_get_controller_war_reinforcement_hits_family_banana_scout(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_banana_scout_hits);
}

int engine_get_controller_war_reinforcement_hits_family_banana_siege(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_banana_siege_hits);
}

int engine_get_controller_war_reinforcement_hits_family_banana_apex(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_banana_apex_hits);
}

int engine_get_controller_war_reinforcement_hits_family_banana_mythic(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_banana_mythic_hits);
}

int engine_get_controller_war_reinforcement_hits_family_bean_raider(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_bean_raider_hits);
}

int engine_get_controller_war_reinforcement_hits_family_bean_warbrute(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_bean_warbrute_hits);
}

int engine_get_controller_war_reinforcement_hits_family_bean_apex(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_bean_apex_hits);
}

int engine_get_controller_war_reinforcement_hits_family_bean_mythic(void)
{
    return runtime_engine_sum_war_reinforcement_hits(s_engine_state.war_reinforcement_bean_mythic_hits);
}

int engine_get_controller_war_reinforcement_hits_stage_banana_apex(int stage_index)
{
    return runtime_engine_war_reinforcement_stage_hits_at(s_engine_state.war_reinforcement_banana_apex_stage_hits,
                                                          stage_index);
}

int engine_get_controller_war_reinforcement_hits_stage_banana_mythic(int stage_index)
{
    return runtime_engine_war_reinforcement_stage_hits_at(s_engine_state.war_reinforcement_banana_mythic_stage_hits,
                                                          stage_index);
}

int engine_get_controller_war_reinforcement_hits_stage_bean_apex(int stage_index)
{
    return runtime_engine_war_reinforcement_stage_hits_at(s_engine_state.war_reinforcement_bean_apex_stage_hits,
                                                          stage_index);
}

int engine_get_controller_war_reinforcement_hits_stage_bean_mythic(int stage_index)
{
    return runtime_engine_war_reinforcement_stage_hits_at(s_engine_state.war_reinforcement_bean_mythic_stage_hits,
                                                          stage_index);
}
