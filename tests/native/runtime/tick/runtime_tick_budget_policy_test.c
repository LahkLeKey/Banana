#include "runtime/tick/tick_budget_policy.h"

#include <stdio.h>

#if defined(_WIN32)
#include <stdlib.h>
#define setenv(name, value, overwrite) _putenv_s((name), (value))
#define unsetenv(name) _putenv_s((name), "")
#endif

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    unsetenv("BANANA_TERRAIN_REBUILD_BUDGET");
    if (!expect_int("default budget", runtime_tick_budget_policy_terrain_chunks_per_tick(), 2))
        return 1;

    setenv("BANANA_TERRAIN_REBUILD_BUDGET", "8", 1);
    if (!expect_int("configured budget", runtime_tick_budget_policy_terrain_chunks_per_tick(), 8))
        return 1;

    setenv("BANANA_TERRAIN_REBUILD_BUDGET", "0", 1);
    if (!expect_int("minimum clamp", runtime_tick_budget_policy_terrain_chunks_per_tick(), 1))
        return 1;

    setenv("BANANA_TERRAIN_REBUILD_BUDGET", "9999", 1);
    if (!expect_int("maximum clamp", runtime_tick_budget_policy_terrain_chunks_per_tick(), 128))
        return 1;

    unsetenv("BANANA_TERRAIN_REBUILD_BUDGET");
    return 0;
}
