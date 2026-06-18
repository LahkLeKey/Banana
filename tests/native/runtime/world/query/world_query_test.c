#include "world/world.h"
#include "../../support/test_support.h"

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

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_world_query_nearby_finds_entities)
)
