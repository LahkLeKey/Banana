#include "runtime/engine/gameplay/service/gameplay_service.h"

#include "runtime/support/test_support.h"

#include <string.h>

static int g_wildlife_call_count = 0;
static int g_expand_call_count = 0;
static int g_last_controller_count = 0;
static float g_last_wildlife_radius = 0.0f;

static void reset_counters(void)
{
    g_wildlife_call_count = 0;
    g_expand_call_count = 0;
    g_last_controller_count = -1;
    g_last_wildlife_radius = 0.0f;
}

int runtime_wildlife_signal_player_nearby(World *world,
                                          ControllerInstance **controllers,
                                          int controller_count,
                                          EntityId player_id,
                                          float trigger_radius)
{
    (void)world;
    (void)controllers;
    (void)player_id;

    g_wildlife_call_count += 1;
    g_last_controller_count = controller_count;
    g_last_wildlife_radius = trigger_radius;
    return 0;
}

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
    (void)world;
    (void)runtime_state;
    (void)controllers;
    (void)max_controllers;
    (void)inout_controller_count;
    (void)controller_war_radius;
    (void)controller_war_reinforcements_per_tick;
    (void)controller_war_population_cap;
    (void)war_escalation_tier;
    (void)war_intelligence_stage;
    (void)war_biome_stage_index;

    g_expand_call_count += 1;
}

Entity *world_get_entity(World *world, EntityId id)
{
    int i;

    if (!world || !id)
        return NULL;

    for (i = 0; i < world->entity_count; ++i)
    {
        Entity *entity = world->entities[i];
        if (entity && entity->id == id)
            return entity;
    }

    return NULL;
}

static Entity make_entity(EntityId id,
                          EntityType type,
                          const char *controller_kind,
                          float x,
                          float z,
                          int active)
{
    Entity entity;

    memset(&entity, 0, sizeof(entity));
    entity.id = id;
    entity.type = type;
    entity.active = active;
    entity.position[0] = x;
    entity.position[2] = z;

    if (controller_kind)
        strncpy(entity.controller_kind, controller_kind, sizeof(entity.controller_kind) - 1);

    return entity;
}

static void test_tick_returns_early_for_missing_required_inputs(void **state)
{
    World world;
    int pbj_collected = 0;

    (void)state;

    memset(&world, 0, sizeof(world));
    reset_counters();

    runtime_gameplay_service_tick(NULL,
                                  NULL,
                                  NULL,
                                  0,
                                  NULL,
                                  1,
                                  &pbj_collected,
                                  2.0f,
                                  1.0f,
                                  3.0f,
                                  1,
                                  4,
                                  1,
                                  2,
                                  3);
    BANANA_TEST_ASSERT_INT_EQ(g_wildlife_call_count, 0, "null world must return before wildlife signaling");

    runtime_gameplay_service_tick(&world,
                                  NULL,
                                  NULL,
                                  0,
                                  NULL,
                                  0,
                                  &pbj_collected,
                                  2.0f,
                                  1.0f,
                                  3.0f,
                                  1,
                                  4,
                                  1,
                                  2,
                                  3);
    BANANA_TEST_ASSERT_INT_EQ(g_wildlife_call_count, 0, "missing player id must return before wildlife signaling");

    runtime_gameplay_service_tick(&world,
                                  NULL,
                                  NULL,
                                  0,
                                  NULL,
                                  1,
                                  NULL,
                                  2.0f,
                                  1.0f,
                                  3.0f,
                                  1,
                                  4,
                                  1,
                                  2,
                                  3);
    BANANA_TEST_ASSERT_INT_EQ(g_wildlife_call_count, 0, "null pbj output pointer must return before wildlife signaling");
}

static void test_tick_signals_wildlife_and_expands_when_pickup_already_collected(void **state)
{
    World world;
    int pbj_collected = 1;
    int controller_count = 7;

    (void)state;

    memset(&world, 0, sizeof(world));
    reset_counters();

    runtime_gameplay_service_tick(&world,
                                  NULL,
                                  NULL,
                                  16,
                                  &controller_count,
                                  11,
                                  &pbj_collected,
                                  5.0f,
                                  1.0f,
                                  3.0f,
                                  2,
                                  20,
                                  1,
                                  4,
                                  2);

    BANANA_TEST_ASSERT_INT_EQ(g_wildlife_call_count, 1, "wildlife signaling should run on valid inputs");
    BANANA_TEST_ASSERT_INT_EQ(g_last_controller_count, 7, "wildlife signaling should use provided controller count");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_wildlife_radius, 5.0f, 0.0001f, "wildlife signaling should use trigger radius");
    BANANA_TEST_ASSERT_INT_EQ(g_expand_call_count, 1, "warfront expansion should run when pickup scan is skipped");
}

