#include "physics/world.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

static void test_physics_world_lifecycle_and_raycast(void **state)
{
    (void)state;
    PhysicsWorld *world = physics_world_create();
    assert_non_null(world);

    PhysicsBody *body = physics_world_add_body(world, 1.0f, 0.0f, 0.0f, 0.0f, SHAPE_BOX);
    assert_non_null(body);

    PhysicsBodyId hit_id = 0;
    float hit_t = 0.0f;
    float direction[3] = {1.0f, 0.0f, 0.0f};

    assert_true(physics_world_raycast(world, (const float[]){-2.0f, 0.0f, 0.0f}, direction, &hit_id, &hit_t));
    assert_int_equal((int)hit_id, (int)body->id);
    assert_true(hit_t > 0.0f);

    physics_world_step_fixed(world);
    physics_world_destroy(world);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_physics_world_lifecycle_and_raycast),
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

static int test_physics_world_lifecycle_and_raycast(void)
{
    PhysicsWorld *world = physics_world_create();
    if (fail_if(!world, "physics_world_create must allocate the world"))
        return 1;

    PhysicsBody *body = physics_world_add_body(world, 1.0f, 0.0f, 0.0f, 0.0f, SHAPE_BOX);
    if (fail_if(!body, "physics_world_add_body must create a body"))
    {
        physics_world_destroy(world);
        return 1;
    }

    PhysicsBodyId hit_id = 0;
    float hit_t = 0.0f;
    float direction[3] = {1.0f, 0.0f, 0.0f};

    if (fail_if(!physics_world_raycast(world, (const float[]){-2.0f, 0.0f, 0.0f}, direction, &hit_id, &hit_t),
                "physics_world_raycast must detect a hit on an inserted box body"))
    {
        physics_world_destroy(world);
        return 1;
    }

    if (fail_if(hit_id != body->id || hit_t <= 0.0f,
                "physics_world_raycast must return the touched body and positive distance"))
    {
        physics_world_destroy(world);
        return 1;
    }

    physics_world_step_fixed(world);

    physics_world_destroy(world);
    return 0;
}

int main(void)
{
    return test_physics_world_lifecycle_and_raycast();
}
#endif
