#include "application_service_ports.h"
#include "application_service_player_port.h"
#include "application_service_render_port.h"
#include "application_service_terrain_port.h"

static const RuntimeApplicationServicePorts k_ports = {
    .terrain = {
        .sample_height = runtime_application_terrain_sample_height_port,
        .rebuild_dirty_chunks = runtime_application_terrain_rebuild_dirty_chunks_port,
        .draw = runtime_application_terrain_draw_port,
        .set_height = runtime_application_terrain_set_height_port,
        .mark_region_dirty = runtime_application_terrain_mark_region_dirty_port,
    },
    .player = {
        .update_motion = runtime_application_player_update_motion_port,
        .follow_camera = runtime_application_player_follow_camera_port,
        .apply_click_input = runtime_application_player_apply_click_input_port,
    },
    .merchant = {
        .get_price = runtime_merchant_service_get_price,
        .trade_buy = runtime_merchant_service_trade_buy,
        .trade_sell = runtime_merchant_service_trade_sell,
    },
    .render = {
        .submit_scene = runtime_application_render_submit_scene_port,
    },
};

const RuntimeApplicationServicePorts *runtime_application_service_ports(void)
{
    return &k_ports;
}

void runtime_application_service_ports_reset(void)
{
    runtime_application_render_ports_reset();
}
