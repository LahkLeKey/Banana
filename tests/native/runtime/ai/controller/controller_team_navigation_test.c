#include "ai/controller.h"
#include "ai/controller_system.h"
#include "ai/navigation.h"
#include "../../support/test_support.h"

#include <string.h>

static void test_controller_team_helpers(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_STR_EQ(controller_team_name(CONTROLLER_TEAM_NEUTRAL), "neutral",
                              "neutral team name must resolve to neutral");
    BANANA_TEST_ASSERT_STR_EQ(controller_team_name(CONTROLLER_TEAM_BANANA), "banana",
                              "banana team name must resolve to banana");
    BANANA_TEST_ASSERT_STR_EQ(controller_team_name(CONTROLLER_TEAM_BEAN), "bean",
                              "bean team name must resolve to bean");
    BANANA_TEST_ASSERT_INT_EQ(controller_teams_are_hostile(CONTROLLER_TEAM_BANANA, CONTROLLER_TEAM_BEAN), 1,
                              "banana and bean teams must be hostile");
    BANANA_TEST_ASSERT_INT_EQ(controller_teams_are_hostile(CONTROLLER_TEAM_BEAN, CONTROLLER_TEAM_BANANA), 1,
                              "bean and banana teams must be hostile");
    BANANA_TEST_ASSERT_INT_EQ(controller_teams_are_hostile(CONTROLLER_TEAM_BANANA, CONTROLLER_TEAM_BANANA), 0,
                              "same-team controllers must not be hostile");
}

static void test_nav_move_along_path_advances_between_waypoints(void **state)
{
    (void)state;
    NavPath path;
    float position[3] = {0.0f, 0.0f, 0.0f};

    memset(&path, 0, sizeof(path));
    path.count = 2;
    path.current_index = 0;
    path.waypoints[0][0] = 4.0f;
    path.waypoints[0][1] = 0.0f;
    path.waypoints[0][2] = 0.0f;
    path.waypoints[1][0] = 8.0f;
    path.waypoints[1][1] = 0.0f;
    path.waypoints[1][2] = 0.0f;

    BANANA_TEST_ASSERT_INT_EQ(nav_move_along_path(&path, position, 10.0f, 1.0f), 0,
                              "first move should advance to the first waypoint");
    BANANA_TEST_ASSERT_INT_EQ(path.current_index, 1,
                              "path should advance to the next waypoint after the first move");
    BANANA_TEST_ASSERT_FLOAT_EQ(position[0], 4.0f, 0.001f,
                                "position x should match the first waypoint after first move");
    BANANA_TEST_ASSERT_FLOAT_EQ(position[2], 0.0f, 0.001f,
                                "position z should match the first waypoint after first move");

    BANANA_TEST_ASSERT_INT_EQ(nav_move_along_path(&path, position, 10.0f, 1.0f), 1,
                              "second move should complete the path");
    BANANA_TEST_ASSERT_INT_EQ(path.current_index, 2,
                              "path should mark itself complete after reaching destination");
    BANANA_TEST_ASSERT_FLOAT_EQ(position[0], 8.0f, 0.001f,
                                "position x should match the final waypoint");
    BANANA_TEST_ASSERT_FLOAT_EQ(position[2], 0.0f, 0.001f,
                                "position z should match the final waypoint");
}

static void test_navigation_blocked_grid_returns_no_path(void **state)
{
    (void)state;
    NavGrid *grid = nav_grid_create(1.0f, 0.0f, 0.0f);
    NavPath path;
    int row;
    int col;

    if (!grid)
    {
        BANANA_TEST_FAIL("nav_grid_create must allocate a nav grid");
    }

    for (row = 0; row < NAV_GRID_H; ++row)
    {
        for (col = 0; col < NAV_GRID_W; ++col)
        {
            nav_grid_set_blocked(grid, col, row, 1);
        }
    }

    BANANA_TEST_ASSERT_INT_EQ(nav_find_path(grid, 0.0f, 0.0f, 2.0f, 2.0f, &path), 0,
                              "blocked nav grids must not return a path");

    nav_grid_destroy(grid);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_controller_team_helpers),
    BANANA_TEST_CASE(test_nav_move_along_path_advances_between_waypoints),
    BANANA_TEST_CASE(test_navigation_blocked_grid_returns_no_path)
)
