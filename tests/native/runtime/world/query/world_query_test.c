#include "world/world.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

static void test_world_query_nearby_finds_entities(void **state)
{
    (void)state;
    World *world = world_create();
    EntityId first = 0;
    EntityId second = 0;
    EntityId ids[4] = {0};
    int count = 0;

    assert_non_null(world);

    first = world_spawn_entity(world, ENTITY_TYPE_DYNAMIC, 0.0f, 0.0f, 0.0f);
    second = world_spawn_entity(world, ENTITY_TYPE_NPC, 1.0f, 0.0f, 0.0f);
    world_spawn_entity(world, ENTITY_TYPE_STATIC, 5.0f, 0.0f, 0.0f);

    assert_int_not_equal(first, 0);
    assert_int_not_equal(second, 0);

    count = world_query_nearby(world, (const float[]){0.0f, 0.0f, 0.0f}, 1.5f, ids, 4);
    assert_int_equal(count, 2);
    assert_int_equal(ids[0], first);
    assert_int_equal(ids[1], second);

    world_destroy(world);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_world_query_nearby_finds_entities),
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

static int test_world_query_nearby_finds_entities(void)
{
    World *world = world_create();
    EntityId first = 0;
    EntityId second = 0;
    EntityId ids[4] = {0};
    int count = 0;

    if (!world)
        return fail_if(1, "world_create must allocate a usable world");

    first = world_spawn_entity(world, ENTITY_TYPE_DYNAMIC, 0.0f, 0.0f, 0.0f);
    second = world_spawn_entity(world, ENTITY_TYPE_NPC, 1.0f, 0.0f, 0.0f);
    world_spawn_entity(world, ENTITY_TYPE_STATIC, 5.0f, 0.0f, 0.0f);

    if (fail_if(first == 0 || second == 0, "world_spawn_entity must create entities for the query test"))
    {
        world_destroy(world);
        return 1;
    }

    count = world_query_nearby(world, (const float[]){0.0f, 0.0f, 0.0f}, 1.5f, ids, 4);
    if (fail_if(count != 2, "world_query_nearby must return only the nearby entities") ||
        fail_if(ids[0] != first || ids[1] != second,
                "world_query_nearby must return the expected nearby entity ids"))
    {
        world_destroy(world);
        return 1;
    }

    world_destroy(world);
    return 0;
}

int main(void)
{
    return test_world_query_nearby_finds_entities();
}
#endif
