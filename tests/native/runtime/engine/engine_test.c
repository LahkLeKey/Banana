#include "engine.h"
#include "physics/dynamics.h"
#include "render/backend.h"

#include "runtime/support/test_support.h"

#include <stdlib.h>

#if defined(_WIN32)
#include <string.h>
static void set_env_value(const char *name, const char *value)
{
    if (!name || !value)
        return;
    _putenv_s(name, value);
}
#else
static void set_env_value(const char *name, const char *value)
{
    if (!name || !value)
        return;
    setenv(name, value, 1);
}
#endif

static void set_launch_gate_env_for_engine_init(void)
{
    set_env_value("BANANA_LAUNCH_GATE_MODE", "development");
    set_env_value("BANANA_LAUNCH_GATE_HAS_STEAM_IDENTITY", "0");
    set_env_value("BANANA_LAUNCH_GATE_ACCOUNT_LINKED", "0");
    set_env_value("BANANA_LAUNCH_GATE_ACCOUNT_IN_GOOD_STANDING", "1");
    set_env_value("BANANA_LAUNCH_GATE_VERIFICATION_FRESH", "0");
    set_env_value("BANANA_LAUNCH_GATE_VERIFICATION_AVAILABLE", "1");
}

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

static void test_engine_init_succeeds_with_development_launch_gate_inputs(void **state)
{
    int init_result = -1;

    (void)state;

    set_launch_gate_env_for_engine_init();
    init_result = engine_init(320, 180);

    BANANA_TEST_ASSERT_INT_EQ(init_result,
                              0,
                              "engine_init must succeed when launch gate inputs satisfy development policy");

    engine_shutdown();
}

static void test_dynamics_guard_paths_for_engine_module(void **state)
{
    PhysicsBody body;

    (void)state;
    memset(&body, 0, sizeof(body));

    body.is_static = 1;
    body.mass = 1.0f;
    body.force_accum[1] = 4.0f;

    dynamics_set_gravity(-1.25f);
    dynamics_integrate(&body, 0.016f);

    BANANA_TEST_ASSERT_FLOAT_EQ(body.force_accum[1],
                                4.0f,
                                0.0001f,
                                "dynamics integrate must early-return for static-body guard path");
}

static void test_backend_fallback_and_name_paths_for_engine_module(void **state)
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
    BANANA_TEST_CASE(test_engine_init_fails_when_service_ports_unavailable),
    BANANA_TEST_CASE(test_engine_tick_returns_error_without_initialized_window),
    BANANA_TEST_CASE(test_engine_shutdown_tolerates_uninitialized_runtime),
    BANANA_TEST_CASE(test_engine_init_succeeds_with_development_launch_gate_inputs),
    BANANA_TEST_CASE(test_dynamics_guard_paths_for_engine_module),
    BANANA_TEST_CASE(test_backend_fallback_and_name_paths_for_engine_module)
)
