#include "ai/navigation.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

static void test_nav_grid_defaults_to_open(void **state)
{
    (void)state;
    NavGrid *grid = nav_grid_create(1.0f, 0.0f, 0.0f);
    assert_non_null(grid);

    assert_float_equal(grid->cell_size, 1.0f, 0.0001f);
    assert_float_equal(grid->origin[0], 0.0f, 0.0001f);
    assert_float_equal(grid->origin[1], 0.0f, 0.0001f);
    assert_int_equal(grid->passable[0][0], 1);
    assert_int_equal(grid->passable[5][7], 1);

    nav_grid_set_blocked(grid, 0, 0, 1);
    assert_int_equal(grid->passable[0][0], 0);

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

    assert_true(nav_find_path(&grid, 0.0f, 0.0f, 2.0f, 2.0f, &path));
    assert_true(path.count > 0);
    assert_int_equal(path.current_index, 0);
    assert_true(path.waypoints[0][0] >= 0.0f);
    assert_true(path.waypoints[path.count - 1][2] >= 0.0f);
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

    assert_int_equal(nav_move_along_path(&path, position, 1.0f, 1.0f), 1);
    assert_float_equal(position[0], 1.0f, 0.0001f);
    assert_float_equal(position[2], 0.0f, 0.0001f);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_nav_grid_defaults_to_open),
        cmocka_unit_test(test_nav_find_path_returns_waypoints),
        cmocka_unit_test(test_nav_move_along_path_advances_position),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#else
#include <stdio.h>

static int fail_if(int condition, const char *message)
{
    if (condition)
    {
        fprintf(stderr, "%s\n", message);
        return 1;
    }
    return 0;
}

static int test_nav_grid_defaults_to_open(void)
{
    NavGrid *grid = nav_grid_create(1.0f, 0.0f, 0.0f);
    if (!grid)
        return fail_if(1, "nav_grid_create must allocate a usable grid");

    if (fail_if(grid->cell_size != 1.0f, "grid cell size must be preserved") ||
        fail_if(grid->origin[0] != 0.0f || grid->origin[1] != 0.0f,
                "grid origin must be preserved") ||
        fail_if(grid->passable[0][0] != 1 || grid->passable[5][7] != 1,
                "new nav grids must start open for all cells"))
    {
        nav_grid_destroy(grid);
        return 1;
    }

    nav_grid_set_blocked(grid, 0, 0, 1);
    if (fail_if(grid->passable[0][0] != 0, "nav_grid_set_blocked must close a cell"))
    {
        nav_grid_destroy(grid);
        return 1;
    }

    nav_grid_destroy(grid);
    return 0;
}

static int test_nav_find_path_returns_waypoints(void)
{
    NavGrid grid;
    NavPath path;

    grid.cell_size = 1.0f;
    grid.origin[0] = 0.0f;
    grid.origin[1] = 0.0f;
    for (int c = 0; c < NAV_GRID_W; ++c)
        for (int r = 0; r < NAV_GRID_H; ++r)
            grid.passable[c][r] = 1;

    if (!nav_find_path(&grid, 0.0f, 0.0f, 2.0f, 2.0f, &path))
        return fail_if(1, "nav_find_path must find a path for an open grid");

    if (fail_if(path.count <= 0, "nav_find_path must populate at least one waypoint") ||
        fail_if(path.current_index != 0, "nav_find_path must start at the first waypoint") ||
        fail_if(path.waypoints[0][0] < 0.0f || path.waypoints[path.count - 1][2] < 0.0f,
                "nav_find_path must return world-space waypoint coordinates"))
        return 1;

    return 0;
}

static int test_nav_move_along_path_advances_position(void)
{
    NavPath path;
    float position[3] = {0.0f, 0.0f, 0.0f};

    path.count = 1;
    path.current_index = 0;
    path.waypoints[0][0] = 1.0f;
    path.waypoints[0][1] = 0.0f;
    path.waypoints[0][2] = 0.0f;

    if (fail_if(nav_move_along_path(&path, position, 1.0f, 1.0f) != 1,
                "nav_move_along_path must report completion when it reaches the waypoint") ||
        fail_if(position[0] != 1.0f || position[2] != 0.0f,
                "nav_move_along_path must update the position in place"))
        return 1;

    return 0;
}

int main(void)
{
    if (test_nav_grid_defaults_to_open() ||
        test_nav_find_path_returns_waypoints() ||
        test_nav_move_along_path_advances_position())
        return 1;

    return 0;
}
#endif
