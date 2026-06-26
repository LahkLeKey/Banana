#include "runtime/engine/gameplay/service/gameplay_service_reinforcement_spawn.h"
#include "runtime/engine/gameplay/service/gameplay_service_models.h"
#include "runtime/engine/gameplay/service/gameplay_service_reinforcement_metrics.h"
#include "runtime/controller/attach/controller_attach.h"
#include "runtime/support/test_support.h"

#include <string.h>

static World g_fake_world;
static Entity g_fake_entity;
static int g_fake_entity_active = 0;
static int g_spawn_should_fail = 0;
static int g_get_should_fail = 0;
static uint32_t g_attach_return_id = 17;
static int g_record_call_count = 0;
static ControllerTeam g_recorded_team = CONTROLLER_TEAM_NEUTRAL;
static RuntimeWarReinforcementFamily g_recorded_family = RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE;
static RuntimeWarSentienceBehaviorMode g_recorded_behavior = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
static int g_recorded_intelligence_stage = -1;
static int g_recorded_biome_index = -1;
static int g_despawn_call_count = 0;

static void reset_reinforcement_state(void)
{
    memset(&g_fake_world, 0, sizeof(g_fake_world));
    memset(&g_fake_entity, 0, sizeof(g_fake_entity));
    g_fake_entity_active = 0;
    g_spawn_should_fail = 0;
    g_get_should_fail = 0;
    g_attach_return_id = 17;
    g_record_call_count = 0;
    g_recorded_team = CONTROLLER_TEAM_NEUTRAL;
    g_recorded_family = RUNTIME_WAR_REINFORCEMENT_FAMILY_NONE;
    g_recorded_behavior = RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE;
    g_recorded_intelligence_stage = -1;
    g_recorded_biome_index = -1;
    g_despawn_call_count = 0;
}

EntityId world_spawn_entity(World *world, EntityType type, float x, float y, float z)
{
    (void)world;
    if (g_spawn_should_fail)
        return 0;

    memset(&g_fake_entity, 0, sizeof(g_fake_entity));
    g_fake_entity.id = 1;
    g_fake_entity.type = type;
    g_fake_entity.position[0] = x;
    g_fake_entity.position[1] = y;
    g_fake_entity.position[2] = z;
    g_fake_entity_active = 1;
    return g_fake_entity.id;
}

Entity *world_get_entity(World *world, EntityId id)
{
    (void)world;
    if (g_get_should_fail || !g_fake_entity_active || id != g_fake_entity.id)
        return NULL;
    return &g_fake_entity;
}

void world_despawn_entity(World *world, EntityId id)
{
    (void)world;
    if (g_fake_entity_active && id == g_fake_entity.id)
    {
        g_fake_entity_active = 0;
        g_despawn_call_count += 1;
    }
}

uint32_t runtime_controller_attach_to_entity_with_team(World *world,
                                                       ControllerInstance **controllers,
                                                       int max_controllers,
                                                       int *inout_controller_count,
                                                       uint32_t entity_id,
                                                       const char *type,
                                                       ControllerTeam team)
{
    (void)world;
    (void)controllers;
    (void)max_controllers;
    (void)inout_controller_count;
    (void)entity_id;
    (void)type;
    (void)team;
    return g_attach_return_id;
}

void runtime_gameplay_record_war_reinforcement_spawn(EngineRuntimeState *runtime_state,
                                                    ControllerTeam team,
                                                    RuntimeWarReinforcementFamily family,
                                                    RuntimeWarSentienceBehaviorMode behavior_mode,
                                                    int war_intelligence_stage,
                                                    int biome_index)
{
    (void)runtime_state;
    g_record_call_count += 1;
    g_recorded_team = team;
    g_recorded_family = family;
    g_recorded_behavior = behavior_mode;
    g_recorded_intelligence_stage = war_intelligence_stage;
    g_recorded_biome_index = biome_index;
}

static void test_reinforcement_spawn_rejects_invalid_inputs_and_capacity(void **state)
{
    (void)state;
    ControllerInstance *controllers[1] = {NULL};
    int controller_count = 0;
    EngineRuntimeState runtime_state = {0};

    reset_reinforcement_state();
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_spawn_war_reinforcement(NULL,
                                                                        &runtime_state,
                                                                        controllers,
                                                                        1,
                                                                        &controller_count,
                                                                        CONTROLLER_TEAM_BANANA,
                                                                        0.0f,
                                                                        0.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0,
                                                                        RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                                        0,
                                                                        0,
                                                                        0),
                              0,
                              "spawn must reject null world");

    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_spawn_war_reinforcement(&g_fake_world,
                                                                        &runtime_state,
                                                                        NULL,
                                                                        1,
                                                                        &controller_count,
                                                                        CONTROLLER_TEAM_BANANA,
                                                                        0.0f,
                                                                        0.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0,
                                                                        RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                                        0,
                                                                        0,
                                                                        0),
                              0,
                              "spawn must reject null controller arrays");

    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_spawn_war_reinforcement(&g_fake_world,
                                                                        &runtime_state,
                                                                        controllers,
                                                                        1,
                                                                        NULL,
                                                                        CONTROLLER_TEAM_BANANA,
                                                                        0.0f,
                                                                        0.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0,
                                                                        RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                                        0,
                                                                        0,
                                                                        0),
                              0,
                              "spawn must reject null controller count pointers");

    controller_count = 1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_spawn_war_reinforcement(&g_fake_world,
                                                                        &runtime_state,
                                                                        controllers,
                                                                        1,
                                                                        &controller_count,
                                                                        CONTROLLER_TEAM_BANANA,
                                                                        0.0f,
                                                                        0.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0,
                                                                        RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                                        0,
                                                                        0,
                                                                        0),
                              0,
                              "spawn must stop when controller capacity is full");
}

