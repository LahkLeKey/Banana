#include "runtime/input/contract/input_contract.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <math.h>

static void test_input_contract_helpers(void **state)
{
    (void)state;
    float move_x = 0.0f;
    float move_z = 0.0f;

    runtime_input_contract_reset();

    assert_int_equal(runtime_input_contract_handle_right_click(12.0f, 34.0f), 1);
    assert_int_equal(runtime_input_contract_get_click_count(), 1);
    assert_true(runtime_input_contract_get_has_move_target());

    runtime_input_contract_mark_target_reached();
    assert_int_equal(runtime_input_contract_get_target_reached_count(), 1);
    assert_false(runtime_input_contract_get_has_move_target());

    runtime_input_contract_reset();
    runtime_input_contract_handle_right_click_normalized(0.5f, -0.25f);
    assert_true(runtime_input_contract_get_has_move_target());

    assert_int_equal(runtime_input_contract_handle_right_click_normalized(2.0f, 0.0f), 0);

    runtime_input_contract_sanitize_move_input(NAN, -2.0f, &move_x, &move_z);
    assert_true(isfinite(move_x));
    assert_float_equal(move_x, 0.0f, 0.0001f);
    assert_float_equal(move_z, -1.0f, 0.0001f);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_input_contract_helpers),
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

static int test_input_contract_helpers(void)
{
    float move_x = 0.0f;
    float move_z = 0.0f;

    runtime_input_contract_reset();

    if (fail_if(runtime_input_contract_handle_right_click(12.0f, 34.0f) != 1,
                "handle_right_click must accept finite coordinates"))
        return 1;

    if (fail_if(runtime_input_contract_get_click_count() != 1,
                "click count must increment after a valid right click") ||
        fail_if(!runtime_input_contract_get_has_move_target(),
                "handle_right_click must mark move targeting active"))
        return 1;

    runtime_input_contract_mark_target_reached();
    if (fail_if(runtime_input_contract_get_target_reached_count() != 1,
                "mark_target_reached must increment the reached count") ||
        fail_if(runtime_input_contract_get_has_move_target(),
                "mark_target_reached must clear move targeting"))
        return 1;

    runtime_input_contract_reset();
    runtime_input_contract_handle_right_click_normalized(0.5f, -0.25f);
    if (fail_if(!runtime_input_contract_get_has_move_target(),
                "normalized right click must set move target state"))
        return 1;

    if (runtime_input_contract_handle_right_click_normalized(2.0f, 0.0f) != 0)
        return fail_if(1, "normalized right click must reject out-of-range values");

    runtime_input_contract_sanitize_move_input(NAN, -2.0f, &move_x, &move_z);
    if (fail_if(!isfinite(move_x) || move_x != 0.0f || move_z != -1.0f,
                "sanitize_move_input must clamp non-finite and out-of-range values"))
        return 1;

    return 0;
}

int main(void)
{
    return test_input_contract_helpers();
}
#endif
