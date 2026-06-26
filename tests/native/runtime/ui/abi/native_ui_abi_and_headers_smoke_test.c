#include "include/banana_native_ui_abi.h"
#include "k3h4/infrastructure/k3h4_metrics_infrastructure_adapter.h"
#include "physics/world_collision.h"
#include "runtime/camera/basis/camera_basis.h"
#include "runtime/engine/application/service/application_service_render_model_cache.h"
#include "runtime/engine/application/service/application_service_render_port.h"
#include "runtime/engine/application/service/application_service_ports.h"
#include "runtime/engine/composition/engine_composition_shutdown.h"
#include "runtime/engine/engine_aux_abi.h"
#include "runtime/merchant/trade/merchant_trade_domain.h"
#include "runtime/parallel/model/parallel_model.h"
#include "runtime/render/material/render_material.h"
#include "runtime/world/world_abi.h"
#include "tools/application/banana_asset_generation_service.h"
#include "tools/infrastructure/banana_asset_json_writer.h"
#include "engine_serialize.h"

#include "runtime/support/test_support.h"

#include <string.h>

static void test_native_ui_frame_reset_and_write_cover_all_string_paths(void **state)
{
    banana_native_ui_frame frame;
    banana_native_ui_viewport viewport;

    (void)state;

    memset(&frame, 1, sizeof(frame));

    viewport.css_width = 1280;
    viewport.css_height = 720;
    viewport.pixel_width = 2560;
    viewport.pixel_height = 1440;
    viewport.device_pixel_ratio = 2.0f;

    banana_native_ui_frame_reset(&frame);
    BANANA_TEST_ASSERT_INT_EQ(frame.host, 0, "frame reset should clear host");
    BANANA_TEST_ASSERT_INT_EQ(frame.surface, 0, "frame reset should clear surface");

    banana_native_ui_frame_write(&frame,
                                 BANANA_NATIVE_UI_HOST_ELECTRON,
                                 BANANA_NATIVE_UI_SURFACE_GAME_ENGINE,
                                 BANANA_NATIVE_UI_STATUS_RUNNING,
                                 "renderer online",
                                 BANANA_NATIVE_UI_MOVEMENT_KEYBOARD,
                                 0.25f,
                                 -0.5f,
                                 viewport,
                                 "interact",
                                 987654321);

    BANANA_TEST_ASSERT_INT_EQ(frame.host,
                              (int)BANANA_NATIVE_UI_HOST_ELECTRON,
                              "frame write should store host");
    BANANA_TEST_ASSERT_STR_EQ(frame.error,
                              "renderer online",
                              "frame write should store non-empty error text");
    BANANA_TEST_ASSERT_STR_EQ(frame.interaction_message,
                              "interact",
                              "frame write should store non-empty interaction message");

    banana_native_ui_frame_write(&frame,
                                 BANANA_NATIVE_UI_HOST_WEB,
                                 BANANA_NATIVE_UI_SURFACE_GAME_ENGINE,
                                 BANANA_NATIVE_UI_STATUS_IDLE,
                                 "",
                                 BANANA_NATIVE_UI_MOVEMENT_NONE,
                                 0.0f,
                                 0.0f,
                                 viewport,
                                 "",
                                 0);

    BANANA_TEST_ASSERT_STR_EQ(frame.error,
                              "",
                              "empty error input should clear error field");
    BANANA_TEST_ASSERT_STR_EQ(frame.interaction_message,
                              "",
                              "empty interaction input should clear interaction field");

    banana_native_ui_frame_write(NULL,
                                 BANANA_NATIVE_UI_HOST_WEB,
                                 BANANA_NATIVE_UI_SURFACE_GAME_ENGINE,
                                 BANANA_NATIVE_UI_STATUS_IDLE,
                                 NULL,
                                 BANANA_NATIVE_UI_MOVEMENT_NONE,
                                 0.0f,
                                 0.0f,
                                 viewport,
                                 NULL,
                                 0);
    banana_native_ui_frame_reset(NULL);
}

static void test_header_surfaces_compile_and_expose_declared_types(void **state)
{
    RuntimeEngineAuxContext aux_context;
    RuntimeApplicationServicePorts service_ports;
    RuntimeTerrainServicePort terrain_port;
    RuntimePlayerServicePort player_port;
    RuntimeMerchantServicePort merchant_port;
    RuntimeRenderServicePort render_port;
    banana_compiler_config_t compiler_config;
    banana_terrain_cell_t cells[1];
    banana_native_ui_viewport viewport;

    (void)state;

    memset(&aux_context, 0, sizeof(aux_context));
    memset(&service_ports, 0, sizeof(service_ports));
    memset(&terrain_port, 0, sizeof(terrain_port));
    memset(&player_port, 0, sizeof(player_port));
    memset(&merchant_port, 0, sizeof(merchant_port));
    memset(&render_port, 0, sizeof(render_port));
    memset(&compiler_config, 0, sizeof(compiler_config));
    memset(cells, 0, sizeof(cells));
    memset(&viewport, 0, sizeof(viewport));

    BANANA_TEST_ASSERT_INT_EQ((int)BANANA_NATIVE_UI_HOST_WEB,
                              0,
                              "UI host enum should keep expected baseline value");
    BANANA_TEST_ASSERT_INT_EQ((int)BANANA_NATIVE_UI_MOVEMENT_RADIAL,
                              2,
                              "UI movement enum should keep expected radial value");
    BANANA_TEST_ASSERT_TRUE(aux_context.world == NULL,
                            "aux context should be zero-initialized in smoke test");
    BANANA_TEST_ASSERT_TRUE(service_ports.render.submit_scene == NULL,
                            "service-port render callback should be nullable");
    BANANA_TEST_ASSERT_TRUE(terrain_port.draw == NULL,
                            "terrain service-port callback should be nullable");
    BANANA_TEST_ASSERT_TRUE(player_port.follow_camera == NULL,
                            "player service-port callback should be nullable");
    BANANA_TEST_ASSERT_TRUE(merchant_port.trade_buy == NULL,
                            "merchant service-port callback should be nullable");
    BANANA_TEST_ASSERT_TRUE(render_port.submit_scene == NULL,
                            "render service-port callback should be nullable");
    BANANA_TEST_ASSERT_INT_EQ((int)viewport.css_width,
                              0,
                              "viewport should be zero-initialized in smoke test");
    BANANA_TEST_ASSERT_INT_EQ((int)cells[0].elevation,
                              0,
                              "terrain cell should be zero-initialized in smoke test");
    BANANA_TEST_ASSERT_INT_EQ((int)compiler_config.width,
                              0,
                              "asset compiler config should be zero-initialized in smoke test");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_native_ui_frame_reset_and_write_cover_all_string_paths),
    BANANA_TEST_CASE(test_header_surfaces_compile_and_expose_declared_types)
)