static void test_reinforcement_spawn_uses_fallback_models_for_none_family(void **state)
{
    (void)state;
    ControllerInstance *controllers[4] = {NULL};
    int controller_count = 0;
    EngineRuntimeState runtime_state = {0};

    reset_reinforcement_state();
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_spawn_war_reinforcement(&g_fake_world,
                                                                        &runtime_state,
                                                                        controllers,
                                                                        4,
                                                                        &controller_count,
                                                                        CONTROLLER_TEAM_NEUTRAL,
                                                                        10.0f,
                                                                        2.0f,
                                                                        30.0f,
                                                                        1.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0,
                                                                        RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                                        0,
                                                                        0,
                                                                        0),
                              1,
                              "spawn must succeed for neutral team fallback");
    BANANA_TEST_ASSERT_TRUE(g_fake_entity_active,
                            "spawn must leave an entity active on success");
    BANANA_TEST_ASSERT_TRUE(strcmp(g_fake_entity.controller_kind, "wildlife") == 0,
                            "neutral fallback must use wildlife controller type");
    BANANA_TEST_ASSERT_TRUE(strcmp(g_fake_entity.gameplay_model_id,
                                   "gameplay/reference/banana-basic-v1") == 0,
                            "neutral fallback must use banana-basic model id");
    BANANA_TEST_ASSERT_INT_EQ((int)g_fake_entity.controller_id, (int)g_attach_return_id,
                              "successful spawn must store attached controller id");
    BANANA_TEST_ASSERT_INT_EQ(g_record_call_count, 1,
                              "successful spawn must record reinforcement telemetry");
    BANANA_TEST_ASSERT_INT_EQ((int)g_recorded_team, (int)CONTROLLER_TEAM_NEUTRAL,
                              "telemetry must record the team");

    reset_reinforcement_state();
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_spawn_war_reinforcement(&g_fake_world,
                                                                        &runtime_state,
                                                                        controllers,
                                                                        4,
                                                                        &controller_count,
                                                                        CONTROLLER_TEAM_BEAN,
                                                                        10.0f,
                                                                        2.0f,
                                                                        30.0f,
                                                                        1.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0,
                                                                        RUNTIME_WAR_SENTIENCE_BEHAVIOR_HOLD_LINE,
                                                                        0,
                                                                        0,
                                                                        0),
                              1,
                              "spawn must succeed for bean fallback");
    BANANA_TEST_ASSERT_TRUE(strcmp(g_fake_entity.gameplay_model_id,
                                   "gameplay/reference/banana-bean-green-v1") == 0,
                            "bean fallback must use bean-green model id");
}

static void test_reinforcement_spawn_handles_spawn_attach_failures(void **state)
{
    (void)state;
    ControllerInstance *controllers[4] = {NULL};
    int controller_count = 0;
    EngineRuntimeState runtime_state = {0};

    reset_reinforcement_state();
    g_spawn_should_fail = 1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_spawn_war_reinforcement(&g_fake_world,
                                                                        &runtime_state,
                                                                        controllers,
                                                                        4,
                                                                        &controller_count,
                                                                        CONTROLLER_TEAM_BANANA,
                                                                        10.0f,
                                                                        2.0f,
                                                                        30.0f,
                                                                        1.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0,
                                                                        RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK,
                                                                        1,
                                                                        2,
                                                                        3),
                              0,
                              "spawn must fail when entity creation fails");

    reset_reinforcement_state();
    g_get_should_fail = 1;
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_spawn_war_reinforcement(&g_fake_world,
                                                                        &runtime_state,
                                                                        controllers,
                                                                        4,
                                                                        &controller_count,
                                                                        CONTROLLER_TEAM_BANANA,
                                                                        10.0f,
                                                                        2.0f,
                                                                        30.0f,
                                                                        1.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0,
                                                                        RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK,
                                                                        1,
                                                                        2,
                                                                        3),
                              0,
                              "spawn must fail when entity lookup fails");

    reset_reinforcement_state();
    g_attach_return_id = 0;
    BANANA_TEST_ASSERT_INT_EQ(runtime_gameplay_spawn_war_reinforcement(&g_fake_world,
                                                                        &runtime_state,
                                                                        controllers,
                                                                        4,
                                                                        &controller_count,
                                                                        CONTROLLER_TEAM_BANANA,
                                                                        10.0f,
                                                                        2.0f,
                                                                        30.0f,
                                                                        1.0f,
                                                                        0.0f,
                                                                        1.0f,
                                                                        0,
                                                                        RUNTIME_WAR_SENTIENCE_BEHAVIOR_FLANK,
                                                                        1,
                                                                        2,
                                                                        3),
                              0,
                              "spawn must fail when controller attachment fails");
    BANANA_TEST_ASSERT_INT_EQ(g_despawn_call_count, 1,
                              "failed attach must despawn the temporary entity");
    BANANA_TEST_ASSERT_INT_EQ(g_record_call_count, 0,
                              "failed attach must not record reinforcement telemetry");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_reinforcement_spawn_rejects_invalid_inputs_and_capacity),
    BANANA_TEST_CASE(test_reinforcement_spawn_uses_fallback_models_for_none_family),
    BANANA_TEST_CASE(test_reinforcement_spawn_handles_spawn_attach_failures)
)
