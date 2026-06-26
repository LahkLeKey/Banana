#include "runtime/engine/lifecycle/engine_lifecycle.h"
#include "world/world.h"

#include "runtime/support/test_support.h"

#include <string.h>

static int g_attach_calls = 0;

static float flat_terrain_height(float x, float z)
{
    (void)x;
    (void)z;
    return 2.0f;
}

static uint32_t tracking_attach_controller(uint32_t entity_id, const char *type_name)
{
    (void)entity_id;
    (void)type_name;
    g_attach_calls += 1;
    return 77u;
}

static void test_actor_spawn_rejects_missing_inputs(void **state)
{
    (void)state;
    World *world = world_create();

    BANANA_TEST_ASSERT_TRUE(world != NULL, "world_create must succeed for actor spawn guard tests");

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_spawn_default_actors(NULL,
                                                                             flat_terrain_height,
                                                                             tracking_attach_controller),
                              0,
                              "actor spawn must reject null world inputs");

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_lifecycle_spawn_default_actors(world,
                                                                             NULL,
                                                                             tracking_attach_controller),
                              0,
                              "actor spawn must reject null terrain sampling callbacks");

    world_destroy(world);
}

static void test_actor_spawn_creates_expected_default_archetypes(void **state)
{
    (void)state;
    World *world = world_create();
    int spawned = 0;
    int npc_with_controller = 0;

    BANANA_TEST_ASSERT_TRUE(world != NULL, "world_create must succeed for actor spawn behavior tests");

    g_attach_calls = 0;
    spawned = runtime_engine_lifecycle_spawn_default_actors(world,
                                                            flat_terrain_height,
                                                            tracking_attach_controller);

    BANANA_TEST_ASSERT_INT_EQ(spawned,
                              9,
                              "actor spawn must create the expected archetype count");
    BANANA_TEST_ASSERT_INT_EQ(world->entity_count,
                              9,
                              "actor spawn should add all default actors to the world");
    BANANA_TEST_ASSERT_TRUE(g_attach_calls > 0,
                            "actor spawn should invoke controller attachment for AI archetypes");

    for (int i = 0; i < world->entity_count; i++)
    {
        Entity *entity = world->entities[i];
        if (entity && entity->type == ENTITY_TYPE_NPC && entity->controller_id != 0)
            npc_with_controller += 1;
    }

    BANANA_TEST_ASSERT_TRUE(npc_with_controller >= 1,
                            "actor spawn should assign at least one NPC controller id");

    world_destroy(world);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_actor_spawn_rejects_missing_inputs),
    BANANA_TEST_CASE(test_actor_spawn_creates_expected_default_archetypes)
)
