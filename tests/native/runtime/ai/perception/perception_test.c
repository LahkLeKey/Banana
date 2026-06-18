#include "ai/perception.h"
#include "world/world.h"
#include "../../support/test_support.h"

static void test_perception_query_returns_nearby_entities(void **state)
{
    (void)state;
    World *world = world_create();
    PerceptionQuery query;
    PerceptionResult result;

    BANANA_TEST_ASSERT_TRUE(world != NULL, "world_create must allocate a world for perception tests");

    world_spawn_entity(world, ENTITY_TYPE_NPC, 0.0f, 0.0f, 0.0f);
    world_spawn_entity(world, ENTITY_TYPE_NPC, 5.0f, 0.0f, 0.0f);

    query.origin[0] = 0.0f;
    query.origin[1] = 0.0f;
    query.origin[2] = 0.0f;
    query.radius = 3.0f;
    query.exclude_id = 0;

    result = perception_query_nearby(&query);
    BANANA_TEST_ASSERT_INT_EQ(result.count, 1, "perception_query_nearby should ignore out-of-range entities");
    BANANA_TEST_ASSERT_INT_EQ(result.entity_ids[0], 1,
                              "perception_query_nearby should return nearby entity id");
    BANANA_TEST_ASSERT_FLOAT_EQ(result.distances[0], 0.0f, 0.0001f,
                                "perception_query_nearby should report measured distance");

    world_destroy(world);
}

static void test_perception_sense_and_line_of_sight(void **state)
{
    (void)state;
    World *world = world_create();

    BANANA_TEST_ASSERT_TRUE(world != NULL, "world_create must succeed for perception sense checks");

    world_spawn_entity(world, ENTITY_TYPE_NPC, 1.0f, 0.0f, 1.0f);

    BANANA_TEST_ASSERT_TRUE(perception_sense((const float[]){0.0f, 0.0f, 0.0f}, 2.5f, 0),
                            "perception_sense should detect nearby entity");
    BANANA_TEST_ASSERT_TRUE(perception_line_of_sight((const float[]){0.0f, 0.0f, 0.0f},
                                                     (const float[]){1.0f, 0.0f, 1.0f}),
                            "perception_line_of_sight should report visibility");

    world_destroy(world);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_perception_query_returns_nearby_entities),
    BANANA_TEST_CASE(test_perception_sense_and_line_of_sight)
)
