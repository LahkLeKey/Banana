#include "runtime/orchestration/tick/loop/runtime_tick_orchestration.h"
#include "runtime/tick/tick_budget_policy.h"
#include "runtime/tick/tick_input_phase.h"
#include "runtime/tick/tick_phases.h"
#include "runtime/tick/tick_post_phase.h"
#include "../../../support/test_support.h"

#include <stdint.h>

static int g_poll_calls = 0;
static int g_input_calls = 0;
static int g_physics_calls = 0;
static int g_world_calls = 0;
static int g_update_calls = 0;
static int g_resize_calls = 0;
static int g_budget_calls = 0;
static int g_gameplay_calls = 0;
static int g_post_calls = 0;
static int g_follow_calls = 0;
static int g_render_calls = 0;
static int g_window_open = 1;
static int g_budget_result = 0;
static EngineRuntimeState *g_last_update_context = NULL;
static void *g_last_input_context = NULL;
static void *g_last_budget_context = NULL;
static void *g_last_post_context = NULL;
static float g_last_update_dt = 0.0f;
static float g_last_physics_dt = 0.0f;
static float g_last_world_dt = 0.0f;
static float g_last_post_dt = 0.0f;
static int g_last_resize_width = 0;
static int g_last_resize_height = 0;
static int g_last_budget_max_chunks = 0;
static int g_last_controller_count = 0;
static int g_last_gameplay_context_seen = 0;

static int stub_rebuild_dirty_chunks(void *context, int max_chunks)
{
    (void)context;
    g_budget_calls += 1;
    g_last_budget_max_chunks = max_chunks;
    return 0;
}

int runtime_tick_budget_policy_terrain_chunks_per_tick(void)
{
    return 7;
}

static void reset_execute_state(void)
{
    g_poll_calls = 0;
    g_input_calls = 0;
    g_physics_calls = 0;
    g_world_calls = 0;
    g_update_calls = 0;
    g_resize_calls = 0;
    g_budget_calls = 0;
    g_gameplay_calls = 0;
    g_post_calls = 0;
    g_follow_calls = 0;
    g_render_calls = 0;
    g_window_open = 1;
    g_budget_result = 0;
    g_last_update_context = NULL;
    g_last_input_context = NULL;
    g_last_budget_context = NULL;
    g_last_post_context = NULL;
    g_last_update_dt = 0.0f;
    g_last_physics_dt = 0.0f;
    g_last_world_dt = 0.0f;
    g_last_post_dt = 0.0f;
    g_last_resize_width = 0;
    g_last_resize_height = 0;
    g_last_budget_max_chunks = 0;
    g_last_controller_count = 0;
    g_last_gameplay_context_seen = 0;
}

void window_poll_events(Window *window)
{
    (void)window;
    g_poll_calls += 1;
}

int window_is_open(Window *window)
{
    (void)window;
    return g_window_open;
}

void physics_world_step(PhysicsWorld *physics_world, float dt)
{
    (void)physics_world;
    g_physics_calls += 1;
    g_last_physics_dt = dt;
}

void world_tick(World *world, float dt)
{
    (void)world;
    g_world_calls += 1;
    g_last_world_dt = dt;
}

void runtime_tick_input_phase_process(Window *window,
                                      void *context,
                                      RuntimeTickInputClickFn apply_click_input)
{
    (void)window;
    (void)apply_click_input;
    g_input_calls += 1;
    g_last_input_context = context;
}

void runtime_phase_viewport_resize(Window *window,
                                   Renderer *renderer,
                                   int *viewport_width,
                                   int *viewport_height)
{
    (void)window;
    (void)renderer;
    g_resize_calls += 1;
    if (viewport_width)
        g_last_resize_width = *viewport_width;
    if (viewport_height)
        g_last_resize_height = *viewport_height;
}

int runtime_phase_terrain_budget(void *context,
                                 RuntimeTerrainRebuildFn rebuild_dirty_chunks,
                                 int max_chunks)
{
    (void)rebuild_dirty_chunks;
    g_budget_calls += 1;
    g_last_budget_context = context;
    g_last_budget_max_chunks = max_chunks;
    return g_budget_result;
}

void runtime_tick_post_phase_execute(World *world,
                                     ControllerInstance **controllers,
                                     int controller_count,
                                     float dt,
                                     void *context,
                                     RuntimeTickPostVoidFn follow_player_camera,
                                     RuntimeTickPostVoidFn render_scene)
{
    (void)world;
    (void)controllers;
    g_post_calls += 1;
    g_last_controller_count = controller_count;
    g_last_post_dt = dt;
    g_last_post_context = context;
    if (follow_player_camera)
    {
        g_follow_calls += 1;
        follow_player_camera(context);
    }
    if (render_scene)
    {
        g_render_calls += 1;
        render_scene(context);
    }
}

static void stub_update_player_motion(void *context, float dt)
{
    g_update_calls += 1;
    g_last_update_context = (EngineRuntimeState *)context;
    g_last_update_dt = dt;
}

static void stub_follow_player_camera(void *context)
{
    g_follow_calls += 1;
    g_last_post_context = context;
}

static void stub_apply_click_input(void *context, float normalized_x, float normalized_y)
{
    (void)normalized_x;
    (void)normalized_y;
    g_last_input_context = context;
}

static void stub_run_gameplay(void *context)
{
    g_gameplay_calls += 1;
    g_last_gameplay_context_seen = context != NULL;
}

static void stub_render_scene(void *context)
{
    g_render_calls += 1;
    g_last_post_context = context;
}

