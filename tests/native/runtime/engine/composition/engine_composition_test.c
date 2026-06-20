#include "runtime/engine/composition/engine_composition.h"
#include "runtime/engine/application/service/application_service_ports.h"

#include "../../support/test_support.h"

#include <string.h>

static int g_apply_click_calls = 0;
static float g_last_click_x = 0.0f;
static float g_last_click_y = 0.0f;

static float fake_sample_height(float x, float z)
{
    (void)x;
    (void)z;
    return 0.0f;
}

static uint32_t fake_attach_controller(uint32_t entity_id, const char *type_name)
{
    (void)entity_id;
    (void)type_name;
    return 0u;
}

static void fake_apply_click_input(EngineRuntimeState *state,
                                   float normalized_x,
                                   float normalized_y,
                                   RuntimeTerrainSampleHeightFn sample_height)
{
    (void)state;
    (void)sample_height;
    g_apply_click_calls++;
    g_last_click_x = normalized_x;
    g_last_click_y = normalized_y;
}

static void test_engine_composition_init_rejects_null_state(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_init(NULL,
                                                              1280,
                                                              720,
                                                              fake_sample_height,
                                                              fake_attach_controller),
                              -1,
                              "engine composition init must reject null runtime state");
}

static void test_engine_composition_tick_rejects_null_inputs(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;

    memset(&runtime_state, 0, sizeof(runtime_state));

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_tick(NULL, 0.016f, fake_sample_height),
                              -1,
                              "engine composition tick must reject null runtime state");
    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_tick(&runtime_state, 0.016f, NULL),
                              -1,
                              "engine composition tick must reject null terrain sample function");
}

static void test_engine_composition_tick_executes_tick_pipeline(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;
    RuntimeApplicationServicePorts ports;

    memset(&runtime_state, 0, sizeof(runtime_state));
    memset(&ports, 0, sizeof(ports));

    runtime_state.viewport_width = 800;
    runtime_state.viewport_height = 600;
    runtime_state.service_ports = &ports;

    BANANA_TEST_ASSERT_INT_EQ(runtime_engine_composition_tick(&runtime_state,
                                                              0.033f,
                                                              fake_sample_height),
                              -1,
                              "engine composition tick should surface tick execution failure when window is missing");
}

static void test_engine_composition_apply_click_input_forwards_when_port_exists(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;
    RuntimeApplicationServicePorts ports;

    memset(&runtime_state, 0, sizeof(runtime_state));
    memset(&ports, 0, sizeof(ports));

    ports.player.apply_click_input = fake_apply_click_input;
    runtime_state.service_ports = &ports;

    g_apply_click_calls = 0;
    g_last_click_x = 0.0f;
    g_last_click_y = 0.0f;

    runtime_engine_composition_apply_click_input(&runtime_state,
                                                 0.25f,
                                                 -0.75f,
                                                 fake_sample_height);

    BANANA_TEST_ASSERT_INT_EQ(g_apply_click_calls,
                              1,
                              "engine composition click-input path must call player service port callback");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_click_x,
                                0.25f,
                                0.0001f,
                                "engine composition click-input must forward normalized x coordinate");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_click_y,
                                -0.75f,
                                0.0001f,
                                "engine composition click-input must forward normalized y coordinate");
}

static void test_engine_composition_apply_click_input_rejects_missing_dependencies(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;

    memset(&runtime_state, 0, sizeof(runtime_state));

    g_apply_click_calls = 0;

    runtime_engine_composition_apply_click_input(NULL, 0.1f, 0.2f, fake_sample_height);
    runtime_engine_composition_apply_click_input(&runtime_state, 0.1f, 0.2f, fake_sample_height);
    runtime_engine_composition_apply_click_input(&runtime_state, 0.1f, 0.2f, NULL);

    BANANA_TEST_ASSERT_INT_EQ(g_apply_click_calls,
                              0,
                              "engine composition click-input path must no-op when state, sample, or ports are missing");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_engine_composition_init_rejects_null_state),
    BANANA_TEST_CASE(test_engine_composition_tick_rejects_null_inputs),
    BANANA_TEST_CASE(test_engine_composition_tick_executes_tick_pipeline),
    BANANA_TEST_CASE(test_engine_composition_apply_click_input_forwards_when_port_exists),
    BANANA_TEST_CASE(test_engine_composition_apply_click_input_rejects_missing_dependencies)
)
