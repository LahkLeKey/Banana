#include "runtime/input/click/input_click_policy.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <math.h>

static void test_input_click_policy_normalize(void **state)
{
    (void)state;
    float out_x = 0.0f;
    float out_y = 0.0f;

    assert_true(runtime_input_click_policy_normalize(0.0f, 0.0f, 100, 50, &out_x, &out_y));
    assert_true(isfinite(out_x));
    assert_true(isfinite(out_y));
    assert_float_equal(out_x, -1.0f, 0.0001f);
    assert_float_equal(out_y, 1.0f, 0.0001f);

    assert_false(runtime_input_click_policy_normalize(10.0f, 10.0f, 0, 50, &out_x, &out_y));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_input_click_policy_normalize),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#else
#include <math.h>
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

static int test_input_click_policy_normalize(void)
{
    float out_x = 0.0f;
    float out_y = 0.0f;

    if (fail_if(!runtime_input_click_policy_normalize(0.0f, 0.0f, 100, 50, &out_x, &out_y),
                "normalize must return success for valid dimensions") ||
        fail_if(!isfinite(out_x) || !isfinite(out_y),
                "normalize must write finite normalized values"))
        return 1;

    if (fail_if(out_x != -1.0f || out_y != 1.0f,
                "normalize must map the top-left corner to the expected clip-space values"))
        return 1;

    if (fail_if(runtime_input_click_policy_normalize(10.0f, 10.0f, 0, 50, &out_x, &out_y),
                "normalize must reject invalid viewport dimensions"))
        return 1;

    return 0;
}

int main(void)
{
    return test_input_click_policy_normalize();
}
#endif
