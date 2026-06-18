#include "ai/controller.h"
#include "ai/controller_system.h"
#include "ai/navigation.h"

#include <stdio.h>
#include <string.h>

static int fail_if(int condition, const char *message)
{
    if (condition)
    {
        fprintf(stderr, "%s\n", message);
        return 1;
    }
    return 0;
}

static int test_controller_team_helpers(void)
{
    if (fail_if(strcmp(controller_team_name(CONTROLLER_TEAM_NEUTRAL), "neutral") != 0,
                "neutral team name must resolve to 'neutral'"))
        return 1;

    if (fail_if(strcmp(controller_team_name(CONTROLLER_TEAM_BANANA), "banana") != 0,
                "banana team name must resolve to 'banana'"))
        return 1;

    if (fail_if(strcmp(controller_team_name(CONTROLLER_TEAM_BEAN), "bean") != 0,
                "bean team name must resolve to 'bean'"))
        return 1;

    if (fail_if(controller_teams_are_hostile(CONTROLLER_TEAM_BANANA, CONTROLLER_TEAM_BEAN) != 1,
                "banana and bean teams must be hostile"))
        return 1;

    if (fail_if(controller_teams_are_hostile(CONTROLLER_TEAM_BEAN, CONTROLLER_TEAM_BANANA) != 1,
                "bean and banana teams must be hostile"))
        return 1;

    if (fail_if(controller_teams_are_hostile(CONTROLLER_TEAM_BANANA, CONTROLLER_TEAM_BANANA) != 0,
                "same-team controllers must not be hostile"))
        return 1;

    return 0;
}

static int test_navigation_blocked_grid_returns_no_path(void)
{
    NavGrid *grid = nav_grid_create(1.0f, 0.0f, 0.0f);
    NavPath path;
    int row;
    int col;

    if (!grid)
        return fail_if(1, "nav_grid_create must allocate a nav grid");

    for (row = 0; row < NAV_GRID_H; ++row)
    {
        for (col = 0; col < NAV_GRID_W; ++col)
        {
            nav_grid_set_blocked(grid, col, row, 1);
        }
    }

    if (fail_if(nav_find_path(grid, 0.0f, 0.0f, 2.0f, 2.0f, &path) != 0,
                "blocked nav grids must not return a path"))
    {
        nav_grid_destroy(grid);
        return 1;
    }

    nav_grid_destroy(grid);
    return 0;
}

int main(void)
{
    if (test_controller_team_helpers() || test_navigation_blocked_grid_returns_no_path())
        return 1;

    return 0;
}
