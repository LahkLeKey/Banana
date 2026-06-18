#include "physics/body.h"
#include "../../support/test_support.h"

static void test_physics_body_lifecycle_and_forces(void **state)
{
    (void)state;
    PhysicsBody *body = physics_body_create(1, 2.0f, 1.0f, 2.0f, 3.0f, SHAPE_SPHERE);
    BANANA_TEST_ASSERT_TRUE(body != NULL, "physics_body_create must allocate a body");

    BANANA_TEST_ASSERT_INT_EQ((int)body->shape_type, (int)SHAPE_SPHERE,
                              "physics_body_create must initialize shape type");
    BANANA_TEST_ASSERT_TRUE(body->shape.sphere.radius > 0.0f,
                            "physics_body_create must initialize usable sphere radius");

    physics_body_apply_force(body, 1.0f, 0.0f, 0.0f);
    BANANA_TEST_ASSERT_FLOAT_EQ(body->force_accum[0], 1.0f, 0.0001f,
                                "physics_body_apply_force must accumulate force");

    physics_body_apply_impulse(body, 2.0f, 0.0f, 0.0f);
    BANANA_TEST_ASSERT_FLOAT_EQ(body->velocity[0], 1.0f, 0.0001f,
                                "physics_body_apply_impulse must update velocity using inverse mass");

    physics_body_set_static(body, 1);
    BANANA_TEST_ASSERT_TRUE(body->is_static, "physics_body_set_static must mark body immovable");
    BANANA_TEST_ASSERT_FLOAT_EQ(body->mass, 0.0f, 0.0001f,
                                "physics_body_set_static must zero body mass");

    physics_body_apply_force(body, 5.0f, 0.0f, 0.0f);
    BANANA_TEST_ASSERT_FLOAT_EQ(body->force_accum[0], 1.0f, 0.0001f,
                                "static bodies must ignore additional force application");

    physics_body_destroy(body);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_physics_body_lifecycle_and_forces)
)
