#include "physics/collider.h"

#include "runtime/support/test_support.h"

static void test_box_vs_box_no_overlap(void **state)
{
    (void)state;
    PhysicsBody a = { .id = 1, .position = {0.f, 0.f, 0.f},
                      .shape_type = SHAPE_BOX,
                      .shape = {.box = {.half_extents = {1.f, 1.f, 1.f}}} };
    PhysicsBody b = { .id = 2, .position = {5.f, 0.f, 0.f},
                      .shape_type = SHAPE_BOX,
                      .shape = {.box = {.half_extents = {1.f, 1.f, 1.f}}} };
    Collision c = {0};

    BANANA_TEST_ASSERT_INT_EQ(collider_box_vs_box(&a, &b, &c),
                              0,
                              "distant boxes must not collide");
}

static void test_box_vs_box_x_overlap(void **state)
{
    (void)state;
    PhysicsBody a = { .id = 1, .position = {0.f, 0.f, 0.f},
                      .shape_type = SHAPE_BOX,
                      .shape = {.box = {.half_extents = {1.f, 1.f, 1.f}}} };
    PhysicsBody b = { .id = 2, .position = {1.4f, 0.f, 0.f},
                      .shape_type = SHAPE_BOX,
                      .shape = {.box = {.half_extents = {1.f, 1.f, 1.f}}} };
    Collision c = {0};

    BANANA_TEST_ASSERT_TRUE(collider_box_vs_box(&a, &b, &c),
                            "overlapping boxes on x-axis must collide");
    BANANA_TEST_ASSERT_TRUE(c.penetration > 0.f,
                            "x overlap must report positive penetration");
    BANANA_TEST_ASSERT_FLOAT_EQ(c.normal[0], 1.f, 0.001f,
                                "x-axis overlap must produce normal in +x direction");
}

static void test_box_vs_box_y_overlap(void **state)
{
    (void)state;
    PhysicsBody a = { .id = 3, .position = {0.f, 0.f, 0.f},
                      .shape_type = SHAPE_BOX,
                      .shape = {.box = {.half_extents = {2.f, 0.5f, 2.f}}} };
    PhysicsBody b = { .id = 4, .position = {0.f, 0.6f, 0.f},
                      .shape_type = SHAPE_BOX,
                      .shape = {.box = {.half_extents = {2.f, 0.5f, 2.f}}} };
    Collision c = {0};

    BANANA_TEST_ASSERT_TRUE(collider_box_vs_box(&a, &b, &c),
                            "overlapping boxes on y-axis must collide");
    BANANA_TEST_ASSERT_TRUE(c.penetration > 0.f,
                            "y overlap must report positive penetration");
    BANANA_TEST_ASSERT_FLOAT_EQ(c.normal[1], 1.f, 0.001f,
                                "y-axis overlap must produce normal in +y direction");
}

static void test_box_vs_box_z_overlap(void **state)
{
    (void)state;
    PhysicsBody a = { .id = 5, .position = {0.f, 0.f, 0.f},
                      .shape_type = SHAPE_BOX,
                      .shape = {.box = {.half_extents = {2.f, 2.f, 0.5f}}} };
    PhysicsBody b = { .id = 6, .position = {0.f, 0.f, 0.6f},
                      .shape_type = SHAPE_BOX,
                      .shape = {.box = {.half_extents = {2.f, 2.f, 0.5f}}} };
    Collision c = {0};

    BANANA_TEST_ASSERT_TRUE(collider_box_vs_box(&a, &b, &c),
                            "overlapping boxes on z-axis must collide");
    BANANA_TEST_ASSERT_FLOAT_EQ(c.normal[2], 1.f, 0.001f,
                                "z-axis overlap must produce normal in +z direction");
}

static void test_sphere_vs_sphere(void **state)
{
    (void)state;
    PhysicsBody a = { .id = 7, .position = {0.f, 0.f, 0.f},
                      .shape_type = SHAPE_SPHERE,
                      .shape = {.sphere = {.radius = 1.f}} };
    PhysicsBody b_overlap = { .id = 8, .position = {1.5f, 0.f, 0.f},
                              .shape_type = SHAPE_SPHERE,
                              .shape = {.sphere = {.radius = 1.f}} };
    PhysicsBody b_miss = { .id = 9, .position = {5.f, 0.f, 0.f},
                           .shape_type = SHAPE_SPHERE,
                           .shape = {.sphere = {.radius = 0.5f}} };
    Collision c = {0};

    BANANA_TEST_ASSERT_TRUE(collider_sphere_vs_sphere(&a, &b_overlap, &c),
                            "overlapping spheres must collide");
    BANANA_TEST_ASSERT_TRUE(c.penetration > 0.f,
                            "sphere overlap must report positive penetration");
    BANANA_TEST_ASSERT_INT_EQ(collider_sphere_vs_sphere(&a, &b_miss, &c),
                              0,
                              "distant spheres must not collide");
}

static void test_box_vs_sphere(void **state)
{
    (void)state;
    PhysicsBody box = { .id = 10, .position = {0.f, 0.f, 0.f},
                        .shape_type = SHAPE_BOX,
                        .shape = {.box = {.half_extents = {1.f, 1.f, 1.f}}} };
    PhysicsBody sphere_hit = { .id = 11, .position = {1.5f, 0.f, 0.f},
                               .shape_type = SHAPE_SPHERE,
                               .shape = {.sphere = {.radius = 1.f}} };
    PhysicsBody sphere_miss = { .id = 12, .position = {5.f, 0.f, 0.f},
                                .shape_type = SHAPE_SPHERE,
                                .shape = {.sphere = {.radius = 0.5f}} };
    Collision c = {0};

    BANANA_TEST_ASSERT_TRUE(collider_box_vs_sphere(&box, &sphere_hit, &c),
                            "sphere penetrating box face must collide");
    BANANA_TEST_ASSERT_INT_EQ(collider_box_vs_sphere(&box, &sphere_miss, &c),
                              0,
                              "distant sphere-box pair must not collide");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_box_vs_box_no_overlap),
    BANANA_TEST_CASE(test_box_vs_box_x_overlap),
    BANANA_TEST_CASE(test_box_vs_box_y_overlap),
    BANANA_TEST_CASE(test_box_vs_box_z_overlap),
    BANANA_TEST_CASE(test_sphere_vs_sphere),
    BANANA_TEST_CASE(test_box_vs_sphere)
)
