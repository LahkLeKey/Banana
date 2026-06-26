#include "physics/collider.h"
#include "runtime/support/test_support.h"

static void test_box_and_sphere_collisions_are_detected(void **state)
{
    (void)state;
    PhysicsBody a = { .id = 1, .position = {0.0f, 0.0f, 0.0f}, .shape_type = SHAPE_BOX, .shape = {.box = {.half_extents = {1.0f, 1.0f, 1.0f}}} };
    PhysicsBody b = { .id = 2, .position = {0.6f, 0.0f, 0.0f}, .shape_type = SHAPE_BOX, .shape = {.box = {.half_extents = {1.0f, 1.0f, 1.0f}}} };
    Collision c = {0};

    BANANA_TEST_ASSERT_TRUE(collider_box_vs_box(&a, &b, &c),
                            "box-vs-box collision should be detected");
    BANANA_TEST_ASSERT_TRUE(c.penetration > 0.0f,
                            "box-vs-box collision should report positive penetration");
    BANANA_TEST_ASSERT_INT_EQ((int)c.body_a, 1,
                              "collision metadata should include first body id");
    BANANA_TEST_ASSERT_INT_EQ((int)c.body_b, 2,
                              "collision metadata should include second body id");

    PhysicsBody sphere_a = { .id = 3, .position = {0.0f, 0.0f, 0.0f}, .shape_type = SHAPE_SPHERE, .shape = {.sphere = {.radius = 0.5f}} };
    PhysicsBody sphere_b = { .id = 4, .position = {0.2f, 0.0f, 0.0f}, .shape_type = SHAPE_SPHERE, .shape = {.sphere = {.radius = 0.5f}} };
    Collision sphere_collision = {0};

    BANANA_TEST_ASSERT_TRUE(collider_sphere_vs_sphere(&sphere_a, &sphere_b, &sphere_collision),
                            "sphere-vs-sphere collision should be detected");
    BANANA_TEST_ASSERT_TRUE(sphere_collision.penetration > 0.0f,
                            "sphere-vs-sphere should report positive penetration");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_box_and_sphere_collisions_are_detected)
)
