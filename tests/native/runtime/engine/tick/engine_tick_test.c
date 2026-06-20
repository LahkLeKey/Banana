#include "runtime/engine/tick/engine_tick.h"

#include "../../support/test_support.h"

static void test_engine_tick_execute_rejects_null_window(void **state)
{
    (void)state;
    int viewport_width = 0;
    int viewport_height = 0;
    int rc = runtime_engine_tick_execute(NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         &viewport_width,
                                         &viewport_height,
                                         NULL,
                                         NULL,
                                         0,
                                         0.016f,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL);

    BANANA_TEST_ASSERT_INT_EQ(rc,
                              -1,
                              "engine tick must return -1 when window context is missing");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_engine_tick_execute_rejects_null_window)
)
