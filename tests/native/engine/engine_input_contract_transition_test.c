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
    /* Ensure deterministic baseline for global input-contract state. */
    engine_shutdown();

    if (!expect_int("baseline click count", engine_get_click_count(), 0))
        return 1;
    if (!expect_int("baseline target reached", engine_get_target_reached_count(), 0))
        return 1;
    if (!expect_int("baseline has target", engine_get_has_move_target(), 0))
        return 1;

    if (!expect_int("engine right click accepted", engine_handle_right_click(48.f, 92.f), 1))
        return 1;
    if (!expect_int("click count after right click", engine_get_click_count(), 1))
        return 1;
    if (!expect_int("has target after right click", engine_get_has_move_target(), 1))
        return 1;

    /* Manual movement intent should cancel click-target ownership. */
    engine_set_move_input(0.45f, 0.0f);
    if (!expect_int("click count stable after manual input", engine_get_click_count(), 1))
        return 1;
    if (!expect_int("has target cleared by manual input", engine_get_has_move_target(), 0))
        return 1;

    if (!expect_int("normalized click accepted", engine_handle_right_click_normalized(0.0f, 0.0f), 1))
        return 1;
    if (!expect_int("normalized click does not change click count", engine_get_click_count(), 1))
        return 1;
    if (!expect_int("normalized click sets target flag", engine_get_has_move_target(), 1))
        return 1;

    if (!expect_int("normalized out-of-range rejected", engine_handle_right_click_normalized(3.0f, 0.0f), 0))
        return 1;

    engine_shutdown();

    if (!expect_int("reset click count", engine_get_click_count(), 0))
        return 1;
    if (!expect_int("reset target reached", engine_get_target_reached_count(), 0))
        return 1;
    if (!expect_int("reset has target", engine_get_has_move_target(), 0))
        return 1;

    return 0;
}