static void test_tick_returns_before_expand_when_player_missing_or_inactive(void **state)
{
    World world;
    Entity inactive_player;
    int pbj_collected = 0;

    (void)state;

    memset(&world, 0, sizeof(world));
    reset_counters();

    runtime_gameplay_service_tick(&world,
                                  NULL,
                                  NULL,
                                  0,
                                  NULL,
                                  77,
                                  &pbj_collected,
                                  2.0f,
                                  1.0f,
                                  3.0f,
                                  1,
                                  4,
                                  1,
                                  2,
                                  3);

    BANANA_TEST_ASSERT_INT_EQ(g_wildlife_call_count, 1, "wildlife signaling should occur before player lookup");
    BANANA_TEST_ASSERT_INT_EQ(g_expand_call_count, 0, "missing player should prevent warfront expansion");

    inactive_player = make_entity(77, ENTITY_TYPE_PLAYER, "player", 0.0f, 0.0f, 0);
    world.entities[0] = &inactive_player;
    world.entity_count = 1;
    reset_counters();

    runtime_gameplay_service_tick(&world,
                                  NULL,
                                  NULL,
                                  0,
                                  NULL,
                                  77,
                                  &pbj_collected,
                                  2.0f,
                                  1.0f,
                                  3.0f,
                                  1,
                                  4,
                                  1,
                                  2,
                                  3);

    BANANA_TEST_ASSERT_INT_EQ(g_wildlife_call_count, 1, "wildlife signaling should still run with inactive player");
    BANANA_TEST_ASSERT_INT_EQ(g_expand_call_count, 0, "inactive player should prevent warfront expansion");
}

static void test_tick_collects_pbj_pickup_and_invokes_expand(void **state)
{
    World world;
    Entity player;
    Entity inactive_pickup;
    Entity wrong_type;
    Entity wrong_kind;
    Entity pickup;
    int pbj_collected = 0;

    (void)state;

    memset(&world, 0, sizeof(world));
    reset_counters();

    player = make_entity(5, ENTITY_TYPE_PLAYER, "player", 10.0f, 8.0f, 1);
    inactive_pickup = make_entity(6, ENTITY_TYPE_STATIC, "pbj_pickup", 10.1f, 8.1f, 0);
    wrong_type = make_entity(7, ENTITY_TYPE_DYNAMIC, "pbj_pickup", 10.1f, 8.1f, 1);
    wrong_kind = make_entity(8, ENTITY_TYPE_STATIC, "not_pickup", 10.1f, 8.1f, 1);
    pickup = make_entity(9, ENTITY_TYPE_STATIC, "pbj_pickup", 10.2f, 8.2f, 1);

    world.entities[0] = &player;
    world.entities[1] = &inactive_pickup;
    world.entities[2] = &wrong_type;
    world.entities[3] = &wrong_kind;
    world.entities[4] = &pickup;
    world.entity_count = 5;

    runtime_gameplay_service_tick(&world,
                                  NULL,
                                  NULL,
                                  0,
                                  NULL,
                                  player.id,
                                  &pbj_collected,
                                  4.0f,
                                  0.5f,
                                  2.0f,
                                  1,
                                  10,
                                  1,
                                  2,
                                  3);

    BANANA_TEST_ASSERT_INT_EQ(g_wildlife_call_count, 1, "wildlife signaling should run once");
    BANANA_TEST_ASSERT_INT_EQ(g_expand_call_count, 1, "warfront expansion should run after pickup scan");
    BANANA_TEST_ASSERT_INT_EQ(pbj_collected, 1, "nearby pbj pickup should mark collection flag");
    BANANA_TEST_ASSERT_INT_EQ(pickup.active, 0, "collected pickup should be deactivated");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_tick_returns_early_for_missing_required_inputs),
    BANANA_TEST_CASE(test_tick_signals_wildlife_and_expands_when_pickup_already_collected),
    BANANA_TEST_CASE(test_tick_returns_before_expand_when_player_missing_or_inactive),
    BANANA_TEST_CASE(test_tick_collects_pbj_pickup_and_invokes_expand)
)
