#include "runtime/player/registry/player_registry.h"
#include "runtime/support/test_support.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

static uint32_t g_attach_calls = 0;
static uint32_t g_last_attached_entity = 0;
static char g_last_attached_type[32];

static float fake_terrain_sample(float x, float z)
{
    return (x * 0.5f) + (z * 0.25f) + 10.0f;
}

static uint32_t fake_attach_controller(uint32_t entity_id, const char *type)
{
    g_attach_calls += 1;
    g_last_attached_entity = entity_id;
    if (type)
        strncpy(g_last_attached_type, type, sizeof(g_last_attached_type) - 1);
    return 9000u + entity_id;
}

static int player_registry_test_setup(void **state)
{
    (void)state;
    runtime_player_registry_reset();
    g_attach_calls = 0;
    g_last_attached_entity = 0;
    memset(g_last_attached_type, 0, sizeof(g_last_attached_type));
    return 0;
}

static int player_registry_test_teardown(void **state)
{
    (void)state;
    runtime_player_registry_reset();
    return 0;
}

static void test_player_registry_default_add_find_and_input(void **state)
{
    (void)state;
    NativePlayerBinding *binding = NULL;

    runtime_player_registry_add_default(11u, "player-1", "Alpha", "combat", 1);
    runtime_player_registry_add_default(12u, "player-2", "Beta", NULL, 0);

    BANANA_TEST_ASSERT_INT_EQ(runtime_player_registry_count(), 2,
                              "add_default must append bindings");

    binding = runtime_player_registry_find("player-1");
    BANANA_TEST_ASSERT_TRUE(binding != NULL, "find must locate existing bindings by guid");
    BANANA_TEST_ASSERT_INT_EQ((int)binding->entity_id, 11,
                              "find must return the stored entity id");
    BANANA_TEST_ASSERT_STR_EQ(binding->controller_kind, "combat",
                              "controller kind should remain normalized for explicit combat kinds");

    binding = runtime_player_registry_find("player-2");
    BANANA_TEST_ASSERT_TRUE(binding != NULL, "find must locate the second binding");
    BANANA_TEST_ASSERT_STR_EQ(binding->controller_kind, "",
                              "missing controller kinds should remain empty in default bindings");

    runtime_player_registry_apply_input("player-1", 3.5f, -2.0f);
    BANANA_TEST_ASSERT_FLOAT_EQ(runtime_player_registry_find("player-1")->pending_move_x, 1.0f, 0.0001f,
                                "apply_input must clamp x to the upper bound");
    BANANA_TEST_ASSERT_FLOAT_EQ(runtime_player_registry_find("player-1")->pending_move_z, -1.0f, 0.0001f,
                                "apply_input must clamp z to the lower bound");

    runtime_player_registry_apply_input("player-1", NAN, INFINITY);
    BANANA_TEST_ASSERT_FLOAT_EQ(runtime_player_registry_find("player-1")->pending_move_x, 0.0f, 0.0001f,
                                "apply_input must sanitize non-finite x values");
    BANANA_TEST_ASSERT_FLOAT_EQ(runtime_player_registry_find("player-1")->pending_move_z, 0.0f, 0.0001f,
                                "apply_input must sanitize non-finite z values");

    BANANA_TEST_ASSERT_TRUE(runtime_player_registry_find(NULL) == NULL,
                            "find must reject null guids");
    BANANA_TEST_ASSERT_TRUE(runtime_player_registry_get(-1) == NULL,
                            "get must reject negative indexes");
}

