#include "runtime/engine/exports/engine_runtime_context.h"
#include "physics/dynamics.h"
#include "render/backend.h"

#include "runtime/support/test_support.h"

#include <stdint.h>
#include <string.h>

#if defined(_WIN32)
static void set_env_value(const char *name, const char *value)
{
    if (!name || !value)
        return;
    _putenv_s(name, value);
}
#else
#include <stdlib.h>
static void set_env_value(const char *name, const char *value)
{
    if (!name || !value)
        return;
    setenv(name, value, 1);
}
#endif

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

static void test_dynamics_guard_paths_for_runtime_context_module(void **state)
{
    PhysicsBody body;

    (void)state;
    memset(&body, 0, sizeof(body));

    body.is_static = 1;
    body.mass = 1.0f;
    body.force_accum[1] = 2.0f;

    dynamics_set_gravity(-7.0f);
    dynamics_integrate(&body, 0.01f);

    BANANA_TEST_ASSERT_FLOAT_EQ(body.force_accum[1],
                                2.0f,
                                0.0001f,
                                "dynamics integrate must early-return for static-body guard path");
}

static void test_backend_fallback_and_name_paths_for_runtime_context_module(void **state)
{
    BananaRenderBackend active_backend;

    (void)state;

    set_env_value("BANANA_DX12_FORCE_UNAVAILABLE", "1");
    active_backend = banana_render_backend_active();

    BANANA_TEST_ASSERT_INT_EQ((int)active_backend,
                              (int)BANANA_RENDER_BACKEND_HEADLESS,
                              "backend active path must fall back to headless when DX12 availability is forced off");
    BANANA_TEST_ASSERT_STR_EQ(banana_render_backend_name(BANANA_RENDER_BACKEND_GLFW),
                              "glfw",
                              "backend name must map GLFW enum");
    BANANA_TEST_ASSERT_STR_EQ(banana_render_backend_name(BANANA_RENDER_BACKEND_HEADLESS),
                              "headless",
                              "backend name must map headless enum");
    BANANA_TEST_ASSERT_STR_EQ(banana_render_backend_name((BananaRenderBackend)99),
                              "unknown",
                              "backend name must map unknown enums to unknown string");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_engine_runtime_aux_context_uses_global_state),
    BANANA_TEST_CASE(test_dynamics_guard_paths_for_runtime_context_module),
    BANANA_TEST_CASE(test_backend_fallback_and_name_paths_for_runtime_context_module)
)
