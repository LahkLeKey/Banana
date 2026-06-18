#include "ai/perception.h"
#include "world/world.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

static void test_perception_query_returns_nearby_entities(void **state)
{
    (void)state;
    World *world = world_create();
    PerceptionQuery query;
    PerceptionResult result;

    assert_non_null(world);

    world_spawn_entity(world, ENTITY_TYPE_NPC, 0.0f, 0.0f, 0.0f);
    world_spawn_entity(world, ENTITY_TYPE_NPC, 5.0f, 0.0f, 0.0f);

    query.origin[0] = 0.0f;
    query.origin[1] = 0.0f;
    query.origin[2] = 0.0f;
    query.radius = 3.0f;
    query.exclude_id = 0;

    result = perception_query_nearby(&query);
    assert_int_equal(result.count, 1);
    assert_int_equal(result.entity_ids[0], 1);
    assert_float_equal(result.distances[0], 0.0f, 0.0001f);

    world_destroy(world);
}

static void test_perception_sense_and_line_of_sight(void **state)
{
    (void)state;
    World *world = world_create();

    assert_non_null(world);

    world_spawn_entity(world, ENTITY_TYPE_NPC, 1.0f, 0.0f, 1.0f);

    assert_true(perception_sense((const float[]){0.0f, 0.0f, 0.0f}, 2.5f, 0));
    assert_true(perception_line_of_sight((const float[]){0.0f, 0.0f, 0.0f},
                                         (const float[]){1.0f, 0.0f, 1.0f}));

    world_destroy(world);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_perception_query_returns_nearby_entities),
        cmocka_unit_test(test_perception_sense_and_line_of_sight),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#else
#include <stdio.h>

static int fail_if(int condition, const char *message)
{
    if (condition)
    {
        fprintf(stderr, "%s\n", message);
        return 1;
    }
    return 0;
}

static int test_perception_query_returns_nearby_entities(void)
{
    World *world = world_create();
    PerceptionQuery query;
    PerceptionResult result;

    if (!world)
        return fail_if(1, "world_create must allocate a world for perception tests");

    world_spawn_entity(world, ENTITY_TYPE_NPC, 0.0f, 0.0f, 0.0f);
    world_spawn_entity(world, ENTITY_TYPE_NPC, 5.0f, 0.0f, 0.0f);

    query.origin[0] = 0.0f;
    query.origin[1] = 0.0f;
    query.origin[2] = 0.0f;
    query.radius = 3.0f;
    query.exclude_id = 0;

    result = perception_query_nearby(&query);
    if (fail_if(result.count != 1, "perception_query_nearby should ignore out-of-range entities") ||
        fail_if(result.entity_ids[0] != 1, "perception_query_nearby should return the nearby entity id") ||
        fail_if(result.distances[0] != 0.0f, "perception_query_nearby should report the measured distance"))
    {
        world_destroy(world);
        return 1;
    }

    world_destroy(world);
    return 0;
}

static int test_perception_sense_and_line_of_sight(void)
{
    World *world = world_create();

    if (!world)
        return fail_if(1, "world_create must succeed for perception sense checks");

    world_spawn_entity(world, ENTITY_TYPE_NPC, 1.0f, 0.0f, 1.0f);

    if (fail_if(!perception_sense((const float[]){0.0f, 0.0f, 0.0f}, 2.5f, 0),
                "perception_sense should detect a nearby entity") ||
        fail_if(!perception_line_of_sight((const float[]){0.0f, 0.0f, 0.0f},
                                          (const float[]){1.0f, 0.0f, 1.0f}),
                "perception_line_of_sight should report line-of-sight as available"))
    {
        world_destroy(world);
        return 1;
    }

    world_destroy(world);
    return 0;
}

int main(void)
{
    if (test_perception_query_returns_nearby_entities() ||
        test_perception_sense_and_line_of_sight())
        return 1;

    return 0;
}
#endif
