#include "runtime/engine/application/service/application_service_ports.h"
#include "runtime/engine/application/service/application_service_player_port.h"
#include "runtime/engine/application/service/application_service_render_port.h"
#include "runtime/engine/application/service/application_service_terrain_port.h"

#include "runtime/support/test_support.h"

static void test_application_service_ports_resolve_expected_bindings(void **state)
{
    (void)state;
    const RuntimeApplicationServicePorts *ports = runtime_application_service_ports();

    BANANA_TEST_ASSERT_TRUE(ports != NULL, "service ports must resolve to a static table");

    BANANA_TEST_ASSERT_TRUE(ports->terrain.sample_height == runtime_application_terrain_sample_height_port,
                            "terrain sample-height binding must use the terrain port adapter");
    BANANA_TEST_ASSERT_TRUE(ports->terrain.rebuild_dirty_chunks == runtime_application_terrain_rebuild_dirty_chunks_port,
                            "terrain rebuild binding must use the terrain port adapter");
    BANANA_TEST_ASSERT_TRUE(ports->terrain.draw == runtime_application_terrain_draw_port,
                            "terrain draw binding must use the terrain port adapter");
    BANANA_TEST_ASSERT_TRUE(ports->terrain.set_height == runtime_application_terrain_set_height_port,
                            "terrain set-height binding must use the terrain port adapter");
    BANANA_TEST_ASSERT_TRUE(ports->terrain.mark_region_dirty == runtime_application_terrain_mark_region_dirty_port,
                            "terrain dirty-region binding must use the terrain port adapter");

    BANANA_TEST_ASSERT_TRUE(ports->player.update_motion == runtime_application_player_update_motion_port,
                            "player update-motion binding must use the player port adapter");
    BANANA_TEST_ASSERT_TRUE(ports->player.follow_camera == runtime_application_player_follow_camera_port,
                            "player follow-camera binding must use the player port adapter");
    BANANA_TEST_ASSERT_TRUE(ports->player.apply_click_input == runtime_application_player_apply_click_input_port,
                            "player click-input binding must use the player port adapter");

    BANANA_TEST_ASSERT_TRUE(ports->merchant.get_price == runtime_merchant_service_get_price,
                            "merchant get-price binding must use merchant service");
    BANANA_TEST_ASSERT_TRUE(ports->merchant.trade_buy == runtime_merchant_service_trade_buy,
                            "merchant buy binding must use merchant service");
    BANANA_TEST_ASSERT_TRUE(ports->merchant.trade_sell == runtime_merchant_service_trade_sell,
                            "merchant sell binding must use merchant service");

    BANANA_TEST_ASSERT_TRUE(ports->render.submit_scene == runtime_application_render_submit_scene_port,
                            "render submit-scene binding must use render port adapter");
}

static void test_application_service_ports_reset_is_callable(void **state)
{
    (void)state;

    runtime_application_service_ports_reset();

    BANANA_TEST_ASSERT_TRUE(1,
                            "service ports reset must be callable without requiring additional setup");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_application_service_ports_resolve_expected_bindings),
    BANANA_TEST_CASE(test_application_service_ports_reset_is_callable)
)
