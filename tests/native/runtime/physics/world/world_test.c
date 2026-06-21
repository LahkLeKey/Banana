#include "physics/world.h"
#include "physics/world_collision.h"
#include "physics/dynamics.h"
#include "../../support/test_support.h"

#include <string.h>

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

static void test_dynamics_guard_paths_for_physics_world_module(void **state)
{
    PhysicsBody body;

    (void)state;
    memset(&body, 0, sizeof(body));

    body.is_static = 1;
    body.mass = 1.0f;
    body.force_accum[1] = 9.0f;

    dynamics_set_gravity(-8.0f);
    dynamics_integrate(&body, 0.016f);

    BANANA_TEST_ASSERT_FLOAT_EQ(body.force_accum[1],
                                9.0f,
                                0.0001f,
                                "dynamics integrate must early-return for static-body guard path");
}

static void test_world_collision_resolution_paths_for_physics_world_module(void **state)
{
    PhysicsWorld world;
    CollisionList cols;
    PhysicsBody invalid_zero;
    PhysicsBody invalid_high;
    PhysicsBody valid_a;
    PhysicsBody valid_b;

    (void)state;

    memset(&world, 0, sizeof(world));
    memset(&cols, 0, sizeof(cols));
    memset(&invalid_zero, 0, sizeof(invalid_zero));
    memset(&invalid_high, 0, sizeof(invalid_high));
    memset(&valid_a, 0, sizeof(valid_a));
    memset(&valid_b, 0, sizeof(valid_b));

    invalid_zero.id = 0;
    invalid_high.id = BANANA_MAX_PHYSICS_BODIES + 1;
    valid_a.id = 8;
    valid_b.id = 9;

    world.body_count = 5;
    world.bodies[0] = NULL;
    world.bodies[1] = &invalid_zero;
    world.bodies[2] = &invalid_high;
    world.bodies[3] = &valid_a;
    world.bodies[4] = &valid_b;

    cols.count = 4;
    cols.items[0].body_a = 0;
    cols.items[0].body_b = valid_b.id;
    cols.items[1].body_a = valid_a.id;
    cols.items[1].body_b = BANANA_MAX_PHYSICS_BODIES + 1;
    cols.items[2].body_a = valid_a.id;
    cols.items[2].body_b = 77;
    cols.items[3].body_a = valid_a.id;
    cols.items[3].body_b = valid_b.id;

    physics_world_resolve_collision_pairs(NULL, &cols);
    physics_world_resolve_collision_pairs(&world, NULL);
    physics_world_resolve_collision_pairs(&world, &cols);

    BANANA_TEST_ASSERT_TRUE(1,
                            "world collision resolve paths must execute without crashes for mixed pair lists");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_physics_world_lifecycle_and_raycast),
    BANANA_TEST_CASE(test_dynamics_guard_paths_for_physics_world_module),
    BANANA_TEST_CASE(test_world_collision_resolution_paths_for_physics_world_module)
)
