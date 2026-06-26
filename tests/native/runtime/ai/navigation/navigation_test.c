#include "ai/navigation.h"
#include "runtime/support/test_support.h"

static void test_nav_grid_defaults_to_open(void **state)
{
    (void)state;
    NavGrid *grid = nav_grid_create(1.0f, 0.0f, 0.0f);
    BANANA_TEST_ASSERT_TRUE(grid != NULL, "nav_grid_create must allocate a usable grid");

    BANANA_TEST_ASSERT_FLOAT_EQ(grid->cell_size, 1.0f, 0.0001f, "grid cell size must be preserved");
    BANANA_TEST_ASSERT_FLOAT_EQ(grid->origin[0], 0.0f, 0.0001f, "grid origin x must be preserved");
    BANANA_TEST_ASSERT_FLOAT_EQ(grid->origin[1], 0.0f, 0.0001f, "grid origin z must be preserved");
    BANANA_TEST_ASSERT_INT_EQ(grid->passable[0][0], 1, "new nav grids must start open for all cells");
    BANANA_TEST_ASSERT_INT_EQ(grid->passable[5][7], 1, "new nav grids must start open for all cells");

    nav_grid_set_blocked(grid, 0, 0, 1);
    BANANA_TEST_ASSERT_INT_EQ(grid->passable[0][0], 0, "nav_grid_set_blocked must close a cell");

    nav_grid_destroy(grid);
}

static void test_nav_find_path_returns_waypoints(void **state)
{
    (void)state;
    NavGrid grid;
    NavPath path;

    grid.cell_size = 1.0f;
    grid.origin[0] = 0.0f;
    grid.origin[1] = 0.0f;
    for (int c = 0; c < NAV_GRID_W; ++c)
        for (int r = 0; r < NAV_GRID_H; ++r)
            grid.passable[c][r] = 1;

    BANANA_TEST_ASSERT_TRUE(nav_find_path(&grid, 0.0f, 0.0f, 2.0f, 2.0f, &path),
                            "nav_find_path must find a path for an open grid");
    BANANA_TEST_ASSERT_TRUE(path.count > 0, "nav_find_path must populate at least one waypoint");
    BANANA_TEST_ASSERT_INT_EQ(path.current_index, 0, "nav_find_path must start at the first waypoint");
    BANANA_TEST_ASSERT_TRUE(path.waypoints[0][0] >= 0.0f,
                            "nav_find_path must return non-negative starting waypoint x");
    BANANA_TEST_ASSERT_TRUE(path.waypoints[path.count - 1][2] >= 0.0f,
                            "nav_find_path must return non-negative final waypoint z");
}

static void test_nav_move_along_path_advances_position(void **state)
{
    (void)state;
    NavPath path;
    float position[3] = {0.0f, 0.0f, 0.0f};

    path.count = 1;
    path.current_index = 0;
    path.waypoints[0][0] = 1.0f;
    path.waypoints[0][1] = 0.0f;
    path.waypoints[0][2] = 0.0f;

    BANANA_TEST_ASSERT_INT_EQ(nav_move_along_path(&path, position, 1.0f, 1.0f), 1,
                              "nav_move_along_path must report completion at waypoint");
    BANANA_TEST_ASSERT_FLOAT_EQ(position[0], 1.0f, 0.0001f,
                                "nav_move_along_path must update position x in place");
    BANANA_TEST_ASSERT_FLOAT_EQ(position[2], 0.0f, 0.0001f,
                                "nav_move_along_path must update position z in place");
}

static void test_nav_grid_set_blocked_out_of_bounds_is_noop(void **state)
{
    (void)state;
    NavGrid *grid = nav_grid_create(1.0f, 0.0f, 0.0f);
    BANANA_TEST_ASSERT_TRUE(grid != NULL, "nav_grid_create must allocate a usable grid");

    nav_grid_set_blocked(grid, 1, 1, 1);
    BANANA_TEST_ASSERT_INT_EQ(grid->passable[1][1], 0, "setup must block an in-bounds cell");

    nav_grid_set_blocked(grid, -1, 1, 0);
    nav_grid_set_blocked(grid, NAV_GRID_W, 1, 0);
    nav_grid_set_blocked(grid, 1, -1, 0);
    nav_grid_set_blocked(grid, 1, NAV_GRID_H, 0);

    BANANA_TEST_ASSERT_INT_EQ(grid->passable[1][1], 0,
                              "out-of-bounds writes must leave grid state unchanged");
    nav_grid_destroy(grid);
}

static void test_nav_move_along_path_partial_step_and_done_guard(void **state)
{
    (void)state;
    NavPath path;
    float position[3] = {0.0f, 0.0f, 0.0f};

    path.count = 1;
    path.current_index = 0;
    path.waypoints[0][0] = 2.0f;
    path.waypoints[0][1] = 0.0f;
    path.waypoints[0][2] = 0.0f;

    BANANA_TEST_ASSERT_INT_EQ(nav_move_along_path(&path, position, 0.5f, 1.0f), 0,
                              "nav_move_along_path must report in-progress when step is short");
    BANANA_TEST_ASSERT_TRUE(position[0] > 0.0f && position[0] < 2.0f,
                            "nav_move_along_path must advance partially toward waypoint");
    BANANA_TEST_ASSERT_INT_EQ(path.current_index, 0,
                              "nav_move_along_path must not advance waypoint index on short step");

    path.current_index = path.count;
    BANANA_TEST_ASSERT_INT_EQ(nav_move_along_path(&path, position, 1.0f, 1.0f), 1,
                              "nav_move_along_path must immediately report done when index is complete");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_nav_grid_defaults_to_open),
    BANANA_TEST_CASE(test_nav_find_path_returns_waypoints),
    BANANA_TEST_CASE(test_nav_move_along_path_advances_position),
    BANANA_TEST_CASE(test_nav_grid_set_blocked_out_of_bounds_is_noop),
    BANANA_TEST_CASE(test_nav_move_along_path_partial_step_and_done_guard)
)
