#include "runtime/tick/tick_post_phase.h"

#include "../support/test_support.h"

#include <stdint.h>

static int g_follow_calls = 0;
static int g_render_calls = 0;
static void *g_last_follow_context = NULL;
static void *g_last_render_context = NULL;

static void fake_follow_camera(void *context)
{
    g_follow_calls += 1;
    g_last_follow_context = context;
}

static void fake_render_scene(void *context)
{
    g_render_calls += 1;
    g_last_render_context = context;
}

static void test_tick_post_phase_calls_optional_callbacks(void **state)
{
    (void)state;
    void *context = (void *)(uintptr_t)0xCAFE;

    g_follow_calls = 0;
    g_render_calls = 0;
    g_last_follow_context = NULL;
    g_last_render_context = NULL;

    runtime_tick_post_phase_execute(NULL,
                                    NULL,
                                    0,
                                    0.016f,
                                    context,
                                    fake_follow_camera,
                                    fake_render_scene);

    BANANA_TEST_ASSERT_INT_EQ(g_follow_calls,
                              1,
                              "post-phase must invoke follow-camera callback when provided");
    BANANA_TEST_ASSERT_INT_EQ(g_render_calls,
                              1,
                              "post-phase must invoke render-scene callback when provided");
    BANANA_TEST_ASSERT_TRUE(g_last_follow_context == context,
                            "post-phase must forward context to follow-camera callback");
    BANANA_TEST_ASSERT_TRUE(g_last_render_context == context,
                            "post-phase must forward context to render-scene callback");
}

static void test_tick_post_phase_accepts_null_callbacks(void **state)
{
    (void)state;

    runtime_tick_post_phase_execute(NULL,
                                    NULL,
                                    0,
                                    0.016f,
                                    NULL,
                                    NULL,
                                    NULL);

    BANANA_TEST_ASSERT_TRUE(1,
                            "post-phase must tolerate missing optional callbacks");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_tick_post_phase_calls_optional_callbacks),
    BANANA_TEST_CASE(test_tick_post_phase_accepts_null_callbacks)
)
