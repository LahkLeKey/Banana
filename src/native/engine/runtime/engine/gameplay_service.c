#include "gameplay_service.h"
#include "engine_state.h"

#include "../controller/attach/controller_attach.h"
#include "../tick/tick_budget_policy.h"
#include "../wildlife/wildlife_gameplay.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static const char *k_banana_skirmish_models[] = {
    "gameplay/war/banana-scout-tropical-v1",
    "gameplay/war/banana-scout-glacier-v1",
    "gameplay/war/banana-scout-urban-v1",
    "gameplay/war/banana-scout-volcanic-v1",
};

static const char *k_banana_skirmish_flank_models[] = {
    "gameplay/war/banana-scout-flank-tropical-v1",
    "gameplay/war/banana-scout-flank-glacier-v1",
    "gameplay/war/banana-scout-flank-urban-v1",
    "gameplay/war/banana-scout-flank-volcanic-v1",
};

static const char *k_banana_skirmish_regroup_models[] = {
    "gameplay/war/banana-scout-regroup-tropical-v1",
    "gameplay/war/banana-scout-regroup-glacier-v1",
    "gameplay/war/banana-scout-regroup-urban-v1",
    "gameplay/war/banana-scout-regroup-volcanic-v1",
};

static const char *k_banana_skirmish_negotiate_models[] = {
    "gameplay/war/banana-scout-envoy-tropical-v1",
    "gameplay/war/banana-scout-envoy-glacier-v1",
    "gameplay/war/banana-scout-envoy-urban-v1",
    "gameplay/war/banana-scout-envoy-volcanic-v1",
};

static const char *k_banana_siege_models[] = {
    "gameplay/war/banana-siege-commander-tropical-v1",
    "gameplay/war/banana-siege-commander-glacier-v1",
    "gameplay/war/banana-siege-commander-urban-v1",
    "gameplay/war/banana-siege-commander-volcanic-v1",
};

static const char *k_bean_skirmish_models[] = {
    "gameplay/war/bean-raider-tropical-v1",
    "gameplay/war/bean-raider-glacier-v1",
    "gameplay/war/bean-raider-urban-v1",
    "gameplay/war/bean-raider-volcanic-v1",
};

static const char *k_bean_skirmish_flank_models[] = {
    "gameplay/war/bean-raider-flank-tropical-v1",
    "gameplay/war/bean-raider-flank-glacier-v1",
    "gameplay/war/bean-raider-flank-urban-v1",
    "gameplay/war/bean-raider-flank-volcanic-v1",
};

static const char *k_bean_skirmish_regroup_models[] = {
    "gameplay/war/bean-raider-regroup-tropical-v1",
    "gameplay/war/bean-raider-regroup-glacier-v1",
    "gameplay/war/bean-raider-regroup-urban-v1",
    "gameplay/war/bean-raider-regroup-volcanic-v1",
};

static const char *k_bean_skirmish_negotiate_models[] = {
    "gameplay/war/bean-raider-envoy-tropical-v1",
    "gameplay/war/bean-raider-envoy-glacier-v1",
    "gameplay/war/bean-raider-envoy-urban-v1",
    "gameplay/war/bean-raider-envoy-volcanic-v1",
};

static const char *k_bean_siege_models[] = {
    "gameplay/war/bean-warbrute-tropical-v1",
    "gameplay/war/bean-warbrute-glacier-v1",
    "gameplay/war/bean-warbrute-urban-v1",
    "gameplay/war/bean-warbrute-volcanic-v1",
};

static const char *k_banana_apex_models[] = {
    "gameplay/war/banana-phalanx-tropical-v1",
    "gameplay/war/banana-phalanx-glacier-v1",
    "gameplay/war/banana-phalanx-urban-v1",
    "gameplay/war/banana-phalanx-volcanic-v1",
};

static const char *k_bean_apex_models[] = {
    "gameplay/war/bean-colossus-tropical-v1",
    "gameplay/war/bean-colossus-glacier-v1",
    "gameplay/war/bean-colossus-urban-v1",
    "gameplay/war/bean-colossus-volcanic-v1",
};

static const char *k_banana_mythic_models[] = {
    "gameplay/war/banana-archon-tropical-v1",
    "gameplay/war/banana-archon-glacier-v1",
    "gameplay/war/banana-archon-urban-v1",
    "gameplay/war/banana-archon-volcanic-v1",
};

