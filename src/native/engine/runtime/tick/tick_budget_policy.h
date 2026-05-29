#ifndef BANANA_ENGINE_RUNTIME_TICK_BUDGET_POLICY_H
#define BANANA_ENGINE_RUNTIME_TICK_BUDGET_POLICY_H

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_tick_budget_policy_terrain_chunks_per_tick(void);
    float runtime_tick_budget_policy_controller_war_radius(void);
    int runtime_tick_budget_policy_controller_war_reinforcements_per_tick(void);
    int runtime_tick_budget_policy_controller_war_population_cap(void);
    int runtime_tick_budget_policy_controller_war_terrain_pressure_pct(void);
    int runtime_tick_budget_policy_controller_war_terrain_expand_cooldown_ticks(void);
    int runtime_tick_budget_policy_controller_war_max_frontier_chunks(void);
    int runtime_tick_budget_policy_controller_war_skirmish_pct(void);
    int runtime_tick_budget_policy_controller_war_siege_pct(void);
    int runtime_tick_budget_policy_controller_war_skirmish_reinforcement_bonus(void);
    int runtime_tick_budget_policy_controller_war_siege_reinforcement_bonus(void);
    int runtime_tick_budget_policy_controller_war_skirmish_cooldown_reduction(void);
    int runtime_tick_budget_policy_controller_war_siege_cooldown_reduction(void);
    int runtime_tick_budget_policy_controller_war_intelligence_progress_skirmish(void);
    int runtime_tick_budget_policy_controller_war_intelligence_progress_siege(void);
    int runtime_tick_budget_policy_controller_war_intelligence_level_threshold_ticks(void);
    int runtime_tick_budget_policy_controller_war_intelligence_max_stage(void);
    int runtime_tick_budget_policy_controller_war_intelligence_reinforcement_bonus_per_stage(void);
    int runtime_tick_budget_policy_controller_war_intelligence_frontier_bonus_per_stage(void);
    int runtime_tick_budget_policy_controller_war_overcrowd_pct(void);
    int runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks(void);
    int runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage(void);
    int runtime_tick_budget_policy_controller_war_life_tick_interval(void);
    int runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max(void);
    int runtime_tick_budget_policy_controller_war_procgen_biome_variance(void);
    int runtime_tick_budget_policy_controller_war_sentience_gain_per_tick(void);
    int runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination(void);
    int runtime_tick_budget_policy_controller_war_negotiate_min_intelligence_stage(void);
    int runtime_tick_budget_policy_controller_war_negotiate_min_reinforcements(void);

#ifdef __cplusplus
}
#endif

#endif
