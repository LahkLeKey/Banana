#include "engine.h"

#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    const float dt = 1.0f / 60.0f;
    int baseline_reached = 0;
    int reached = 0;

    engine_shutdown();
    if (!expect_int("engine init", engine_init(640, 360), 0))
        return 1;

    if (!expect_int("initial tick", engine_tick(dt), 0))
        return 1;

    baseline_reached = engine_get_target_reached_count();

    if (!expect_int("normalized click accepted", engine_handle_right_click_normalized(0.0f, 0.0f), 1))
        return 1;
    if (!expect_int("target active after click", engine_get_has_move_target(), 1))
        return 1;

    for (int i = 0; i < 600; i++)
    {
        if (!expect_int("tick during move", engine_tick(dt), 0))
            return 1;

        reached = engine_get_target_reached_count();
        if (reached > baseline_reached)
            break;
    }

    reached = engine_get_target_reached_count();
    if (!expect_int("target reached incremented", reached > baseline_reached ? 1 : 0, 1))
        return 1;
    if (!expect_int("target cleared after arrival", engine_get_has_move_target(), 0))
        return 1;

    engine_shutdown();
    return 0;
}
