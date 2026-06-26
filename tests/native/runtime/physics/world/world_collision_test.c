#include "physics/world.h"
#include "physics/world_collision.h"
#include "runtime/support/test_support.h"

#include <string.h>

static int g_resolve_calls = 0;
static PhysicsBodyId g_last_a_id = 0;
static PhysicsBodyId g_last_b_id = 0;

void collider_resolve(const Collision *c, PhysicsBody *a, PhysicsBody *b)
{
    (void)c;
    g_resolve_calls += 1;
    g_last_a_id = a ? a->id : 0;
    g_last_b_id = b ? b->id : 0;
}

static void reset_probe_state(void)
{
    g_resolve_calls = 0;
    g_last_a_id = 0;
    g_last_b_id = 0;
}

static void test_world_collision_null_guards_do_not_resolve(void **state)
{
    CollisionList cols;
    PhysicsWorld world;

    (void)state;

    memset(&cols, 0, sizeof(cols));
    memset(&world, 0, sizeof(world));

    reset_probe_state();
    physics_world_resolve_collision_pairs(NULL, &cols);
    physics_world_resolve_collision_pairs(&world, NULL);

    BANANA_TEST_ASSERT_INT_EQ(g_resolve_calls, 0,
                              "world collision must no-op on null inputs");
}

static void test_world_collision_skips_invalid_pairs_and_resolves_valid_pair(void **state)
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
    valid_a.id = 11;
    valid_b.id = 27;

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
    cols.items[2].body_b = 55;
    cols.items[3].body_a = valid_a.id;
    cols.items[3].body_b = valid_b.id;

    reset_probe_state();
    physics_world_resolve_collision_pairs(&world, &cols);

    BANANA_TEST_ASSERT_INT_EQ(g_resolve_calls, 1,
                              "world collision must resolve exactly one valid pair");
    BANANA_TEST_ASSERT_INT_EQ((int)g_last_a_id,
                              (int)valid_a.id,
                              "world collision must pass mapped body A");
    BANANA_TEST_ASSERT_INT_EQ((int)g_last_b_id,
                              (int)valid_b.id,
                              "world collision must pass mapped body B");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_world_collision_null_guards_do_not_resolve),
    BANANA_TEST_CASE(test_world_collision_skips_invalid_pairs_and_resolves_valid_pair)
)
