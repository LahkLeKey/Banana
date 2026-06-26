#include "world/world.h"
#include "runtime/support/test_support.h"

static void test_world_query_nearby_finds_entities(void **state)
{
    (void)state;
    World *world = world_create();
    EntityId first = 0;
    EntityId second = 0;
    EntityId ids[4] = {0};
    int count = 0;

    BANANA_TEST_ASSERT_TRUE(world != NULL, "world_create must allocate a usable world");

    first = world_spawn_entity(world, ENTITY_TYPE_DYNAMIC, 0.0f, 0.0f, 0.0f);
    second = world_spawn_entity(world, ENTITY_TYPE_NPC, 1.0f, 0.0f, 0.0f);
    world_spawn_entity(world, ENTITY_TYPE_STATIC, 5.0f, 0.0f, 0.0f);

    BANANA_TEST_ASSERT_TRUE(first != 0 && second != 0,
                            "world_spawn_entity must create entities for query test");

    count = world_query_nearby(world, (const float[]){0.0f, 0.0f, 0.0f}, 1.5f, ids, 4);
    BANANA_TEST_ASSERT_INT_EQ(count, 2,
                              "world_query_nearby must return only nearby entities");
    BANANA_TEST_ASSERT_INT_EQ(ids[0], first,
                              "world_query_nearby should return first nearby id");
    BANANA_TEST_ASSERT_INT_EQ(ids[1], second,
                              "world_query_nearby should return second nearby id");

    world_destroy(world);
}

static void test_world_query_nearby_rejects_invalid_inputs(void **state)
{
    (void)state;
    World *world = world_create();
    EntityId ids[2] = {0};
    int count = 0;

    BANANA_TEST_ASSERT_TRUE(world != NULL, "world_create must allocate a world for guard-path tests");

    count = world_query_nearby(NULL, (const float[]){0.0f, 0.0f, 0.0f}, 1.0f, ids, 2);
    BANANA_TEST_ASSERT_INT_EQ(count, 0,
                              "world_query_nearby must reject null world pointers");

    count = world_query_nearby(world, NULL, 1.0f, ids, 2);
    BANANA_TEST_ASSERT_INT_EQ(count, 0,
                              "world_query_nearby must reject null position pointers");

    count = world_query_nearby(world, (const float[]){0.0f, 0.0f, 0.0f}, 1.0f, NULL, 2);
    BANANA_TEST_ASSERT_INT_EQ(count, 0,
                              "world_query_nearby must reject null output buffers");

    count = world_query_nearby(world, (const float[]){0.0f, 0.0f, 0.0f}, 1.0f, ids, 0);
    BANANA_TEST_ASSERT_INT_EQ(count, 0,
                              "world_query_nearby must reject non-positive max-results values");

    count = world_query_nearby(world, (const float[]){0.0f, 0.0f, 0.0f}, -1.0f, ids, 2);
    BANANA_TEST_ASSERT_INT_EQ(count, 0,
                              "world_query_nearby must reject negative radius values");

    world_destroy(world);
}

static void test_world_query_nearby_handles_entity_count_clamps_and_inactive_entities(void **state)
{
    (void)state;
    World *world = world_create();
    EntityId first = 0;
    EntityId second = 0;
    Entity *first_entity = NULL;
    EntityId ids[4] = {0};
    int count = 0;

    BANANA_TEST_ASSERT_TRUE(world != NULL, "world_create must allocate a world for clamp tests");

    world->entity_count = -5;
    count = world_query_nearby(world, (const float[]){0.0f, 0.0f, 0.0f}, 2.0f, ids, 4);
    BANANA_TEST_ASSERT_INT_EQ(count, 0,
                              "world_query_nearby must clamp negative entity_count to zero");

    world->entity_count = 0;

    first = world_spawn_entity(world, ENTITY_TYPE_DYNAMIC, 0.5f, 0.0f, 0.0f);
    second = world_spawn_entity(world, ENTITY_TYPE_NPC, 1.0f, 0.0f, 0.0f);
    BANANA_TEST_ASSERT_TRUE(first != 0 && second != 0,
                            "world_spawn_entity must create entities for clamp tests");

    first_entity = world_get_entity(world, first);
    BANANA_TEST_ASSERT_TRUE(first_entity != NULL,
                            "world_get_entity must return spawned entities for branch checks");
    first_entity->active = 0;

    world->entity_count = BANANA_MAX_ENTITIES + 8;
    count = world_query_nearby(world, (const float[]){0.0f, 0.0f, 0.0f}, 2.0f, ids, 4);
    BANANA_TEST_ASSERT_INT_EQ(count, 1,
                              "world_query_nearby must skip inactive entities and clamp oversized entity_count");
    BANANA_TEST_ASSERT_INT_EQ(ids[0], second,
                              "world_query_nearby must return the active nearby entity");

    world->entity_count = 2;
    world_destroy(world);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_world_query_nearby_finds_entities),
    BANANA_TEST_CASE(test_world_query_nearby_rejects_invalid_inputs),
    BANANA_TEST_CASE(test_world_query_nearby_handles_entity_count_clamps_and_inactive_entities)
)
