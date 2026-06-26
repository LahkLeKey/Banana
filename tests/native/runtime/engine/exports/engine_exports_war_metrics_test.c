#include "runtime/engine/exports/engine_runtime_context.h"
#include "engine.h"

#include "runtime/support/test_support.h"

#include <string.h>

EngineRuntimeState s_engine_state = {0};
const RuntimeApplicationServicePorts *s_service_ports = NULL;

float runtime_tick_budget_policy_controller_war_radius(void) { return 6.5f; }
int runtime_tick_budget_policy_controller_war_reinforcements_per_tick(void) { return 3; }
int runtime_tick_budget_policy_controller_war_population_cap(void) { return 12; }
int runtime_tick_budget_policy_controller_war_overcrowd_pct(void) { return 75; }
int runtime_tick_budget_policy_controller_war_overcrowd_expand_bonus_chunks(void) { return 2; }
int runtime_tick_budget_policy_controller_war_overcrowd_intelligence_bonus_per_stage(void) { return 1; }
int runtime_tick_budget_policy_controller_war_life_tick_interval(void) { return 4; }
int runtime_tick_budget_policy_controller_war_life_intelligence_bonus_max(void) { return 5; }
int runtime_tick_budget_policy_controller_war_procgen_biome_variance(void) { return 6; }
int runtime_tick_budget_policy_controller_war_sentience_gain_per_tick(void) { return 2; }
int runtime_tick_budget_policy_controller_war_sentience_comeback_bonus_per_coordination(void) { return 3; }

ControllerInstance *runtime_controller_find_by_id(ControllerInstance **controllers,
                                                  int controller_count,
                                                  uint32_t controller_id)
{
    for (int i = 0; i < controller_count; i++)
    {
        ControllerInstance *controller = controllers[i];
        if (controller && controller->id == controller_id)
            return controller;
    }
    return NULL;
}

static void reset_state(void)
{
    memset(&s_engine_state, 0, sizeof(s_engine_state));
}

static void test_team_counts_and_policy_getters(void **state)
{
    (void)state;
    World world = {0};
    Entity banana_npc = { .id = 1, .type = ENTITY_TYPE_NPC, .active = 1, .controller_id = 11 };
    Entity bean_npc = { .id = 2, .type = ENTITY_TYPE_NPC, .active = 1, .controller_id = 22 };
    Entity inactive_npc = { .id = 3, .type = ENTITY_TYPE_NPC, .active = 0, .controller_id = 33 };
    Entity player = { .id = 4, .type = ENTITY_TYPE_PLAYER, .active = 1, .controller_id = 44 };
    ControllerInstance banana_ctrl = { .id = 11, .team = CONTROLLER_TEAM_BANANA };
    ControllerInstance bean_ctrl = { .id = 22, .team = CONTROLLER_TEAM_BEAN };
    ControllerInstance neutral_ctrl = { .id = 33, .team = CONTROLLER_TEAM_NEUTRAL };
    ControllerInstance *controllers[3] = { &banana_ctrl, &bean_ctrl, &neutral_ctrl };

    reset_state();
    BANANA_TEST_ASSERT_INT_EQ(engine_get_team_banana_count(), 0,
                              "null world must report zero banana team members");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_team_bean_count(), 0,
                              "null world must report zero bean team members");

    world.entities[0] = &banana_npc;
    world.entities[1] = &bean_npc;
    world.entities[2] = &inactive_npc;
    world.entities[3] = &player;
    world.entity_count = 4;
    s_engine_state.world = &world;
    s_engine_state.controllers[0] = &banana_ctrl;
    s_engine_state.controllers[1] = &bean_ctrl;
    s_engine_state.controllers[2] = &neutral_ctrl;
    s_engine_state.controller_count = 3;

    BANANA_TEST_ASSERT_INT_EQ(engine_get_team_banana_count(), 1,
                              "one active banana NPC must be counted");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_team_bean_count(), 1,
                              "one active bean NPC must be counted");

    BANANA_TEST_ASSERT_FLOAT_EQ(engine_get_controller_war_radius(), 6.5f, 0.001f,
                                "war radius getter must forward budget policy value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcements_per_tick(), 3,
                              "reinforcements getter must forward budget policy value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_population_cap(), 12,
                              "population cap getter must forward budget policy value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_overcrowd_pct(), 75,
                              "overcrowd pct getter must forward budget policy value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_overcrowd_expand_bonus_chunks(), 2,
                              "overcrowd bonus getter must forward budget policy value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_overcrowd_intelligence_bonus_per_stage(), 1,
                              "overcrowd intelligence bonus getter must forward budget policy value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_life_tick_interval(), 4,
                              "war life tick interval getter must forward policy value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_life_intelligence_bonus_max(), 5,
                              "war life intelligence max getter must forward policy value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_procgen_biome_variance(), 6,
                              "procgen variance getter must forward policy value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_gain_per_tick(), 2,
                              "sentience gain getter must forward policy value");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_comeback_bonus_per_coordination(), 3,
                              "comeback bonus getter must forward policy value");
}

