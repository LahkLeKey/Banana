#include "runtime/engine/composition/engine_composition_shutdown.h"
#include "ai/controller.h"
#include "ai/npc_merchant.h"
#include "physics/dynamics.h"
#include "render/backend.h"

#include "runtime/support/test_support.h"

#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
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

static int g_shutdown_destroy_calls = 0;

static void fake_controller_destroy(ControllerInstance *self)
{
    (void)self;
    g_shutdown_destroy_calls++;
}

static void test_engine_composition_shutdown_rejects_null_state(void **state)
{
    (void)state;

    runtime_engine_composition_shutdown_state(NULL);

    BANANA_TEST_ASSERT_TRUE(1,
                            "engine composition shutdown must tolerate null state");
}

static void test_engine_composition_shutdown_cleans_controllers_and_merchants(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;
    ControllerInstance *controller = (ControllerInstance *)calloc(1, sizeof(ControllerInstance));
    int merchant_id = -1;

    BANANA_TEST_ASSERT_TRUE(controller != NULL,
                            "test must allocate a heap controller for lifecycle destroy coverage");

    memset(&runtime_state, 0, sizeof(runtime_state));
    controller->destroy = fake_controller_destroy;

    runtime_state.controllers[0] = controller;
    runtime_state.controller_count = 1;
    runtime_state.merchants_seeded = 1;

    BANANA_TEST_ASSERT_INT_EQ(npc_merchant_init(), 0,
                              "merchant registry init must succeed for shutdown branch coverage");
    merchant_id = npc_merchant_register(7, 4.0f, 5.0f, 100);
    BANANA_TEST_ASSERT_TRUE(merchant_id >= 0,
                            "merchant registration must succeed before shutdown");
    BANANA_TEST_ASSERT_INT_EQ(npc_merchant_count(), 1,
                              "merchant registry should contain a seeded merchant before shutdown");

    g_shutdown_destroy_calls = 0;
    runtime_engine_composition_shutdown_state(&runtime_state);

    BANANA_TEST_ASSERT_INT_EQ(g_shutdown_destroy_calls, 1,
                              "shutdown must destroy active controllers");
    BANANA_TEST_ASSERT_TRUE(runtime_state.controllers[0] == NULL,
                            "shutdown must clear controller slots after destruction");
    BANANA_TEST_ASSERT_INT_EQ(runtime_state.controller_count, 0,
                              "shutdown must reset controller count");
    BANANA_TEST_ASSERT_INT_EQ(npc_merchant_count(), 0,
                              "shutdown must clear seeded merchants when merchant seeding is enabled");
}

static void test_dynamics_guard_paths_for_shutdown_module(void **state)
{
    PhysicsBody body;

    (void)state;
    memset(&body, 0, sizeof(body));

    body.is_static = 1;
    body.mass = 1.0f;
    body.force_accum[1] = 5.0f;

    dynamics_set_gravity(-9.81f);
    dynamics_integrate(&body, 0.016f);

    BANANA_TEST_ASSERT_FLOAT_EQ(body.force_accum[1],
                                5.0f,
                                0.0001f,
                                "dynamics integrate must early-return for static bodies");
}

static void test_backend_fallback_and_name_paths_for_shutdown_module(void **state)
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
    BANANA_TEST_CASE(test_engine_composition_shutdown_rejects_null_state),
    BANANA_TEST_CASE(test_engine_composition_shutdown_cleans_controllers_and_merchants),
    BANANA_TEST_CASE(test_dynamics_guard_paths_for_shutdown_module),
    BANANA_TEST_CASE(test_backend_fallback_and_name_paths_for_shutdown_module)
)
