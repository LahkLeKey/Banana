#include "runtime/input/contract/input_contract.h"
#include "runtime/support/test_support.h"
#include <math.h>

static void test_input_contract_helpers(void **state)
{
    (void)state;
    float move_x = 0.0f;
    float move_z = 0.0f;

    runtime_input_contract_reset();

    BANANA_TEST_ASSERT_INT_EQ(runtime_input_contract_handle_right_click(12.0f, 34.0f), 1,
                              "handle_right_click must accept finite coordinates");
    BANANA_TEST_ASSERT_INT_EQ(runtime_input_contract_get_click_count(), 1,
                              "click count must increment after valid right click");
    BANANA_TEST_ASSERT_TRUE(runtime_input_contract_get_has_move_target(),
                            "handle_right_click must mark move targeting active");

    runtime_input_contract_mark_target_reached();
    BANANA_TEST_ASSERT_INT_EQ(runtime_input_contract_get_target_reached_count(), 1,
                              "mark_target_reached must increment reached count");
    BANANA_TEST_ASSERT_TRUE(!runtime_input_contract_get_has_move_target(),
                            "mark_target_reached must clear move targeting");

    runtime_input_contract_reset();
    runtime_input_contract_handle_right_click_normalized(0.5f, -0.25f);
    BANANA_TEST_ASSERT_TRUE(runtime_input_contract_get_has_move_target(),
                            "normalized right click must set move target state");

    BANANA_TEST_ASSERT_INT_EQ(runtime_input_contract_handle_right_click_normalized(2.0f, 0.0f), 0,
                              "normalized right click must reject out-of-range values");

    runtime_input_contract_sanitize_move_input(NAN, -2.0f, &move_x, &move_z);
    BANANA_TEST_ASSERT_TRUE(isfinite(move_x),
                            "sanitize_move_input must produce finite x value");
    BANANA_TEST_ASSERT_FLOAT_EQ(move_x, 0.0f, 0.0001f,
                                "sanitize_move_input must clamp non-finite x");
    BANANA_TEST_ASSERT_FLOAT_EQ(move_z, -1.0f, 0.0001f,
                                "sanitize_move_input must clamp z to valid range");
}

static void test_input_contract_rejects_invalid_and_null_paths(void **state)
{
    (void)state;
    float move_x = 7.0f;
    float move_z = -7.0f;

    runtime_input_contract_reset();

    BANANA_TEST_ASSERT_INT_EQ(runtime_input_contract_handle_right_click(NAN, 12.0f), 0,
                              "handle_right_click must reject non-finite x coordinates");
    BANANA_TEST_ASSERT_INT_EQ(runtime_input_contract_handle_right_click(12.0f, INFINITY), 0,
                              "handle_right_click must reject non-finite y coordinates");
    BANANA_TEST_ASSERT_INT_EQ(runtime_input_contract_handle_right_click_normalized(-2.0f, 0.0f), 0,
                              "normalized right click must reject values below the legal range");
    BANANA_TEST_ASSERT_INT_EQ(runtime_input_contract_handle_right_click_normalized(0.0f, NAN), 0,
                              "normalized right click must reject non-finite values");

    runtime_input_contract_set_has_move_target(3);
    BANANA_TEST_ASSERT_TRUE(runtime_input_contract_get_has_move_target(),
                            "set_has_move_target must normalize truthy values to active");

    runtime_input_contract_sanitize_move_input(2.5f, 2.5f, &move_x, &move_z);
    BANANA_TEST_ASSERT_FLOAT_EQ(move_x, 1.0f, 0.0001f,
                                "sanitize_move_input must clamp positive x to the upper bound");
    BANANA_TEST_ASSERT_FLOAT_EQ(move_z, 1.0f, 0.0001f,
                                "sanitize_move_input must clamp positive z to the upper bound");

    runtime_input_contract_sanitize_move_input(-0.5f, NAN, &move_x, &move_z);
    BANANA_TEST_ASSERT_FLOAT_EQ(move_x, -0.5f, 0.0001f,
                                "sanitize_move_input must preserve finite x values");
    BANANA_TEST_ASSERT_FLOAT_EQ(move_z, 0.0f, 0.0001f,
                                "sanitize_move_input must replace non-finite z with zero");

    move_x = 7.0f;
    move_z = -7.0f;
    runtime_input_contract_sanitize_move_input(0.5f, 0.25f, NULL, &move_z);
    BANANA_TEST_ASSERT_FLOAT_EQ(move_z, -7.0f, 0.0001f,
                                "sanitize_move_input must no-op when x output is missing");

    runtime_input_contract_sanitize_move_input(0.5f, 0.25f, &move_x, NULL);
    BANANA_TEST_ASSERT_FLOAT_EQ(move_x, 7.0f, 0.0001f,
                                "sanitize_move_input must no-op when z output is missing");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_input_contract_helpers)
    ,BANANA_TEST_CASE(test_input_contract_rejects_invalid_and_null_paths)
)