static void test_runtime_state_metric_getters(void **state)
{
    (void)state;
    reset_state();

    s_engine_state.war_escalation_tier = RUNTIME_WAR_ESCALATION_SIEGE;
    s_engine_state.war_effective_reinforcements_per_tick = 9;
    s_engine_state.war_intelligence_stage = 5;
    s_engine_state.war_biome_unlock_mask = 0b1011u;
    s_engine_state.war_frontier_chunks = 7;
    s_engine_state.war_biome_stage_index = 2;
    s_engine_state.war_life_generation = 11;
    s_engine_state.war_life_alive_cells = 8;
    s_engine_state.war_life_frontline_cells = 4;
    s_engine_state.war_procgen_biome_bias = 3;
    s_engine_state.war_sentience_humanoid_index = 22;
    s_engine_state.war_sentience_coordination_level = 6;
    s_engine_state.war_sentience_empathy_level = 5;
    s_engine_state.war_sentience_behavior_banana = RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK;
    s_engine_state.war_sentience_behavior_bean = RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE;
    s_engine_state.war_sentience_spawn_mode_hits_banana[1] = 4;
    s_engine_state.war_sentience_spawn_mode_hits_bean[3] = 7;
    s_engine_state.war_sentience_negotiate_streak_ticks = 12;
    s_engine_state.war_sentience_negotiate_deescalation_trim_last_tick = 2;
    s_engine_state.war_sentience_comeback_bonus_last_tick = 1;
    s_engine_state.war_reinforcement_spawns_total = 44;
    s_engine_state.war_reinforcement_biome_hits[2] = 5;
    s_engine_state.war_reinforcement_banana_scout_hits[0] = 1;
    s_engine_state.war_reinforcement_banana_scout_hits[1] = 2;
    s_engine_state.war_reinforcement_banana_siege_hits[0] = 3;
    s_engine_state.war_reinforcement_banana_apex_hits[0] = 4;
    s_engine_state.war_reinforcement_banana_mythic_hits[0] = 5;
    s_engine_state.war_reinforcement_bean_raider_hits[0] = 6;
    s_engine_state.war_reinforcement_bean_warbrute_hits[0] = 7;
    s_engine_state.war_reinforcement_bean_apex_hits[0] = 8;
    s_engine_state.war_reinforcement_bean_mythic_hits[0] = 9;
    s_engine_state.war_reinforcement_banana_apex_stage_hits[1] = 10;
    s_engine_state.war_reinforcement_banana_mythic_stage_hits[2] = 11;
    s_engine_state.war_reinforcement_bean_apex_stage_hits[3] = 12;
    s_engine_state.war_reinforcement_bean_mythic_stage_hits[4] = 13;

    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_escalation_tier(), (int)RUNTIME_WAR_ESCALATION_SIEGE,
                              "escalation tier getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_effective_reinforcements_per_tick(), 3,
                              "when engine is not initialized, effective reinforcements must fall back to policy value");
    s_engine_state.engine_initialized = 1;
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_effective_reinforcements_per_tick(), 9,
                              "initialized engine must expose effective reinforcements from state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_intelligence_stage(), 5,
                              "intelligence stage getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_biome_unlock_count(), 3,
                              "biome unlock count must count bits in unlock mask");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_frontier_chunks(), 7,
                              "frontier chunk getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_biome_stage_index(), 2,
                              "biome stage getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_apex_feature_active(), 1,
                              "apex feature must activate at stage >= 3");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_mythic_feature_active(), 1,
                              "mythic feature must activate at stage >= 5");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_life_generation(), 11,
                              "life generation getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_life_alive_cells(), 8,
                              "alive cells getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_life_frontline_cells(), 4,
                              "frontline cells getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_procgen_biome_bias(), 3,
                              "procgen bias getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_humanoid_index(), 22,
                              "humanoid index getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_coordination_level(), 6,
                              "coordination getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_empathy_level(), 5,
                              "empathy getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_behavior_mode_banana(), (int)RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK,
                              "banana sentience behavior getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_behavior_mode_bean(), (int)RUNTIME_WAR_SENTIENCE_BEHAVIOR_NEGOTIATE,
                              "bean sentience behavior getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_spawn_mode_hits_banana(1), 4,
                              "banana spawn mode hits getter must expose indexed hit counts");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_spawn_mode_hits_bean(3), 7,
                              "bean spawn mode hits getter must expose indexed hit counts");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_spawn_mode_hits_banana(-1), 0,
                              "negative spawn mode index must return zero");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_spawn_mode_hits_bean(99), 0,
                              "out-of-range spawn mode index must return zero");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_negotiate_streak_ticks(), 12,
                              "negotiate streak getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_negotiate_deescalation_trim_last_tick(), 2,
                              "deescalation trim getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_sentience_comeback_bonus_last_tick(), 1,
                              "comeback bonus getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_total(), 44,
                              "reinforcement total getter must expose runtime state");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_biome(2), 5,
                              "biome reinforcement getter must expose indexed hit counts");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_biome(-1), 0,
                              "negative biome index must return zero");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_biome(99), 0,
                              "out-of-range biome index must return zero");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_family_banana_scout(), 3,
                              "banana scout family getter must sum all stage hits");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_family_banana_siege(), 3,
                              "banana siege family getter must sum all stage hits");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_family_banana_apex(), 4,
                              "banana apex family getter must sum all stage hits");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_family_banana_mythic(), 5,
                              "banana mythic family getter must sum all stage hits");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_family_bean_raider(), 6,
                              "bean raider family getter must sum all stage hits");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_family_bean_warbrute(), 7,
                              "bean warbrute family getter must sum all stage hits");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_family_bean_apex(), 8,
                              "bean apex family getter must sum all stage hits");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_family_bean_mythic(), 9,
                              "bean mythic family getter must sum all stage hits");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_stage_banana_apex(1), 10,
                              "banana apex stage getter must expose indexed hit counts");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_stage_banana_mythic(2), 11,
                              "banana mythic stage getter must expose indexed hit counts");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_stage_bean_apex(3), 12,
                              "bean apex stage getter must expose indexed hit counts");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_stage_bean_mythic(4), 13,
                              "bean mythic stage getter must expose indexed hit counts");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_stage_banana_apex(-1), 0,
                              "negative stage index must return zero");
    BANANA_TEST_ASSERT_INT_EQ(engine_get_controller_war_reinforcement_hits_stage_bean_mythic(99), 0,
                              "out-of-range stage index must return zero");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_team_counts_and_policy_getters),
    BANANA_TEST_CASE(test_runtime_state_metric_getters)
)
