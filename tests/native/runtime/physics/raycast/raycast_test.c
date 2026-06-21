#include "physics/raycast.h"
#include "../../support/test_support.h"

static void test_physics_raycast_invalid_inputs_return_no_hit(void **state)
{
    (void)state;
    PhysicsBody *bodies[1] = {NULL};
    const float origin[3] = {0.0f, 0.0f, 0.0f};
    const float direction[3] = {1.0f, 0.0f, 0.0f};

    BANANA_TEST_ASSERT_INT_EQ(physics_raycast_bodies(NULL, 1, origin, direction, NULL, NULL), 0,
                              "physics_raycast_bodies must reject null body arrays");
    BANANA_TEST_ASSERT_INT_EQ(physics_raycast_bodies(bodies, 0, origin, direction, NULL, NULL),
                              0,
                              "physics_raycast_bodies must reject non-positive body counts");
}

static void test_physics_raycast_skips_non_box_and_hits_box_with_swap(void **state)
{
    (void)state;
    PhysicsBody *sphere = physics_body_create(11, 1.0f, 0.0f, 0.0f, 0.0f, SHAPE_SPHERE);
    PhysicsBody *box = physics_body_create(42, 1.0f, 0.0f, 0.0f, 0.0f, SHAPE_BOX);
    PhysicsBody *bodies[3] = {NULL, sphere, box};
    const float origin[3] = {2.0f, 0.0f, 0.0f};
    const float direction[3] = {-1.0f, 0.0f, 0.0f};
    PhysicsBodyId hit_id = 0;
    float hit_t = 0.0f;

    BANANA_TEST_ASSERT_INT_EQ(physics_raycast_bodies(bodies, 3, origin, direction, &hit_id, &hit_t),
                              1,
                              "physics_raycast_bodies must find the box hit");
    BANANA_TEST_ASSERT_INT_EQ((int)hit_id, 42,
                              "physics_raycast_bodies must report the hit body id");
    BANANA_TEST_ASSERT_FLOAT_EQ(hit_t, 1.5f, 0.0001f,
                                "physics_raycast_bodies must compute near intersection after axis swap");

    physics_body_destroy(sphere);
    physics_body_destroy(box);
}

static void test_physics_raycast_zero_axis_outside_slab_rejects_hit(void **state)
{
    (void)state;
    PhysicsBody *box = physics_body_create(9, 1.0f, 0.0f, 0.0f, 0.0f, SHAPE_BOX);
    PhysicsBody *bodies[1] = {box};
    const float origin[3] = {-2.0f, 2.0f, 0.0f};
    const float direction[3] = {1.0f, 0.0f, 0.0f};

    BANANA_TEST_ASSERT_INT_EQ(physics_raycast_bodies(bodies, 1, origin, direction, NULL, NULL),
                              0,
                              "physics_raycast_bodies must reject rays outside a zero-direction slab");

    physics_body_destroy(box);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_physics_raycast_invalid_inputs_return_no_hit),
    BANANA_TEST_CASE(test_physics_raycast_skips_non_box_and_hits_box_with_swap),
    BANANA_TEST_CASE(test_physics_raycast_zero_axis_outside_slab_rejects_hit)
)
