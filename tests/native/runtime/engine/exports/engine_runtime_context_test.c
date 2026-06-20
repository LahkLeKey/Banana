#include "runtime/engine/exports/engine_runtime_context.h"

#include "../../support/test_support.h"

#include <stdint.h>

static void test_engine_runtime_aux_context_uses_global_state(void **state)
{
    (void)state;
    World *expected_world = (World *)(uintptr_t)0x1000;
    ControllerInstance *expected_controller = (ControllerInstance *)(uintptr_t)0x2000;
    int expected_controller_count = 37;
    RuntimeEngineAuxContext context;

    s_engine_state.world = expected_world;
    s_engine_state.controllers[0] = expected_controller;
    s_engine_state.controller_count = expected_controller_count;

    context = runtime_engine_exports_aux_context();

    BANANA_TEST_ASSERT_TRUE(context.world == expected_world,
                            "aux context must expose the current runtime world pointer");
    BANANA_TEST_ASSERT_TRUE(context.controllers == s_engine_state.controllers,
                            "aux context must expose the current controller array pointer");
    BANANA_TEST_ASSERT_TRUE(context.controllers[0] == expected_controller,
                            "aux context must expose controller entries from runtime state");
    BANANA_TEST_ASSERT_INT_EQ(context.controller_count,
                              expected_controller_count,
                              "aux context must expose the current controller count");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_engine_runtime_aux_context_uses_global_state)
)
