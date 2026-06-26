#include "runtime/orchestration/tick/loop/runtime_tick_orchestration.h"
#include "runtime/support/test_support.h"

#include <string.h>

static int g_update_motion_calls = 0;
static int g_follow_camera_calls = 0;
static int g_apply_click_calls = 0;
static EngineRuntimeState *g_last_state = NULL;
static float g_last_dt = 0.0f;
static float g_last_click_x = 0.0f;
static float g_last_click_y = 0.0f;
static RuntimeTerrainSampleHeightFn g_last_sample_height = NULL;

static float stub_sample_height(float x, float z)
{
    return x + z;
}

static void stub_update_motion(EngineRuntimeState *state,
                               float dt,
                               RuntimeTerrainSampleHeightFn sample_height)
{
    g_update_motion_calls += 1;
    g_last_state = state;
    g_last_dt = dt;
    g_last_sample_height = sample_height;
}

static void stub_follow_camera(EngineRuntimeState *state)
{
    g_follow_camera_calls += 1;
    g_last_state = state;
}

static void stub_apply_click_input(EngineRuntimeState *state,
                                   float normalized_x,
                                   float normalized_y,
                                   RuntimeTerrainSampleHeightFn sample_height)
{
    g_apply_click_calls += 1;
    g_last_state = state;
    g_last_click_x = normalized_x;
    g_last_click_y = normalized_y;
    g_last_sample_height = sample_height;
}

static void reset_player_tick_probe(void)
{
    g_update_motion_calls = 0;
    g_follow_camera_calls = 0;
    g_apply_click_calls = 0;
    g_last_state = NULL;
    g_last_dt = 0.0f;
    g_last_click_x = 0.0f;
    g_last_click_y = 0.0f;
    g_last_sample_height = NULL;
}

static void test_player_tick_orchestration_guards(void **state)
{
    RuntimeTickOrchestrationContext orchestration;
    RuntimeApplicationServicePorts ports;
    EngineRuntimeState runtime_state;

    (void)state;

    memset(&ports, 0, sizeof(ports));
    memset(&runtime_state, 0, sizeof(runtime_state));
    memset(&orchestration, 0, sizeof(orchestration));

    reset_player_tick_probe();

    runtime_tick_orchestration_update_player_motion(NULL, 0.016f);
    runtime_tick_orchestration_follow_player_camera(NULL);
    runtime_tick_orchestration_apply_click_move_input(NULL, 0.25f, 0.5f);

    BANANA_TEST_ASSERT_INT_EQ(g_update_motion_calls, 0,
                              "update_player_motion must ignore null context");
    BANANA_TEST_ASSERT_INT_EQ(g_follow_camera_calls, 0,
                              "follow_player_camera must ignore null context");
    BANANA_TEST_ASSERT_INT_EQ(g_apply_click_calls, 0,
                              "apply_click_input must ignore null context");

    orchestration.state = &runtime_state;
    orchestration.sample_height = stub_sample_height;
    orchestration.service_ports = &ports;

    runtime_tick_orchestration_update_player_motion(&orchestration, 0.016f);
    runtime_tick_orchestration_follow_player_camera(&orchestration);
    runtime_tick_orchestration_apply_click_move_input(&orchestration, 0.25f, 0.5f);

    BANANA_TEST_ASSERT_INT_EQ(g_update_motion_calls, 0,
                              "update_player_motion must require player service callback");
    BANANA_TEST_ASSERT_INT_EQ(g_follow_camera_calls, 0,
                              "follow_player_camera must require player service callback");
    BANANA_TEST_ASSERT_INT_EQ(g_apply_click_calls, 0,
                              "apply_click_input must require player service callback");
}

static void test_player_tick_orchestration_dispatches_player_callbacks(void **state)
{
    RuntimeTickOrchestrationContext orchestration;
    RuntimeApplicationServicePorts ports;
    EngineRuntimeState runtime_state;

    (void)state;

    memset(&ports, 0, sizeof(ports));
    memset(&runtime_state, 0, sizeof(runtime_state));
    memset(&orchestration, 0, sizeof(orchestration));

    ports.player.update_motion = stub_update_motion;
    ports.player.follow_camera = stub_follow_camera;
    ports.player.apply_click_input = stub_apply_click_input;

    orchestration.state = &runtime_state;
    orchestration.sample_height = stub_sample_height;
    orchestration.service_ports = &ports;

    reset_player_tick_probe();

    runtime_tick_orchestration_update_player_motion(&orchestration, 0.033f);
    runtime_tick_orchestration_follow_player_camera(&orchestration);
    runtime_tick_orchestration_apply_click_move_input(&orchestration, -0.75f, 0.125f);

    BANANA_TEST_ASSERT_INT_EQ(g_update_motion_calls, 1,
                              "update_player_motion must dispatch once");
    BANANA_TEST_ASSERT_INT_EQ(g_follow_camera_calls, 1,
                              "follow_player_camera must dispatch once");
    BANANA_TEST_ASSERT_INT_EQ(g_apply_click_calls, 1,
                              "apply_click_input must dispatch once");

    BANANA_TEST_ASSERT_TRUE(g_last_state == &runtime_state,
                            "player tick wrappers must forward runtime state");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_dt, 0.033f, 0.0001f,
                                "update_player_motion must forward dt");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_click_x, -0.75f, 0.0001f,
                                "apply_click_input must forward normalized x");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_click_y, 0.125f, 0.0001f,
                                "apply_click_input must forward normalized y");
    BANANA_TEST_ASSERT_TRUE(g_last_sample_height == stub_sample_height,
                            "player tick wrappers must forward sample_height callback");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_player_tick_orchestration_guards),
    BANANA_TEST_CASE(test_player_tick_orchestration_dispatches_player_callbacks)
)
