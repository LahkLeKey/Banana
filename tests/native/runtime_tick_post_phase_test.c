#include "runtime/tick_post_phase.h"

#include <stdio.h>

static int s_follow_calls = 0;
static int s_render_calls = 0;

static void follow_cb(void *context)
{
    (void)context;
    s_follow_calls += 1;
}

static void render_cb(void *context)
{
    (void)context;
    s_render_calls += 1;
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    runtime_tick_post_phase_execute(NULL,
                                    NULL,
                                    0,
                                    1.0f / 60.0f,
                                    NULL,
                                    follow_cb,
                                    render_cb);

    if (!expect_int("follow called", s_follow_calls, 1))
        return 1;
    if (!expect_int("render called", s_render_calls, 1))
        return 1;

    s_follow_calls = 0;
    s_render_calls = 0;

    runtime_tick_post_phase_execute(NULL,
                                    NULL,
                                    0,
                                    1.0f / 60.0f,
                                    NULL,
                                    NULL,
                                    NULL);

    if (!expect_int("follow skipped when null", s_follow_calls, 0))
        return 1;
    if (!expect_int("render skipped when null", s_render_calls, 0))
        return 1;

    return 0;
}
