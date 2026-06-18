#include "world/world.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

static void test_world_spawn_and_lookup(void **state)
{
    (void)state;
    World *world = world_create();
    EntityId id = 0;
    Entity *entity = NULL;

    assert_non_null(world);

    id = world_spawn_entity(world, ENTITY_TYPE_DYNAMIC, 1.0f, 2.0f, 3.0f);
    assert_int_not_equal(id, 0);

    entity = world_get_entity(world, id);
    assert_non_null(entity);
    assert_int_equal(entity->type, ENTITY_TYPE_DYNAMIC);
    assert_float_equal(entity->position[0], 1.0f, 0.0001f);
    assert_float_equal(entity->position[1], 2.0f, 0.0001f);
    assert_float_equal(entity->position[2], 3.0f, 0.0001f);

    world_tick(world, 0.016f);
    world_despawn_entity(world, id);
    assert_null(world_get_entity(world, id));

    world_destroy(world);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_world_spawn_and_lookup),
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

static int test_world_spawn_and_lookup(void)
{
    World *world = world_create();
    EntityId id = 0;
    Entity *entity = NULL;

    if (!world)
        return fail_if(1, "world_create must allocate a usable world");

    id = world_spawn_entity(world, ENTITY_TYPE_DYNAMIC, 1.0f, 2.0f, 3.0f);
    if (fail_if(id == 0, "world_spawn_entity must return a non-zero entity id"))
    {
        world_destroy(world);
        return 1;
    }

    entity = world_get_entity(world, id);
    if (fail_if(!entity, "world_get_entity must return the spawned entity") ||
        fail_if(entity->type != ENTITY_TYPE_DYNAMIC, "spawned entity type must be preserved") ||
        fail_if(entity->position[0] != 1.0f || entity->position[1] != 2.0f ||
                entity->position[2] != 3.0f,
                "spawned entity position must be preserved"))
    {
        world_destroy(world);
        return 1;
    }

    world_tick(world, 0.016f);
    world_despawn_entity(world, id);
    if (fail_if(world_get_entity(world, id) != NULL,
                "world_despawn_entity must remove the entity from the world"))
    {
        world_destroy(world);
        return 1;
    }

    world_destroy(world);
    return 0;
}

int main(void)
{
    return test_world_spawn_and_lookup();
}
#endif
