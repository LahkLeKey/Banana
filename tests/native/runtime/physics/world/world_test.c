#include "physics/world.h"
#include "../../support/test_support.h"

static void test_physics_world_lifecycle_and_raycast(void **state)
{
    (void)state;
    PhysicsWorld *world = physics_world_create();
    BANANA_TEST_ASSERT_TRUE(world != NULL, "physics_world_create must allocate the world");

    PhysicsBody *body = physics_world_add_body(world, 1.0f, 0.0f, 0.0f, 0.0f, SHAPE_BOX);
    BANANA_TEST_ASSERT_TRUE(body != NULL, "physics_world_add_body must create a body");

    PhysicsBodyId hit_id = 0;
    float hit_t = 0.0f;
    float direction[3] = {1.0f, 0.0f, 0.0f};

    BANANA_TEST_ASSERT_TRUE(physics_world_raycast(world, (const float[]){-2.0f, 0.0f, 0.0f}, direction, &hit_id, &hit_t),
                            "physics_world_raycast must detect hit on inserted box");
    BANANA_TEST_ASSERT_INT_EQ((int)hit_id, (int)body->id,
                              "physics_world_raycast must return touched body id");
    BANANA_TEST_ASSERT_TRUE(hit_t > 0.0f,
                            "physics_world_raycast must return positive hit distance");

    physics_world_step_fixed(world);
    physics_world_destroy(world);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_physics_world_lifecycle_and_raycast)
)
