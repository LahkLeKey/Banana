#include "runtime/engine/lifecycle/engine_lifecycle.h"
#include "runtime/support/test_support.h"

static float lifecycle_flat_height(float x, float z)
{
    (void)x;
    (void)z;
    return 2.0f;
}

static void test_engine_lifecycle_build_terrain_invalid_input_fails(void **state)
{
    (void)state;
    RuntimeTerrainChunk chunks[1] = {{0}};

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_build_terrain(NULL,
                                                                     8,
                                                                     4,
                                                                     4,
                                                                     chunks,
                                                                     1,
                                                                     1),
                              0,
                              "build_terrain must fail when height grid is missing");
}

static void test_engine_lifecycle_bootstrap_primary_player_guards(void **state)
{
    (void)state;
    World *world = world_create();

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_bootstrap_primary_player(NULL,
                                                                                 1,
                                                                                 lifecycle_flat_height),
                              0,
                              "bootstrap_primary_player must reject null world");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_bootstrap_primary_player(world,
                                                                                 0,
                                                                                 lifecycle_flat_height),
                              0,
                              "bootstrap_primary_player must reject zero player id");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_bootstrap_primary_player(world,
                                                                                 12345,
                                                                                 lifecycle_flat_height),
                              0,
                              "bootstrap_primary_player must fail when player entity is missing");

    world_destroy(world);
}

static void test_engine_lifecycle_destroy_controllers_guard_and_clamp(void **state)
{
    (void)state;
    ControllerInstance *controllers[2] = {NULL, NULL};
    int controller_count = 5;

    runtime_engine_lifecycle_destroy_controllers(NULL, 2, &controller_count);
    BANANA_TEST_ASSERT_INT_EQ(controller_count, 5,
                              "destroy_controllers must leave count unchanged for null controller array");

    runtime_engine_lifecycle_destroy_controllers(controllers, 2, &controller_count);
    BANANA_TEST_ASSERT_INT_EQ(controller_count, 0,
                              "destroy_controllers must clamp and reset count");
    BANANA_TEST_ASSERT_TRUE(controllers[0] == NULL && controllers[1] == NULL,
                            "destroy_controllers must clear controller slots");
}

static void test_engine_lifecycle_reset_terrain_chunks_guards(void **state)
{
    (void)state;
    RuntimeTerrainChunk chunks[1] = {{0}};

    runtime_engine_lifecycle_reset_terrain_chunks(NULL, 1);
    runtime_engine_lifecycle_reset_terrain_chunks(chunks, 0);

    BANANA_TEST_ASSERT_INT_EQ((int)chunks[0].dirty, 0,
                              "reset_terrain_chunks guard paths must not mutate chunk state");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_engine_lifecycle_build_terrain_invalid_input_fails),
    BANANA_TEST_CASE(test_engine_lifecycle_bootstrap_primary_player_guards),
    BANANA_TEST_CASE(test_engine_lifecycle_destroy_controllers_guard_and_clamp),
    BANANA_TEST_CASE(test_engine_lifecycle_reset_terrain_chunks_guards)
)