static const char *k_bean_mythic_models[] = {
    "gameplay/war/bean-leviathan-tropical-v1",
    "gameplay/war/bean-leviathan-glacier-v1",
    "gameplay/war/bean-leviathan-urban-v1",
    "gameplay/war/bean-leviathan-volcanic-v1",
};

typedef enum RuntimeWarReinforcementFamily
{
    RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE = 0,
    RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT = 1,
    RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE = 2,
    RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER = 3,
    RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE = 4,
} RuntimeWarReinforcementFamily;

static int runtime_gameplay_clamp_biome_index(int war_biome_stage_index)
{
    int model_count = (int)(sizeof(k_banana_skirmish_models) / sizeof(k_banana_skirmish_models[0]));

    if (model_count <= 0)
        return 0;

    if (war_biome_stage_index < 0)
        war_biome_stage_index = -war_biome_stage_index;

    return war_biome_stage_index % model_count;
}

static int runtime_gameplay_clamp_sentience_mode_index(RuntimeWarSentienceBehaviorMode behavior_mode)
{
    int mode_index = (int)behavior_mode;

    if (mode_index < 0 || mode_index >= BANANA_ENGINE_WAR_SENTIENCE_MODE_COUNT)
        return (int)RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;

    return mode_index;
}

static int runtime_gameplay_clamp_war_intelligence_stage_bucket(int war_intelligence_stage)
{
    if (war_intelligence_stage < 0)
        return 0;
    if (war_intelligence_stage >= BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS)
        return BANANA_ENGINE_WAR_INTELLIGENCE_STAGE_BUCKETS - 1;

    return war_intelligence_stage;
}

static RuntimeWarReinforcementFamily runtime_gameplay_reinforcement_family_for_team(ControllerTeam team,
                                                                                     int war_escalation_tier,
                                                                                     int war_intelligence_stage)
{
    int effective_tier = war_escalation_tier;

    if (war_intelligence_stage >= 2 && effective_tier < 2)
        effective_tier += 1;

    if (team == CONTROLLER_TEAM_BANANA)
    {
        if (effective_tier >= 2)
            return RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE;
        if (effective_tier == 1)
            return RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT;
        return RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE;
    }

    if (team == CONTROLLER_TEAM_BEAN)
    {
        if (effective_tier >= 2)
            return RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE;
        if (effective_tier == 1)
            return RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER;
        return RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE;
    }

    return RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE;
}

static const char *runtime_gameplay_behavioral_skirmish_model_id_for_family(
    RuntimeWarReinforcementFamily family,
    RuntimeWarSentienceBehaviorMode behavior_mode,
    int biome_index)
{
    switch (family)
    {
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT:
            switch (behavior_mode)
            {
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK:
                    return k_banana_skirmish_flank_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP:
                    return k_banana_skirmish_regroup_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE:
                    return k_banana_skirmish_negotiate_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE:
                default:
                    return k_banana_skirmish_models[biome_index];
            }

        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER:
            switch (behavior_mode)
            {
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK:
                    return k_bean_skirmish_flank_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP:
                    return k_bean_skirmish_regroup_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE:
                    return k_bean_skirmish_negotiate_models[biome_index];
                case RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE:
                default:
                    return k_bean_skirmish_models[biome_index];
            }

        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE:
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE:
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE:
        default:
            return NULL;
    }
}

static const char *runtime_gameplay_reinforcement_model_id_for_family(RuntimeWarReinforcementFamily family,
                                                                       RuntimeWarSentienceBehaviorMode behavior_mode,
                                                                       int war_intelligence_stage,
                                                                       int biome_index)
{
    const char *behavioral_skirmish_model_id =
        runtime_gameplay_behavioral_skirmish_model_id_for_family(family,
                                                                 behavior_mode,
                                                                 biome_index);

    if (behavioral_skirmish_model_id)
        return behavioral_skirmish_model_id;

    if (war_intelligence_stage >= 5)
    {
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE)
            return k_banana_mythic_models[biome_index];
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE)
            return k_bean_mythic_models[biome_index];
    }

    if (war_intelligence_stage >= 3)
    {
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE)
            return k_banana_apex_models[biome_index];
        if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE)
            return k_bean_apex_models[biome_index];
    }

    switch (family)
    {
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SIEGE:
            return k_banana_siege_models[biome_index];
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_WARBRUTE:
            return k_bean_siege_models[biome_index];
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BANANA_SCOUT:
            return k_banana_skirmish_models[biome_index];
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_BEAN_RAIDER:
            return k_bean_skirmish_models[biome_index];
        case RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE:
        default:
            return "gameplay/reference/banana-basic-v1";
    }
}

