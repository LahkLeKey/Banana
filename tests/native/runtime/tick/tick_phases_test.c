#include "runtime/tick/tick_phases.h"

#include "../support/test_support.h"

#include <string.h>

static int g_window_width = 0;
static int g_window_height = 0;
static int g_resize_calls = 0;
static int g_last_resize_width = 0;
static int g_last_resize_height = 0;

void window_get_size(Window *window, int *width, int *height)
{
    (void)window;
    *width = g_window_width;
    *height = g_window_height;
}

void renderer_resize(Renderer *renderer, int width, int height)
{
    (void)renderer;
    g_resize_calls += 1;
    g_last_resize_width = width;
    g_last_resize_height = height;
}

static int fake_rebuild_success(void *context, int max_chunks)
{
    (void)context;
    (void)max_chunks;
    return 1;
}

static int fake_rebuild_failure(void *context, int max_chunks)
{
    (void)context;
    (void)max_chunks;
    return 0;
}

static void reset_state(void)
{
    g_window_width = 0;
    g_window_height = 0;
    g_resize_calls = 0;
    g_last_resize_width = 0;
    g_last_resize_height = 0;
}

static void test_viewport_resize_validates_inputs_and_noop_cases(void **state)
{
    Window *window = (Window *)0x1;
    Renderer *renderer = (Renderer *)0x1;
    int viewport_width = 1280;
    int viewport_height = 720;

    (void)state;
    reset_state();

    runtime_phase_viewport_resize(NULL, renderer, &viewport_width, &viewport_height);
    runtime_phase_viewport_resize(window, NULL, &viewport_width, &viewport_height);
    runtime_phase_viewport_resize(window, renderer, NULL, &viewport_width);
    runtime_phase_viewport_resize(window, renderer, &viewport_width, NULL);

    BANANA_TEST_ASSERT_INT_EQ(g_resize_calls, 0, "invalid resize inputs should perform no resize");

    g_window_width = 1280;
    g_window_height = 720;
    runtime_phase_viewport_resize(window, renderer, &viewport_width, &viewport_height);
    BANANA_TEST_ASSERT_INT_EQ(g_resize_calls,
                              0,
                              "same viewport dimensions should not trigger renderer resize");

    g_window_width = -1;
    g_window_height = 600;
    runtime_phase_viewport_resize(window, renderer, &viewport_width, &viewport_height);
    BANANA_TEST_ASSERT_INT_EQ(g_resize_calls,
                              0,
                              "non-positive window dimensions should not trigger resize");
}

static void test_viewport_resize_and_terrain_budget_paths(void **state)
{
    Window *window = (Window *)0x1;
    Renderer *renderer = (Renderer *)0x1;
    int viewport_width = 1280;
    int viewport_height = 720;

    (void)state;
    reset_state();

    g_window_width = 1920;
    g_window_height = 1080;
    runtime_phase_viewport_resize(window, renderer, &viewport_width, &viewport_height);

    BANANA_TEST_ASSERT_INT_EQ(g_resize_calls, 1, "changed viewport should trigger one resize");
    BANANA_TEST_ASSERT_INT_EQ(viewport_width, 1920, "viewport width should update");
    BANANA_TEST_ASSERT_INT_EQ(viewport_height, 1080, "viewport height should update");
    BANANA_TEST_ASSERT_INT_EQ(g_last_resize_width, 1920, "renderer resize width should match window");
    BANANA_TEST_ASSERT_INT_EQ(g_last_resize_height, 1080, "renderer resize height should match window");

    BANANA_TEST_ASSERT_INT_EQ(runtime_phase_terrain_budget(NULL, NULL, 4),
                              -1,
                              "missing terrain rebuild callback should fail");
    BANANA_TEST_ASSERT_INT_EQ(runtime_phase_terrain_budget(NULL, fake_rebuild_failure, 4),
                              -1,
                              "falsy rebuild result should fail budget phase");
    BANANA_TEST_ASSERT_INT_EQ(runtime_phase_terrain_budget(NULL, fake_rebuild_success, 4),
                              0,
                              "truthy rebuild result should pass budget phase");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_viewport_resize_validates_inputs_and_noop_cases),
    BANANA_TEST_CASE(test_viewport_resize_and_terrain_budget_paths)
)
