#include "../../../engine.h"

#include "engine_runtime_context.h"

#include "../../camera/basis/camera_basis.h"
#include "../../input/abi/input_abi.h"
#include "../../player/api/player_api.h"
#include "../../player/sync/player_sync_abi.h"
#include "../../terrain/terrain_abi.h"

static float runtime_engine_exports_terrain_sample_height(float x, float z)
{
    return runtime_terrain_abi_sample_height(s_service_ports,
                                             &s_engine_state,
                                             x,
                                             z);
}

int engine_get_click_count(void)
{
    return runtime_input_abi_get_click_count();
}

int engine_get_target_reached_count(void)
{
    return runtime_input_abi_get_target_reached_count();
}

int engine_get_has_move_target(void)
{
    return runtime_input_abi_get_has_move_target();
}

int engine_get_pbj_pickup_collected(void)
{
    return s_engine_state.pbj_pickup_collected ? 1 : 0;
}

int engine_handle_right_click(float canvas_x, float canvas_y)
{
    RuntimeInputCanvasPoint canvas;
    canvas.x = canvas_x;
    canvas.y = canvas_y;

    return runtime_input_abi_handle_right_click(&s_engine_state,
                                                canvas,
                                                runtime_engine_exports_terrain_sample_height);
}

int engine_handle_right_click_normalized(float screen_x, float screen_y)
{
    RuntimeScreenNormalizedPoint normalized;
    normalized.x = screen_x;
    normalized.y = screen_y;

    return runtime_input_abi_handle_right_click_normalized(&s_engine_state,
                                                           normalized,
                                                           runtime_engine_exports_terrain_sample_height);
}

void engine_set_move_input(float input_x, float input_z)
{
    float basis_forward[3] = {0.f, 0.f, -1.f};
    float basis_right[3] = {1.f, 0.f, 0.f};
    float local_x = input_x;
    float local_z = input_z;

    (void)runtime_camera_compute_ground_basis(s_engine_state.camera_eye,
                                              s_engine_state.camera_target,
                                              s_engine_state.camera_valid,
                                              basis_forward,
                                              basis_right);

    local_x = (input_x * basis_right[0]) + (input_z * basis_right[2]);
    local_z = (input_x * basis_forward[0]) + (input_z * basis_forward[2]);

    runtime_input_abi_set_move_input(&s_engine_state, local_x, local_z);
}

uint32_t engine_player_upsert(const char *player_guid, const char *player_name,
                              const char *controller_kind, int active)
{
    return runtime_player_api_upsert(s_engine_state.world,
                                     player_guid,
                                     player_name,
                                     controller_kind,
                                     active,
                                     runtime_engine_exports_terrain_sample_height,
                                     engine_controller_attach,
                                     &s_engine_state.player_id);
}

void engine_player_apply_input(const char *player_guid, float input_x, float input_z)
{
    runtime_player_api_apply_input(player_guid, input_x, input_z);
}

void engine_player_set_transform(const char *player_guid, float x, float y, float z, int active)
{
    float island_span = (float)(BANANA_ENGINE_TERRAIN_SIZE - 1) * 0.5f;
    runtime_player_api_set_transform(s_engine_state.world,
                                     player_guid,
                                     x,
                                     y,
                                     z,
                                     active,
                                     island_span,
                                     runtime_engine_exports_terrain_sample_height);
}

void engine_player_sync_mark_seen(const char *player_guid, int64_t current_time_ms)
{
    runtime_player_sync_abi_mark_seen(player_guid, current_time_ms);
}

void engine_player_sync_update_staleness(int64_t current_time_ms, int64_t stale_threshold_ms)
{
    RuntimePlayerSyncWindow window;
    window.current_time_ms = current_time_ms;
    window.stale_threshold_ms = stale_threshold_ms;
    runtime_player_sync_abi_update_staleness(window);
}

void engine_player_sync_deactivate_stale(void)
{
    runtime_player_sync_abi_deactivate_stale();
}

int engine_player_sync_count_active(void)
{
    return runtime_player_sync_abi_count_active();
}