static void runtime_gameplay_record_war_reinforcement_spawn(EngineRuntimeState *runtime_state,
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

static int runtime_gameplay_count_active_war_npcs(const World *world)
{
    int count = 0;

    if (!world)
        return 0;

    for (int i = 0; i < world->entity_count; i++)
    {
        const Entity *entity = world->entities[i];
        if (!entity || !entity->active)
            continue;
        if (entity->type != ENTITY_TYPE_NPC)
            continue;

        if (strcmp(entity->controller_kind, "combat") == 0 ||
            strcmp(entity->controller_kind, "wildlife") == 0)
        {
            count += 1;
        }
    }

    return count;
}

static int runtime_gameplay_count_team_controllers(ControllerInstance **controllers,
                                                   int controller_count,
                                                   ControllerTeam team)
{
    int count = 0;

    if (!controllers || controller_count <= 0)
        return 0;

    for (int i = 0; i < controller_count; i++)
    {
        const ControllerInstance *controller = controllers[i];
        if (!controller)
            continue;
        if (controller->team == team)
            count += 1;
    }

    return count;
}

static RuntimeWarSentienceBehaviorMode runtime_gameplay_resolve_sentience_behavior_mode(
    const EngineRuntimeState *runtime_state,
    int team_count,
    int opposing_team_count)
{
    int pressure_gap = opposing_team_count - team_count;
    int parity_gap = team_count - opposing_team_count;
    int humanoid_index = 0;
    int coordination = 0;
    int empathy = 0;

    if (parity_gap < 0)
        parity_gap = -parity_gap;

    if (runtime_state)
    {
        humanoid_index = runtime_state->war_sentience_humanoid_index;
        coordination = runtime_state->war_sentience_coordination_level;
        empathy = runtime_state->war_sentience_empathy_level;
    }

    if (humanoid_index >= 8 && empathy >= 6 && parity_gap <= 1)
        return RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE;

    if (pressure_gap >= 2)
        return RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP;

    if (coordination >= 6 && pressure_gap <= 0)
        return RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK;

    return RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
}

static int runtime_gameplay_sentience_behavior_comeback_bonus(RuntimeWarSentienceBehaviorMode mode)
{
    if (mode == RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP)
        return 1;

    return 0;
}

static const char *runtime_gameplay_sentience_behavior_mode_label(RuntimeWarSentienceBehaviorMode mode)
{
    switch (mode)
    {
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK:
            return "flank";
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP:
            return "regroup";
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE:
            return "negotiate";
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE:
        default:
            return "hold";
    }
}

static int runtime_gameplay_sentience_negotiate_deescalation_trim_for_streak(int streak_ticks)
{
    int trim = 0;

    if (streak_ticks <= 0)
        return 0;

    /* Consecutive negotiate ticks progressively trim spawn pressure. */
    trim = 1 + ((streak_ticks - 1) / 2);
    if (trim > 4)
        trim = 4;

    return trim;
}

static void runtime_gameplay_sentience_behavior_spawn_offsets(RuntimeWarSentienceBehaviorMode mode,
                                                              int ordinal,
                                                              float *out_forward,
                                                              float *out_lateral)
{
    float forward = 1.75f + (float)(ordinal % 3) * 0.45f;
    float lateral = 0.60f + (float)(ordinal % 2) * 0.25f;

    switch (mode)
    {
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK:
            /* Flank mode spreads wide with modest forward pressure. */
            forward += 0.25f;
            lateral += 0.75f;
            break;
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_REGROUP:
            /* Regroup mode forms rear echelons behind the active front anchor. */
            forward = -(1.10f + (float)(ordinal % 3) * 0.25f);
            lateral = 0.18f + (float)(ordinal % 2) * 0.14f;
            break;
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE:
            /* Negotiate mode forms near-center lanes to de-escalate spread. */
            forward = 0.95f + (float)(ordinal % 3) * 0.15f;
            lateral = 0.08f + (float)(ordinal % 2) * 0.06f;
            break;
        case RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE:
        default:
            break;
    }

    if (out_forward)
        *out_forward = forward;
    if (out_lateral)
        *out_lateral = lateral;
}

static void runtime_gameplay_sentience_behavior_apply_directionality(RuntimeWarSentienceBehaviorMode mode,
                                                                     float lateral_sign,
                                                                     float *inout_direction_x,
                                                                     float *inout_direction_z)
{
    float dir_x = 0.0f;
    float dir_z = 0.0f;

    if (!inout_direction_x || !inout_direction_z)
        return;

    dir_x = *inout_direction_x;
    dir_z = *inout_direction_z;

    if (mode == RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK)
    {
        /* Wrap around the opponent-facing normal with signed heading rotation. */
        const float angle_radians = 0.5585053606f; /* 32 degrees */
        float signed_angle = (lateral_sign >= 0.0f) ? angle_radians : -angle_radians;
        float cos_theta = cosf(signed_angle);
        float sin_theta = sinf(signed_angle);
        float rotated_x = (dir_x * cos_theta) - (dir_z * sin_theta);
        float rotated_z = (dir_x * sin_theta) + (dir_z * cos_theta);

        *inout_direction_x = rotated_x;
        *inout_direction_z = rotated_z;
    }
}

static int runtime_gameplay_humanoid_coordination_comeback_bonus(const EngineRuntimeState *runtime_state,
                                                                 int banana_team_count,
                                                                 int bean_team_count)
{
    int per_coordination = 0;
    int bonus = 0;

    if (!runtime_state)
        return 0;
    if (bean_team_count <= banana_team_count)
        return 0;

    per_coordination = runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination();
    if (per_coordination <= 0)
        return 0;

    bonus = runtime_state->war_sentience_coordination_level / per_coordination;
    bonus += runtime_state->war_sentience_empathy_level / ((per_coordination * 2) + 1);

    if (bonus < 0)
        bonus = 0;
    if (bonus > 4)
        bonus = 4;

    return bonus;
}

static int runtime_gameplay_find_active_warfront(ControllerInstance **controllers,
                                                 int controller_count,
                                                 float trigger_radius,
                                                 ControllerInstance **out_banana,
                                                 ControllerInstance **out_bean)
{
    int found = 0;
    float best_dist_sq = 0.0f;
    float radius_sq = 0.0f;

    if (!controllers || controller_count <= 1 || !out_banana || !out_bean || trigger_radius <= 0.0f)
        return 0;

    *out_banana = NULL;
    *out_bean = NULL;
    radius_sq = trigger_radius * trigger_radius;

    for (int i = 0; i < controller_count; i++)
    {
        ControllerInstance *first = controllers[i];
        if (!first)
            continue;

        for (int j = i + 1; j < controller_count; j++)
        {
            ControllerInstance *second = controllers[j];
            float dx = 0.0f;
            float dz = 0.0f;
            float dist_sq = 0.0f;

            if (!second)
                continue;
            if (!controller_teams_are_hostile(first->team, second->team))
                continue;

            dx = first->position[0] - second->position[0];
            dz = first->position[2] - second->position[2];
            dist_sq = (dx * dx) + (dz * dz);
            if (dist_sq > radius_sq)
                continue;

            if (!found || dist_sq < best_dist_sq)
            {
                found = 1;
                best_dist_sq = dist_sq;

                if (first->team == CONTROLLER_TEAM_BANANA)
                {
                    *out_banana = first;
                    *out_bean = second;
                }
                else
                {
                    *out_banana = second;
                    *out_bean = first;
                }
            }
        }
    }

    return found;
}

static int runtime_gameplay_spawn_war_reinforcement(World *world,
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
                                                    int war_biome_stage_index)
{
    int biome_index = runtime_gameplay_clamp_biome_index(war_biome_stage_index);
    RuntimeWarReinforcementFamily family =
        runtime_gameplay_reinforcement_family_for_team(team,
                                                       war_escalation_tier,
                                                       war_intelligence_stage);
    float forward = 0.0f;
    float lateral = 0.0f;
    float oriented_direction_x = direction_x;
    float oriented_direction_z = direction_z;
    float x = anchor_x + (direction_x * forward) + ((-direction_z) * lateral);
    float z = anchor_z + (direction_z * forward) + (direction_x * lateral);
    float y = anchor_y + 0.35f;
    const char *controller_type = (team == CONTROLLER_TEAM_BANANA) ? "combat" : "wildlife";
    const char *gameplay_model_id = NULL;
    EntityId entity_id = 0;
    Entity *entity = NULL;
    uint32_t controller_id = 0;

    if (!world || !controllers || !inout_controller_count || max_controllers <= 0)
        return 0;

    runtime_gameplay_sentience_behavior_spawn_offsets(behavior_mode,
                                                      ordinal,
                                                      &forward,
                                                      &lateral);
    runtime_gameplay_sentience_behavior_apply_directionality(behavior_mode,
                                                             lateral_sign,
                                                             &oriented_direction_x,
                                                             &oriented_direction_z);
    lateral *= lateral_sign;

    x = anchor_x + (oriented_direction_x * forward) + ((-oriented_direction_z) * lateral);
    z = anchor_z + (oriented_direction_z * forward) + (oriented_direction_x * lateral);

    if (family == RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE)
    {
        gameplay_model_id = (team == CONTROLLER_TEAM_BEAN)
                                ? "gameplay/reference/banana-bean-green-v1"
                                : "gameplay/reference/banana-basic-v1";
    }
    else
    {
        gameplay_model_id = runtime_gameplay_reinforcement_model_id_for_family(family,
                                                                                behavior_mode,
                                                                                war_intelligence_stage,
                                                                                biome_index);
    }

    if (*inout_controller_count >= max_controllers)
        return 0;

    entity_id = world_spawn_entity(world, ENTITY_TYPE_NPC, x, y, z);
    if (!entity_id)
        return 0;

    entity = world_get_entity(world, entity_id);
    if (!entity)
        return 0;

    strncpy(entity->controller_kind, controller_type, sizeof(entity->controller_kind) - 1);
    entity->controller_kind[sizeof(entity->controller_kind) - 1] = '\0';
    strncpy(entity->gameplay_model_id, gameplay_model_id, sizeof(entity->gameplay_model_id) - 1);
    entity->gameplay_model_id[sizeof(entity->gameplay_model_id) - 1] = '\0';

    controller_id = runtime_controller_attach_to_entity_with_team(world,
                                                                  controllers,
                                                                  max_controllers,
                                                                  inout_controller_count,
                                                                  entity_id,
                                                                  controller_type,
                                                                  team);
    if (!controller_id)
    {
        world_despawn_entity(world, entity_id);
        return 0;
    }

    entity->controller_id = controller_id;
    runtime_gameplay_record_war_reinforcement_spawn(runtime_state,
                                                    team,
                                                    family,
                                                    behavior_mode,
                                                    war_intelligence_stage,
                                                    biome_index);
    return 1;
}

static void runtime_gameplay_service_expand_warfront(World *world,
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
                                   int war_biome_stage_index)
{
    Entity *player = NULL;
    int controller_count = 0;

    if (!world || !player_id || !inout_pbj_pickup_collected)
        return;

    if (inout_controller_count)
        controller_count = *inout_controller_count;

    runtime_wildlife_signal_player_nearby(world,
                                          controllers,
                                          controller_count,
                                          player_id,
                                          wildlife_signal_radius);

    if (!(*inout_pbj_pickup_collected))
    {
        player = world_get_entity(world, player_id);
        if (!player || !player->active)
            return;

        for (int i = 0; i < world->entity_count; i++)
        {
            Entity *entity = world->entities[i];
            if (!entity || !entity->active)
                continue;
            if (entity->type != ENTITY_TYPE_STATIC)
                continue;
            if (strcmp(entity->controller_kind, "pbj_pickup") != 0)
                continue;

            {
                float dx = player->position[0] - entity->position[0];
                float dz = player->position[2] - entity->position[2];
                float distance = sqrtf(dx * dx + dz * dz);
                if (distance <= collect_radius)
                {
                    entity->active = 0;
                    *inout_pbj_pickup_collected = 1;
                    break;
                }
            }
        }
    }

    runtime_gameplay_service_expand_warfront(world,
                                             runtime_state,
                                             controllers,
                                             max_controllers,
                                             inout_controller_count,
                                             controller_war_radius,
                                             controller_war_reinforcements_per_tick,
                                             controller_war_population_cap,
                                             war_escalation_tier,
                                             war_intelligence_stage,
                                             war_biome_stage_index);
}
