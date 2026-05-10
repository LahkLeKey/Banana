#include "test_engine_domain_suite_framework.h"

#include "../../../src/native/engine/physics/body.h"
#include "../../../src/native/engine/physics/collider.h"
#include "../../../src/native/engine/physics/dynamics.h"
#include "../../../src/native/engine/physics/world.h"

static void test_physics_body_create(DomainSuiteStats *stats)
{
    PhysicsBody *b = NULL;
    SUITE_TEST("physics_body_create: position and mass are set correctly");
    b = physics_body_create(1, 10.f, 1.f, 2.f, 3.f, SHAPE_BOX);
    SUITE_ASSERT_TRUE(stats, b != NULL);
    SUITE_ASSERT_NEAR(stats, b->position[0], 1.f, 0.001f);
    SUITE_ASSERT_NEAR(stats, b->position[1], 2.f, 0.001f);
    SUITE_ASSERT_NEAR(stats, b->position[2], 3.f, 0.001f);
    SUITE_ASSERT_NEAR(stats, b->mass, 10.f, 0.001f);
    SUITE_ASSERT_INT_EQ(stats, b->is_static, 0);
    SUITE_PASS(stats);
done:
    if (b)
        physics_body_destroy(b);
}

static void test_physics_static_body(DomainSuiteStats *stats)
{
    PhysicsBody *b = NULL;
    SUITE_TEST("physics_body: mass=0 makes body static");
    b = physics_body_create(1, 0.f, 0, 0, 0, SHAPE_BOX);
    SUITE_ASSERT_TRUE(stats, b != NULL);
    SUITE_ASSERT_INT_EQ(stats, b->is_static, 1);
    SUITE_PASS(stats);
done:
    if (b)
        physics_body_destroy(b);
}

static void test_dynamics_gravity(DomainSuiteStats *stats)
{
    PhysicsBody *b = NULL;
    float start_y = 0.0f;

    SUITE_TEST("dynamics_integrate: body falls under gravity");
    b = physics_body_create(1, 1.f, 0.f, 10.f, 0.f, SHAPE_BOX);
    SUITE_ASSERT_TRUE(stats, b != NULL);
    start_y = b->position[1];
    dynamics_integrate(b, 1.f / 60.f);
    SUITE_ASSERT_TRUE(stats, b->position[1] < start_y);
    SUITE_PASS(stats);
done:
    if (b)
        physics_body_destroy(b);
}

static void test_dynamics_static_no_move(DomainSuiteStats *stats)
{
    PhysicsBody *b = NULL;
    float px = 0.0f, py = 0.0f, pz = 0.0f;

    SUITE_TEST("dynamics_integrate: static body does not move");
    b = physics_body_create(1, 0.f, 5.f, 5.f, 5.f, SHAPE_BOX);
    SUITE_ASSERT_TRUE(stats, b != NULL);

    px = b->position[0];
    py = b->position[1];
    pz = b->position[2];

    dynamics_integrate(b, 1.f / 60.f);
    SUITE_ASSERT_NEAR(stats, b->position[0], px, 0.001f);
    SUITE_ASSERT_NEAR(stats, b->position[1], py, 0.001f);
    SUITE_ASSERT_NEAR(stats, b->position[2], pz, 0.001f);
    SUITE_PASS(stats);
done:
    if (b)
        physics_body_destroy(b);
}

static void test_physics_impulse(DomainSuiteStats *stats)
{
    PhysicsBody *b = NULL;
    SUITE_TEST("physics_body_apply_impulse: velocity changes immediately");
    b = physics_body_create(1, 2.f, 0, 0, 0, SHAPE_BOX);
    SUITE_ASSERT_TRUE(stats, b != NULL);
    physics_body_apply_impulse(b, 10.f, 0.f, 0.f);
    SUITE_ASSERT_NEAR(stats, b->velocity[0], 5.f, 0.01f);
    SUITE_PASS(stats);
done:
    if (b)
        physics_body_destroy(b);
}

