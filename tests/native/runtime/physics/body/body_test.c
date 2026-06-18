#include "physics/body.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

static void test_physics_body_lifecycle_and_forces(void **state)
{
    (void)state;
    PhysicsBody *body = physics_body_create(1, 2.0f, 1.0f, 2.0f, 3.0f, SHAPE_SPHERE);
    assert_non_null(body);

    assert_int_equal((int)body->shape_type, (int)SHAPE_SPHERE);
    assert_true(body->shape.sphere.radius > 0.0f);

    physics_body_apply_force(body, 1.0f, 0.0f, 0.0f);
    assert_float_equal(body->force_accum[0], 1.0f, 0.0001f);

    physics_body_apply_impulse(body, 2.0f, 0.0f, 0.0f);
    assert_float_equal(body->velocity[0], 1.0f, 0.0001f);

    physics_body_set_static(body, 1);
    assert_true(body->is_static);
    assert_float_equal(body->mass, 0.0f, 0.0001f);

    physics_body_apply_force(body, 5.0f, 0.0f, 0.0f);
    assert_float_equal(body->force_accum[0], 1.0f, 0.0001f);

    physics_body_destroy(body);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_physics_body_lifecycle_and_forces),
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

static int test_physics_body_lifecycle_and_forces(void)
{
    PhysicsBody *body = physics_body_create(1, 2.0f, 1.0f, 2.0f, 3.0f, SHAPE_SPHERE);
    if (fail_if(!body, "physics_body_create must allocate a body"))
        return 1;

    if (fail_if(body->shape_type != SHAPE_SPHERE || body->shape.sphere.radius <= 0.0f,
                "physics_body_create must initialize a usable shape"))
    {
        physics_body_destroy(body);
        return 1;
    }

    physics_body_apply_force(body, 1.0f, 0.0f, 0.0f);
    if (fail_if(body->force_accum[0] != 1.0f, "physics_body_apply_force must accumulate force"))
    {
        physics_body_destroy(body);
        return 1;
    }

    physics_body_apply_impulse(body, 2.0f, 0.0f, 0.0f);
    if (fail_if(body->velocity[0] != 1.0f, "physics_body_apply_impulse must update velocity using inverse mass"))
    {
        physics_body_destroy(body);
        return 1;
    }

    physics_body_set_static(body, 1);
    if (fail_if(!body->is_static || body->mass != 0.0f,
                "physics_body_set_static must mark the body immovable"))
    {
        physics_body_destroy(body);
        return 1;
    }

    physics_body_apply_force(body, 5.0f, 0.0f, 0.0f);
    if (fail_if(body->force_accum[0] != 1.0f,
                "static bodies must ignore additional force application"))
    {
        physics_body_destroy(body);
        return 1;
    }

    physics_body_destroy(body);
    return 0;
}

int main(void)
{
    return test_physics_body_lifecycle_and_forces();
}
#endif
