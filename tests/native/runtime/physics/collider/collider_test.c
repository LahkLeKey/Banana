#include "physics/collider.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

static void test_box_and_sphere_collisions_are_detected(void **state)
{
    (void)state;
    PhysicsBody a = { .id = 1, .position = {0.0f, 0.0f, 0.0f}, .shape_type = SHAPE_BOX, .shape = {.box = {.half_extents = {1.0f, 1.0f, 1.0f}}} };
    PhysicsBody b = { .id = 2, .position = {0.6f, 0.0f, 0.0f}, .shape_type = SHAPE_BOX, .shape = {.box = {.half_extents = {1.0f, 1.0f, 1.0f}}} };
    Collision c = {0};

    assert_true(collider_box_vs_box(&a, &b, &c));
    assert_true(c.penetration > 0.0f);
    assert_int_equal((int)c.body_a, 1);
    assert_int_equal((int)c.body_b, 2);

    PhysicsBody sphere_a = { .id = 3, .position = {0.0f, 0.0f, 0.0f}, .shape_type = SHAPE_SPHERE, .shape = {.sphere = {.radius = 0.5f}} };
    PhysicsBody sphere_b = { .id = 4, .position = {0.2f, 0.0f, 0.0f}, .shape_type = SHAPE_SPHERE, .shape = {.sphere = {.radius = 0.5f}} };
    Collision sphere_collision = {0};

    assert_true(collider_sphere_vs_sphere(&sphere_a, &sphere_b, &sphere_collision));
    assert_true(sphere_collision.penetration > 0.0f);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_box_and_sphere_collisions_are_detected),
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

static int test_box_and_sphere_collisions_are_detected(void)
{
    PhysicsBody a = { .id = 1, .position = {0.0f, 0.0f, 0.0f}, .shape_type = SHAPE_BOX, .shape = {.box = {.half_extents = {1.0f, 1.0f, 1.0f}}} };
    PhysicsBody b = { .id = 2, .position = {0.6f, 0.0f, 0.0f}, .shape_type = SHAPE_BOX, .shape = {.box = {.half_extents = {1.0f, 1.0f, 1.0f}}} };
    Collision c = {0};

    if (fail_if(!collider_box_vs_box(&a, &b, &c), "box-vs-box collision should be detected"))
        return 1;

    if (fail_if(c.penetration <= 0.0f || c.body_a != 1 || c.body_b != 2,
                "collision metadata should reflect the overlapping pair"))
        return 1;

    PhysicsBody sphere_a = { .id = 3, .position = {0.0f, 0.0f, 0.0f}, .shape_type = SHAPE_SPHERE, .shape = {.sphere = {.radius = 0.5f}} };
    PhysicsBody sphere_b = { .id = 4, .position = {0.2f, 0.0f, 0.0f}, .shape_type = SHAPE_SPHERE, .shape = {.sphere = {.radius = 0.5f}} };
    Collision sphere_collision = {0};

    if (fail_if(!collider_sphere_vs_sphere(&sphere_a, &sphere_b, &sphere_collision),
                "sphere-vs-sphere collision should be detected"))
        return 1;

    return fail_if(sphere_collision.penetration <= 0.0f,
                   "sphere-vs-sphere collision should report positive penetration");
}

int main(void)
{
    return test_box_and_sphere_collisions_are_detected();
}
#endif
