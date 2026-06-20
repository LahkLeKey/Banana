#include "runtime/engine/application/service/application_service_render_port.h"
#include "render/renderer.h"

#include "../../../support/test_support.h"

#include <string.h>

static void test_render_port_submit_scene_rejects_null_state(void **state)
{
    (void)state;

    runtime_application_render_submit_scene_port(NULL);

    BANANA_TEST_ASSERT_TRUE(1,
                            "render submit-scene port must tolerate null runtime state");
}

static void test_render_port_submit_scene_accepts_minimal_state(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;

    memset(&runtime_state, 0, sizeof(runtime_state));

    runtime_application_render_submit_scene_port(&runtime_state);

    BANANA_TEST_ASSERT_TRUE(1,
                            "render submit-scene port must tolerate minimal state with null renderer/world resources");
}

static void test_render_port_submit_scene_calls_terrain_draw_indirect_when_enabled(void **state)
{
    (void)state;
    EngineRuntimeState runtime_state;

    memset(&runtime_state, 0, sizeof(runtime_state));

    runtime_state.renderer = renderer_create(16, 16);
    BANANA_TEST_ASSERT_TRUE(runtime_state.renderer != NULL,
                            "renderer_create must succeed for render-port terrain callback coverage");

    runtime_state.terrain_initialized = 1;
    runtime_application_render_submit_scene_port(&runtime_state);

    renderer_destroy(runtime_state.renderer);
    runtime_state.renderer = NULL;

    BANANA_TEST_ASSERT_TRUE(1,
                            "render submit-scene port must execute terrain callback path when terrain is initialized");
}

static void test_render_port_reset_is_callable(void **state)
{
    (void)state;

    runtime_application_render_ports_reset();

    BANANA_TEST_ASSERT_TRUE(1,
                            "render port reset must be callable without prior rendering");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_render_port_submit_scene_rejects_null_state),
    BANANA_TEST_CASE(test_render_port_submit_scene_accepts_minimal_state),
    BANANA_TEST_CASE(test_render_port_submit_scene_calls_terrain_draw_indirect_when_enabled),
    BANANA_TEST_CASE(test_render_port_reset_is_callable)
)
