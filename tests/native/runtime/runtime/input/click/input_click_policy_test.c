#include "runtime/input/click/input_click_policy.h"
#include "../../../support/test_support.h"
#include <math.h>

static void test_input_click_policy_normalize(void **state)
{
    (void)state;
    float out_x = 0.0f;
    float out_y = 0.0f;

    BANANA_TEST_ASSERT_TRUE(!runtime_input_click_policy_normalize(10.0f, 10.0f, 100, 50, NULL, &out_y),
                            "normalize must reject null normalized-x output pointers");
    BANANA_TEST_ASSERT_TRUE(!runtime_input_click_policy_normalize(10.0f, 10.0f, 100, 50, &out_x, NULL),
                            "normalize must reject null normalized-y output pointers");

    BANANA_TEST_ASSERT_TRUE(runtime_input_click_policy_normalize(0.0f, 0.0f, 100, 50, &out_x, &out_y),
                            "normalize must return success for valid dimensions");
    BANANA_TEST_ASSERT_TRUE(isfinite(out_x) && isfinite(out_y),
                            "normalize must produce finite clip-space values");
    BANANA_TEST_ASSERT_FLOAT_EQ(out_x, -1.0f, 0.0001f,
                                "normalize must map top-left x to -1");
    BANANA_TEST_ASSERT_FLOAT_EQ(out_y, 1.0f, 0.0001f,
                                "normalize must map top-left y to +1");

    BANANA_TEST_ASSERT_TRUE(runtime_input_click_policy_normalize(50.0f, 25.0f, 100, 50, &out_x, &out_y),
                            "normalize must return success for center coordinates");
    BANANA_TEST_ASSERT_FLOAT_EQ(out_x, 0.0f, 0.0001f,
                                "normalize must map center x to 0");
    BANANA_TEST_ASSERT_FLOAT_EQ(out_y, 0.0f, 0.0001f,
                                "normalize must map center y to 0");

    BANANA_TEST_ASSERT_TRUE(!runtime_input_click_policy_normalize(10.0f, 10.0f, 0, 50, &out_x, &out_y),
                            "normalize must reject invalid viewport dimensions");
    BANANA_TEST_ASSERT_TRUE(!runtime_input_click_policy_normalize(10.0f, 10.0f, 100, 0, &out_x, &out_y),
                            "normalize must reject invalid viewport height");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_input_click_policy_normalize)
)