static void test_box_box_collision(DomainSuiteStats *stats)
{
    PhysicsBody *a = NULL;
    PhysicsBody *b = NULL;
    Collision c;
    int hit = 0;

    SUITE_TEST("collider_box_vs_box: overlapping boxes produce collision");
    a = physics_body_create(1, 1.f, 0.f, 0.f, 0.f, SHAPE_BOX);
    b = physics_body_create(2, 1.f, 0.8f, 0.f, 0.f, SHAPE_BOX);
    SUITE_ASSERT_TRUE(stats, a != NULL && b != NULL);

    hit = collider_box_vs_box(a, b, &c);
    SUITE_ASSERT_INT_EQ(stats, hit, 1);
    SUITE_ASSERT_TRUE(stats, c.penetration > 0.f);
    SUITE_PASS(stats);
done:
    if (a)
        physics_body_destroy(a);
    if (b)
        physics_body_destroy(b);
}

static void test_box_box_no_collision(DomainSuiteStats *stats)
{
    PhysicsBody *a = NULL;
    PhysicsBody *b = NULL;
    Collision c;
    int hit = 0;

    SUITE_TEST("collider_box_vs_box: non-overlapping boxes have no collision");
    a = physics_body_create(1, 1.f, 0.f, 0.f, 0.f, SHAPE_BOX);
    b = physics_body_create(2, 1.f, 5.f, 0.f, 0.f, SHAPE_BOX);
    SUITE_ASSERT_TRUE(stats, a != NULL && b != NULL);

    hit = collider_box_vs_box(a, b, &c);
    SUITE_ASSERT_INT_EQ(stats, hit, 0);
    SUITE_PASS(stats);
done:
    if (a)
        physics_body_destroy(a);
    if (b)
        physics_body_destroy(b);
}

static void test_physics_world_determinism(DomainSuiteStats *stats)
{
    float pos1[3], pos2[3];

    SUITE_TEST("physics_world: 60-frame simulation is deterministic");
    for (int run = 0; run < 2; run++)
    {
        PhysicsWorld *w = physics_world_create();
        PhysicsBody *floor = NULL;
        float *out = (run == 0) ? pos1 : pos2;

        SUITE_ASSERT_TRUE(stats, w != NULL);
        (void)physics_world_add_body(w, 1.f, 0.f, 10.f, 0.f, SHAPE_BOX);

        floor = physics_world_add_body(w, 0.f, 0.f, -0.5f, 0.f, SHAPE_BOX);
        SUITE_ASSERT_TRUE(stats, floor != NULL);
        floor->shape.box.half_extents[0] = 50.f;
        floor->shape.box.half_extents[1] = 0.5f;
        floor->shape.box.half_extents[2] = 50.f;

        for (int i = 0; i < 60; i++)
            physics_world_step_fixed(w);

        out[0] = w->bodies[0]->position[0];
        out[1] = w->bodies[0]->position[1];
        out[2] = w->bodies[0]->position[2];
        physics_world_destroy(w);
    }

    SUITE_ASSERT_NEAR(stats, pos1[0], pos2[0], 0.0001f);
    SUITE_ASSERT_NEAR(stats, pos1[1], pos2[1], 0.0001f);
    SUITE_ASSERT_NEAR(stats, pos1[2], pos2[2], 0.0001f);
    SUITE_PASS(stats);
done:
    return;
}

int run_engine_physics_suite(void)
{
    DomainSuiteStats stats = {0, 0};

    printf("-- Physics Suite --\n");
    test_physics_body_create(&stats);
    test_physics_static_body(&stats);
    test_dynamics_gravity(&stats);
    test_dynamics_static_no_move(&stats);
    test_physics_impulse(&stats);
    test_box_box_collision(&stats);
    test_box_box_no_collision(&stats);
    test_physics_world_determinism(&stats);

    printf("Physics: %d passed, %d failed\n\n", stats.pass, stats.fail);
    return stats.fail;
}
