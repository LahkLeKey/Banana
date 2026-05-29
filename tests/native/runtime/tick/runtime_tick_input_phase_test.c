#include "runtime/tick/tick_input_phase.h"
#include "runtime/input/contract/input_contract.h"

#include <stdio.h>

static int s_click_callback_calls = 0;
static float s_last_click_x = 0.f;
static float s_last_click_y = 0.f;

static void click_callback(void *context, float normalized_x, float normalized_y)
{
    (void)context;
    s_click_callback_calls += 1;
    s_last_click_x = normalized_x;
    s_last_click_y = normalized_y;
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    runtime_input_contract_reset();

    if (!expect_int("dispatch invalid input dims",
                    runtime_tick_input_phase_dispatch(NULL,
                                                      click_callback,
                                                      10.f,
                                                      20.f,
                                                      0,
                                                      0),
                    0))
        return 1;

    if (!expect_int("click count increments even when normalization fails",
                    runtime_input_contract_get_click_count(),
                    1))
        return 1;

    if (!expect_int("callback not called on invalid dims", s_click_callback_calls, 0))
        return 1;

    if (!expect_int("dispatch valid",
                    runtime_tick_input_phase_dispatch(NULL,
                                                      click_callback,
                                                      50.f,
                                                      100.f,
                                                      100,
                                                      200),
                    1))
        return 1;

    if (!expect_int("click count increments on valid dispatch",
                    runtime_input_contract_get_click_count(),
                    2))
        return 1;

    if (!expect_int("callback called on valid dispatch", s_click_callback_calls, 1))
        return 1;

    if (!expect_true("normalized x centered", s_last_click_x > -0.001f && s_last_click_x < 0.001f))
        return 1;

    if (!expect_true("normalized y centered", s_last_click_y > -0.001f && s_last_click_y < 0.001f))
        return 1;

    /* Seam: right-click polling counter must advance on every dispatch regardless of dims. */
    if (!expect_int("dispatch zero-width keeps polling count",
                    runtime_tick_input_phase_dispatch(NULL,
                                                      click_callback,
                                                      5.f,
                                                      5.f,
                                                      0,
                                                      100),
                    0))
        return 1;
    if (!expect_int("click count tracks polling even on zero width",
                    runtime_input_contract_get_click_count(),
                    3))
        return 1;

    /* Seam: input normalization delegation honors NULL callback without crashing or skipping the contract. */
    if (!expect_int("valid dispatch with NULL callback returns success",
                    runtime_tick_input_phase_dispatch(NULL,
                                                      NULL,
                                                      25.f,
                                                      50.f,
                                                      100,
                                                      200),
                    1))
        return 1;
    if (!expect_int("NULL callback dispatch still increments contract click count",
                    runtime_input_contract_get_click_count(),
                    4))
        return 1;
    if (!expect_int("NULL callback dispatch leaves user callback untouched",
                    s_click_callback_calls,
                    1))
        return 1;

    /* Null-guard: process() with NULL window must early-return without touching the contract. */
    runtime_tick_input_phase_process(NULL, NULL, click_callback);
    if (!expect_int("process(NULL window) does not advance click count",
                    runtime_input_contract_get_click_count(),
                    4))
        return 1;
    if (!expect_int("process(NULL window) does not invoke callback",
                    s_click_callback_calls,
                    1))
        return 1;

    return 0;
}