static void test_player_registry_staleness_and_resources(void **state)
{
    (void)state;
    NativePlayerBinding *binding = NULL;

    runtime_player_registry_add_default(21u, "player-3", "Gamma", "human", 1);
    binding = runtime_player_registry_find("player-3");
    BANANA_TEST_ASSERT_TRUE(binding != NULL, "binding must exist for staleness checks");

    runtime_player_registry_mark_seen("player-3", 1000);
    runtime_player_registry_update_staleness(1500, 1000);
    BANANA_TEST_ASSERT_INT_EQ(binding->is_stale, 0,
                              "binding should stay fresh when under the stale threshold");

    runtime_player_registry_update_staleness(2601, 1000);
    BANANA_TEST_ASSERT_INT_EQ(binding->is_stale, 1,
                              "binding should become stale when it exceeds the threshold");
    BANANA_TEST_ASSERT_INT_EQ(runtime_player_registry_count_active(), 0,
                              "stale active bindings should not count as active");

    runtime_player_registry_deactivate_stale();
    BANANA_TEST_ASSERT_INT_EQ(binding->active, 0,
                              "deactivate_stale must clear active state for stale bindings");

    runtime_player_registry_add_resource_key("player-3", RUNTIME_RESOURCE_WOOD, 5);
    runtime_player_registry_add_resource_key("player-3", RUNTIME_RESOURCE_ORE, 3);
    runtime_player_registry_add_resource_key("player-3", RUNTIME_RESOURCE_GOLD, 2);
    runtime_player_registry_add_resource_key("player-3", RUNTIME_RESOURCE_WOOD, -100);
    BANANA_TEST_ASSERT_INT_EQ(runtime_player_registry_get_resource_key("player-3", RUNTIME_RESOURCE_WOOD), 0,
                              "resource counts must not go below zero");
    BANANA_TEST_ASSERT_INT_EQ(runtime_player_registry_get_resource("player-3", "ore"), 3,
                              "string resource getter must expose ore count");
    BANANA_TEST_ASSERT_INT_EQ(runtime_player_registry_get_resource("player-3", "gold"), 2,
                              "string resource getter must expose gold count");
    BANANA_TEST_ASSERT_INT_EQ(runtime_player_registry_get_resource("player-3", "invalid"), 0,
                              "invalid resource names must return zero");
}

static void test_player_registry_upsert_and_transform(void **state)
{
    (void)state;
    World *world = world_create();
    EntityId primary_player_id = 0;
    NativePlayerBinding *binding = NULL;
    Entity *entity = NULL;

    BANANA_TEST_ASSERT_TRUE(world != NULL, "world_create must return a usable world for registry upsert");

    binding = runtime_player_registry_upsert(world,
                                             "player-4",
                                             "Delta",
                                             "combat",
                                             1,
                                             fake_terrain_sample,
                                             fake_attach_controller,
                                             &primary_player_id);
    BANANA_TEST_ASSERT_TRUE(binding != NULL, "upsert must create a binding for a new guid");
    BANANA_TEST_ASSERT_TRUE(primary_player_id != 0,
                            "upsert must initialize the primary player id when it is empty");
    BANANA_TEST_ASSERT_INT_EQ(runtime_player_registry_count(), 1,
                              "upsert must add a single binding");
    BANANA_TEST_ASSERT_INT_EQ(g_attach_calls, 1,
                              "combat controller kinds should attach a controller exactly once");
    BANANA_TEST_ASSERT_INT_EQ((int)g_last_attached_entity, (int)binding->entity_id,
                              "attach hook must receive the spawned entity id");
    BANANA_TEST_ASSERT_STR_EQ(g_last_attached_type, "combat",
                              "combat controller kinds should resolve to combat attach types");

    entity = world_get_entity(world, binding->entity_id);
    BANANA_TEST_ASSERT_TRUE(entity != NULL, "upsert must spawn a matching world entity");
    BANANA_TEST_ASSERT_INT_EQ(entity->type, ENTITY_TYPE_PLAYER,
                              "upserted entities must be player entities");
    BANANA_TEST_ASSERT_INT_EQ(entity->active, 1,
                              "upserted entities must mirror the active flag");

    runtime_player_registry_set_transform(world,
                                          "player-4",
                                          50.0f,
                                          NAN,
                                          -80.0f,
                                          0,
                                          20.0f,
                                          fake_terrain_sample);
    entity = world_get_entity(world, binding->entity_id);
    BANANA_TEST_ASSERT_TRUE(entity != NULL, "transform must keep the entity accessible");
    BANANA_TEST_ASSERT_FLOAT_EQ(entity->position[0], 20.0f, 0.0001f,
                                "set_transform must clamp x to the island span");
    BANANA_TEST_ASSERT_FLOAT_EQ(entity->position[2], -20.0f, 0.0001f,
                                "set_transform must clamp z to the island span");
    BANANA_TEST_ASSERT_FLOAT_EQ(entity->position[1], fake_terrain_sample(50.0f, -80.0f) + 0.55f, 0.0001f,
                                "set_transform must use terrain height when y is invalid");
    BANANA_TEST_ASSERT_INT_EQ(entity->active, 0,
                              "set_transform must mirror the requested active flag");

    world_destroy(world);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_player_registry_default_add_find_and_input, player_registry_test_setup, player_registry_test_teardown),
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_player_registry_staleness_and_resources, player_registry_test_setup, player_registry_test_teardown),
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_player_registry_upsert_and_transform, player_registry_test_setup, player_registry_test_teardown)
)