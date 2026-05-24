#include "runtime/input/click/input_click_policy.h"

#include <math.h>
#include <stdio.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

static int nearly_equal(float a, float b)
{
    float delta = a - b;
    if (delta < 0.f)
        delta = -delta;

    return delta < 0.0001f;
}

int main(void)
{
    float nx = 0.f;
    float ny = 0.f;

    if (!expect_true("invalid width rejected",
                     runtime_input_click_policy_normalize(10.f, 10.f, 0, 100, &nx, &ny) == 0))
        return 1;

    if (!expect_true("invalid height rejected",
                     runtime_input_click_policy_normalize(10.f, 10.f, 100, 0, &nx, &ny) == 0))
        return 1;

    if (!expect_true("null out x rejected",
                     runtime_input_click_policy_normalize(10.f, 10.f, 100, 100, NULL, &ny) == 0))
        return 1;

    if (!expect_true("null out y rejected",
                     runtime_input_click_policy_normalize(10.f, 10.f, 100, 100, &nx, NULL) == 0))
        return 1;

    if (!expect_true("top left normalizes",
                     runtime_input_click_policy_normalize(0.f, 0.f, 100, 200, &nx, &ny) == 1))
        return 1;
    if (!expect_true("top left x", nearly_equal(nx, -1.f)))
        return 1;
    if (!expect_true("top left y", nearly_equal(ny, 1.f)))
        return 1;

    if (!expect_true("center normalizes",
                     runtime_input_click_policy_normalize(50.f, 100.f, 100, 200, &nx, &ny) == 1))
        return 1;
    if (!expect_true("center x", nearly_equal(nx, 0.f)))
        return 1;
    if (!expect_true("center y", nearly_equal(ny, 0.f)))
        return 1;

    if (!expect_true("bottom right normalizes",
                     runtime_input_click_policy_normalize(100.f, 200.f, 100, 200, &nx, &ny) == 1))
        return 1;
    if (!expect_true("bottom right x", nearly_equal(nx, 1.f)))
        return 1;
    if (!expect_true("bottom right y", nearly_equal(ny, -1.f)))
        return 1;

    return 0;
}
