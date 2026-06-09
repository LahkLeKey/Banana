#include "gameplay_service_warfront_expansion.h"

#include "gameplay_service_reinforcement_spawn.h"
#include "gameplay_service_sentience_behavior.h"
#include "gameplay_service_sentience_decision.h"
#include "gameplay_service_sentience_overrides.h"
#include "gameplay_service_warfront.h"

#include "../tick/tick_budget_policy.h"

#include <math.h>
#include <stdio.h>

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
                                              int war_biome_stage_index)
{
    ControllerInstance *banana_front = NULL;
    ControllerInstance *bean_front = NULL;
    int active_war_npc_count = 0;
    int spawn_budget = 0;
    int banana_team_count = 0;
    int bean_team_count = 0;
    int banana_comeback_bonus = 0;
    int sentience_comeback_bonus = 0;
    int negotiate_deescalation_trim = 0;
    int negotiate_spawn_budget_before_trim = 0;
    int negotiate_streak_ticks = 0;
    int banana_spawn_target = 0;
    int bean_spawn_target = 0;
    int banana_spawned = 0;
    int bean_spawned = 0;
    RuntimeWarSentienceBehaviorMode banana_behavior_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
    RuntimeWarSentienceBehaviorMode bean_behavior_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
    float dx = 0.0f;
    float dz = 0.0f;
    float distance = 0.0f;
    float direction_x = 1.0f;
    float direction_z = 0.0f;

    if (!world || !controllers || !inout_controller_count)
        return;
    if (controller_war_reinforcements_per_tick <= 0 || controller_war_population_cap <= 0)
        return;

    if (runtime_state)
    {
        runtime_state->war_sentience_comeback_bonus_last_tick = 0;
        runtime_state->war_sentience_behavior_banana = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
        runtime_state->war_sentience_behavior_bean = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
        runtime_state->war_sentience_negotiate_deescalation_trim_last_tick = 0;
    }

    runtime_gameplay_apply_sentience_overrides(runtime_state);

    if (!runtime_gameplay_find_active_warfront(controllers,
                                               *inout_controller_count,
                                               controller_war_radius,
                                               &banana_front,
                                               &bean_front))
    {
        if (runtime_state)
            runtime_state->war_sentience_negotiate_streak_ticks = 0;
        return;
    }

    active_war_npc_count = runtime_gameplay_count_active_war_npcs(world);
    if (active_war_npc_count >= controller_war_population_cap)
    {
        if (runtime_state)
            runtime_state->war_sentience_negotiate_streak_ticks = 0;
        return;
    }

    spawn_budget = controller_war_population_cap - active_war_npc_count;
    if (spawn_budget > controller_war_reinforcements_per_tick)
        spawn_budget = controller_war_reinforcements_per_tick;

    banana_team_count = runtime_gameplay_count_team_controllers(controllers,
                                                                 *inout_controller_count,
                                                                 CONTROLLER_TEAM_BANANA);
    bean_team_count = runtime_gameplay_count_team_controllers(controllers,
                                                               *inout_controller_count,
                                                               CONTROLLER_TEAM_BEAN);

    banana_behavior_mode = runtime_gameplay_resolve_sentience_behavior_mode(runtime_state,
                                                                             banana_team_count,
                                                                             bean_team_count);
    bean_behavior_mode = runtime_gameplay_resolve_sentience_behavior_mode(runtime_state,
                                                                           bean_team_count,
                                                                           banana_team_count);
    banana_behavior_mode = runtime_gameplay_apply_sentience_mode_override(banana_behavior_mode, 1);
    bean_behavior_mode = runtime_gameplay_apply_sentience_mode_override(bean_behavior_mode, 0);
    if (runtime_state)
    {
        runtime_state->war_sentience_behavior_banana = banana_behavior_mode;
        runtime_state->war_sentience_behavior_bean = bean_behavior_mode;
    }

    if (bean_team_count > banana_team_count)
    {
        banana_comeback_bonus = 1 + ((bean_team_count - banana_team_count) / 2);
        banana_comeback_bonus += runtime_gameplay_sentience_behavior_comeback_bonus(banana_behavior_mode);
    }

    if (banana_behavior_mode == RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE &&
        bean_behavior_mode == RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE)
    {
        if (runtime_state)
        {
            runtime_state->war_sentience_negotiate_streak_ticks += 1;
            negotiate_streak_ticks = runtime_state->war_sentience_negotiate_streak_ticks;
        }
        else
        {
            negotiate_streak_ticks = 1;
        }

        negotiate_spawn_budget_before_trim = spawn_budget;
        negotiate_deescalation_trim =
            runtime_gameplay_sentience_negotiate_deescalation_trim_for_streak(negotiate_streak_ticks);
        if (negotiate_deescalation_trim > spawn_budget)
            negotiate_deescalation_trim = spawn_budget;

        spawn_budget -= negotiate_deescalation_trim;

        if (war_intelligence_stage >= runtime_tick_budget_policy_controller_war_negotiate_min_intelligence_stage())
        {
            int negotiate_min_reinforcements =
                runtime_tick_budget_policy_controller_war_negotiate_min_reinforcements();

            if (negotiate_min_reinforcements > negotiate_spawn_budget_before_trim)
                negotiate_min_reinforcements = negotiate_spawn_budget_before_trim;

            if (negotiate_min_reinforcements > 0 && spawn_budget < negotiate_min_reinforcements)
            {
                spawn_budget = negotiate_min_reinforcements;
                negotiate_deescalation_trim = negotiate_spawn_budget_before_trim - spawn_budget;
                if (negotiate_deescalation_trim < 0)
                    negotiate_deescalation_trim = 0;
            }
        }
    }
    else if (runtime_state)
    {
        runtime_state->war_sentience_negotiate_streak_ticks = 0;
    }

    if (runtime_state)
        runtime_state->war_sentience_negotiate_deescalation_trim_last_tick = negotiate_deescalation_trim;

    if (spawn_budget <= 0)
        return;

    sentience_comeback_bonus = runtime_gameplay_humanoid_coordination_comeback_bonus(runtime_state,
                                                                                      banana_team_count,
                                                                                      bean_team_count);
    banana_comeback_bonus += sentience_comeback_bonus;
    if (runtime_state)
        runtime_state->war_sentience_comeback_bonus_last_tick = sentience_comeback_bonus;

    banana_spawn_target = (spawn_budget + 1) / 2;
    banana_spawn_target += banana_comeback_bonus;
    if (banana_spawn_target > spawn_budget)
        banana_spawn_target = spawn_budget;
    if (banana_spawn_target < 0)
        banana_spawn_target = 0;

    bean_spawn_target = spawn_budget - banana_spawn_target;

    if (banana_comeback_bonus > 0)
    {
        fprintf(stdout,
                "[runtime] banana-comeback-bonus banana=%d bean=%d bonus=%d sentience=%d targets=%d:%d humanoid=%d coord=%d empathy=%d mode=%s/%s negotiate=%d:%d\n",
                banana_team_count,
                bean_team_count,
                banana_comeback_bonus,
                sentience_comeback_bonus,
                banana_spawn_target,
                bean_spawn_target,
                runtime_state ? runtime_state->war_sentience_humanoid_index : 0,
                runtime_state ? runtime_state->war_sentience_coordination_level : 0,
                runtime_state ? runtime_state->war_sentience_empathy_level : 0,
                runtime_gameplay_sentience_behavior_mode_label(banana_behavior_mode),
                runtime_gameplay_sentience_behavior_mode_label(bean_behavior_mode),
                runtime_state ? runtime_state->war_sentience_negotiate_streak_ticks : 0,
                negotiate_deescalation_trim);
    }

    dx = bean_front->position[0] - banana_front->position[0];
    dz = bean_front->position[2] - banana_front->position[2];
    distance = sqrtf((dx * dx) + (dz * dz));
    if (distance > 0.001f)
    {
        direction_x = dx / distance;
        direction_z = dz / distance;
    }

    for (int i = 0; i < spawn_budget; i++)
    {
        ControllerTeam team = CONTROLLER_TEAM_BANANA;
        ControllerInstance *anchor = (team == CONTROLLER_TEAM_BANANA) ? banana_front : bean_front;
        float team_direction_x = (team == CONTROLLER_TEAM_BANANA) ? direction_x : -direction_x;
        float team_direction_z = (team == CONTROLLER_TEAM_BANANA) ? direction_z : -direction_z;
        float lateral_sign = (((i / 2) % 2) == 0) ? 1.0f : -1.0f;
        RuntimeWarSentienceBehaviorMode team_behavior_mode = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;

        if (banana_spawned >= banana_spawn_target)
            team = CONTROLLER_TEAM_BEAN;
        else if (bean_spawned < bean_spawn_target && ((i % 2) != 0))
            team = CONTROLLER_TEAM_BEAN;

        anchor = (team == CONTROLLER_TEAM_BANANA) ? banana_front : bean_front;
        team_direction_x = (team == CONTROLLER_TEAM_BANANA) ? direction_x : -direction_x;
        team_direction_z = (team == CONTROLLER_TEAM_BANANA) ? direction_z : -direction_z;
        team_behavior_mode = (team == CONTROLLER_TEAM_BANANA)
                                 ? banana_behavior_mode
                                 : bean_behavior_mode;

        if (runtime_gameplay_spawn_war_reinforcement(world,
                                                     runtime_state,
                                                     controllers,
                                                     max_controllers,
                                                     inout_controller_count,
                                                     team,
                                                     anchor->position[0],
                                                     anchor->position[1],
                                                     anchor->position[2],
                                                     team_direction_x,
                                                     team_direction_z,
                                                     lateral_sign,
                                                     i,
                                                     team_behavior_mode,
                                                     war_escalation_tier,
                                                     war_intelligence_stage,
                                                     war_biome_stage_index))
        {
            if (team == CONTROLLER_TEAM_BANANA)
                banana_spawned += 1;
            else
                bean_spawned += 1;
        }
        else
        {
            break;
        }
    }
}