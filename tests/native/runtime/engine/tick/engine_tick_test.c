#include "runtime/engine/tick/engine_tick.h"
#include "physics/dynamics.h"

#include "../../support/test_support.h"

#include <string.h>

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

static void test_dynamics_guard_paths_for_engine_tick_module(void **state)
{
    PhysicsBody body;

    (void)state;
    memset(&body, 0, sizeof(body));

    body.is_static = 1;
    body.mass = 1.0f;
    body.force_accum[1] = 6.0f;

    dynamics_set_gravity(-12.0f);
    dynamics_integrate(&body, 0.033f);

    BANANA_TEST_ASSERT_FLOAT_EQ(body.force_accum[1],
                                6.0f,
                                0.0001f,
                                "dynamics integrate must early-return for static-body guard path");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_engine_tick_execute_rejects_null_window),
    BANANA_TEST_CASE(test_dynamics_guard_paths_for_engine_tick_module)
)
