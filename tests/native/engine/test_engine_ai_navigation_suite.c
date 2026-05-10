#include "test_engine_domain_suite_framework.h"

#include "../../../src/native/engine/ai/navigation.h"

static void test_nav_find_path_simple(DomainSuiteStats *stats)
{
    NavGrid *g = NULL;
    NavPath path;
    int ok = 0;

    SUITE_TEST("nav_find_path: finds path on open grid");
    g = nav_grid_create(1.f, 0.f, 0.f);
    SUITE_ASSERT_TRUE(stats, g != NULL);
    ok = nav_find_path(g, 0.f, 0.f, 5.f, 5.f, &path);
    SUITE_ASSERT_INT_EQ(stats, ok, 1);
    SUITE_ASSERT_TRUE(stats, path.count > 0);
    SUITE_PASS(stats);
done:
    if (g)
        nav_grid_destroy(g);
}

static void test_nav_find_path_blocked(DomainSuiteStats *stats)
{
    NavGrid *g = NULL;
    NavPath path;
    int ok = 0;

    SUITE_TEST("nav_find_path: returns 0 when fully blocked");
    g = nav_grid_create(1.f, 0.f, 0.f);
    SUITE_ASSERT_TRUE(stats, g != NULL);

    for (int r = 0; r < NAV_GRID_H; r++)
        for (int c = 1; c < NAV_GRID_W; c++)
            nav_grid_set_blocked(g, c, r, 1);

    ok = nav_find_path(g, 0.f, 0.f, 10.f, 10.f, &path);
    SUITE_ASSERT_INT_EQ(stats, ok, 0);
    SUITE_PASS(stats);
done:
    if (g)
        nav_grid_destroy(g);
}

static void test_nav_move_along_path(DomainSuiteStats *stats)
{
    NavPath path;
    float pos[3] = {0.f, 0.f, 0.f};

    SUITE_TEST("nav_move_along_path: position advances toward waypoint");
    path.count = 1;
    path.current_index = 0;
    path.waypoints[0][0] = 10.f;
    path.waypoints[0][1] = 0.f;
    path.waypoints[0][2] = 0.f;

    nav_move_along_path(&path, pos, 5.f, 1.f);
    SUITE_ASSERT_NEAR(stats, pos[0], 5.f, 0.01f);
    SUITE_PASS(stats);
done:
    return;
}

int run_engine_ai_navigation_suite(void)
{
    DomainSuiteStats stats = {0, 0};

    printf("-- AI Navigation Suite --\n");
    test_nav_find_path_simple(&stats);
    test_nav_find_path_blocked(&stats);
    test_nav_move_along_path(&stats);

    printf("AI Navigation: %d passed, %d failed\n\n", stats.pass, stats.fail);
    return stats.fail;
}
