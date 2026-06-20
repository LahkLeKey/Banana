#include "engine.h"

#include "../support/test_support.h"

static void test_engine_init_fails_when_service_ports_unavailable(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(engine_init(1280, 720),
                              -1,
                              "engine_init must fail when service ports are not available in this minimal harness");
}

static void test_engine_tick_returns_error_without_initialized_window(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(engine_tick(0.016f),
                              -1,
                              "engine_tick must fail when the runtime window has not been initialized");
}

static void test_engine_shutdown_tolerates_uninitialized_runtime(void **state)
{
    (void)state;

    engine_shutdown();

    BANANA_TEST_ASSERT_TRUE(1,
                            "engine_shutdown must tolerate uninitialized runtime state");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_engine_init_fails_when_service_ports_unavailable),
    BANANA_TEST_CASE(test_engine_tick_returns_error_without_initialized_window),
    BANANA_TEST_CASE(test_engine_shutdown_tolerates_uninitialized_runtime)
)