static void test_execute_rejects_null_window(void **state)
{
    (void)state;
    reset_execute_state();

    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_orchestration_execute(NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 0,
                                                                 0.016f,
                                                                 NULL,
                                                                 stub_update_player_motion,
                                                                 stub_follow_player_camera,
                                                                 stub_apply_click_input,
                                                                 stub_run_gameplay,
                                                                 stub_render_scene),
                              -1,
                              "execute must reject null window inputs");
}

static void test_execute_returns_closed_window_status(void **state)
{
    (void)state;
    Window *window = (Window *)(uintptr_t)0x1;

    reset_execute_state();
    g_window_open = 0;

    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_orchestration_execute(window,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 0,
                                                                 0.016f,
                                                                 NULL,
                                                                 stub_update_player_motion,
                                                                 stub_follow_player_camera,
                                                                 stub_apply_click_input,
                                                                 stub_run_gameplay,
                                                                 stub_render_scene),
                              1,
                              "execute must return the closed-window status before running the tick pipeline");
    BANANA_TEST_ASSERT_INT_EQ(g_poll_calls, 1,
                              "execute must poll events before checking the open state");
    BANANA_TEST_ASSERT_INT_EQ(g_input_calls, 0,
                              "closed windows must stop before input phase processing");
}

static void test_execute_runs_full_pipeline(void **state)
{
    (void)state;
    Window *window = (Window *)(uintptr_t)0x1;
    Renderer *renderer = (Renderer *)(uintptr_t)0x2;
    PhysicsWorld *physics_world = (PhysicsWorld *)(uintptr_t)0x3;
    World *world = (World *)(uintptr_t)0x4;
    int viewport_width = 1280;
    int viewport_height = 720;
    ControllerInstance *controllers[2] = {NULL, NULL};

    reset_execute_state();
    g_budget_result = 0;

    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_orchestration_execute(window,
                                                                 renderer,
                                                                 physics_world,
                                                                 world,
                                                                 &viewport_width,
                                                                 &viewport_height,
                                                                 stub_rebuild_dirty_chunks,
                                                                 controllers,
                                                                 2,
                                                                 0.033f,
                                                                 (void *)(uintptr_t)0xCAFE,
                                                                 stub_update_player_motion,
                                                                 stub_follow_player_camera,
                                                                 stub_apply_click_input,
                                                                 stub_run_gameplay,
                                                                 stub_render_scene),
                              0,
                              "execute must succeed when all phases succeed");
    BANANA_TEST_ASSERT_INT_EQ(g_poll_calls, 1, "execute must poll events once");
    BANANA_TEST_ASSERT_INT_EQ(g_input_calls, 1, "execute must run the input phase once");
    BANANA_TEST_ASSERT_INT_EQ(g_physics_calls, 1, "execute must advance physics once");
    BANANA_TEST_ASSERT_INT_EQ(g_world_calls, 1, "execute must tick the world once");
    BANANA_TEST_ASSERT_INT_EQ(g_update_calls, 1, "execute must update player motion once");
    BANANA_TEST_ASSERT_INT_EQ(g_resize_calls, 1, "execute must resize the viewport once");
    BANANA_TEST_ASSERT_INT_EQ(g_budget_calls, 1, "execute must run terrain budgeting once");
    BANANA_TEST_ASSERT_INT_EQ(g_gameplay_calls, 1, "execute must run gameplay once");
    BANANA_TEST_ASSERT_INT_EQ(g_post_calls, 1, "execute must run post phase once");
    BANANA_TEST_ASSERT_INT_EQ(g_follow_calls, 2,
                              "execute must invoke the follow callback via the post phase and its own stub");
    BANANA_TEST_ASSERT_INT_EQ(g_render_calls, 2,
                              "execute must invoke the render callback via the post phase and its own stub");
    BANANA_TEST_ASSERT_INT_EQ(g_last_controller_count, 2,
                              "execute must pass controller count to post phase");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_update_dt, 0.033f, 0.0001f,
                                "execute must forward dt to player motion update");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_physics_dt, 0.033f, 0.0001f,
                                "execute must forward dt to physics");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_world_dt, 0.033f, 0.0001f,
                                "execute must forward dt to world tick");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_post_dt, 0.033f, 0.0001f,
                                "execute must forward dt to post phase");
    BANANA_TEST_ASSERT_TRUE(g_last_update_context == (EngineRuntimeState *)(uintptr_t)0xCAFE,
                            "execute must forward context through update path");
    BANANA_TEST_ASSERT_TRUE(g_last_input_context == (void *)(uintptr_t)0xCAFE,
                            "execute must forward context through input and post paths");
}

static void test_execute_propagates_terrain_budget_failure(void **state)
{
    (void)state;
    Window *window = (Window *)(uintptr_t)0x1;
    int viewport_width = 640;
    int viewport_height = 360;

    reset_execute_state();
    g_budget_result = -1;

    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_orchestration_execute(window,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 &viewport_width,
                                                                 &viewport_height,
                                                                 stub_rebuild_dirty_chunks,
                                                                 NULL,
                                                                 0,
                                                                 0.016f,
                                                                 (void *)(uintptr_t)0xDEAD,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL,
                                                                 NULL),
                              -1,
                              "execute must propagate terrain-budget failures");
    BANANA_TEST_ASSERT_INT_EQ(g_budget_calls, 1,
                              "execute must evaluate terrain budget even when later phases are absent");
    BANANA_TEST_ASSERT_INT_EQ(g_post_calls, 0,
                              "execute must stop before the post phase on terrain-budget failure");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_execute_rejects_null_window),
    BANANA_TEST_CASE(test_execute_returns_closed_window_status),
    BANANA_TEST_CASE(test_execute_runs_full_pipeline),
    BANANA_TEST_CASE(test_execute_propagates_terrain_budget_failure)
)
