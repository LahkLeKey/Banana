#include "application_service_ports.h"
#include "application_service_render_port.h"

#include "../input/move_target/move_target_service.h"
#include "../player/player_runtime_service.h"
#include "../terrain/terrain_height.h"
#include "../terrain/terrain_host.h"
#include "../terrain/terrain_mutation.h"

static float terrain_sample_height_port(const EngineRuntimeState *state, float x, float z)
{
    if (!state)
        return 0.f;

    return runtime_terrain_sample_height(&state->terrain_height[0][0],
                                         BANANA_ENGINE_TERRAIN_SIZE,
                                         x,
                                         z,
                                         0.48f,
                                         -1.45f);
}

static int terrain_rebuild_dirty_chunks_port(EngineRuntimeState *state, int max_chunks)
{
    if (!state)
        return 0;

    return runtime_terrain_host_rebuild_dirty(&state->terrain_height[0][0],
                                              BANANA_ENGINE_TERRAIN_SIZE,
                                              BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                                              state->terrain_chunks,
                                              BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                                              BANANA_ENGINE_TERRAIN_CHUNK_ROWS,
                                              max_chunks);
}

static void terrain_draw_port(EngineRuntimeState *state)
{
    if (!state)
        return;

    runtime_terrain_draw(state->renderer,
                         state->terrain_chunks,
                         state->terrain_initialized,
                         BANANA_ENGINE_TERRAIN_SIZE,
                         BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                         BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                         BANANA_ENGINE_TERRAIN_CHUNK_ROWS);
}

static EngineRuntimeState *s_mutation_state = NULL;

static void mark_chunk_dirty_port(int chunk_x, int chunk_z)
{
    if (!s_mutation_state)
        return;

    runtime_terrain_host_mark_chunk_dirty(s_mutation_state->terrain_chunks,
                                          BANANA_ENGINE_TERRAIN_CHUNK_COLS,
                                          BANANA_ENGINE_TERRAIN_CHUNK_ROWS,
                                          chunk_x,
                                          chunk_z);
}

static int terrain_set_height_port(EngineRuntimeState *state, int x, int z, int elevation)
{
    int result = 0;

    if (!state)
        return -1;

    s_mutation_state = state;
    result = runtime_terrain_set_height(&state->terrain_height[0][0],
                                        BANANA_ENGINE_TERRAIN_SIZE,
                                        BANANA_ENGINE_TERRAIN_MAX_LAYERS,
                                        BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                                        x,
                                        z,
                                        elevation,
                                        mark_chunk_dirty_port);
    s_mutation_state = NULL;
    return result;
}

static void terrain_mark_region_dirty_port(EngineRuntimeState *state,
                                           int min_x,
                                           int min_z,
                                           int max_x,
                                           int max_z)
{
    if (!state)
        return;

    s_mutation_state = state;
    runtime_terrain_mark_region_dirty(BANANA_ENGINE_TERRAIN_SIZE,
                                      BANANA_ENGINE_TERRAIN_CHUNK_SIZE,
                                      min_x,
                                      min_z,
                                      max_x,
                                      max_z,
                                      mark_chunk_dirty_port);
    s_mutation_state = NULL;
}

static void player_update_motion_port(EngineRuntimeState *state,
                                      float dt,
                                      RuntimeTerrainSampleHeightFn sample_height)
{
    if (!state || !sample_height)
        return;

    runtime_player_runtime_update_motion(state->world,
                                         state->player_id,
                                         &state->move_target_state,
                                         &state->move_input_x,
                                         &state->move_input_z,
                                         state->camera_eye,
                                         state->camera_target,
                                         state->camera_valid,
                                         5.5f,
                                         dt,
                                         (float)(BANANA_ENGINE_TERRAIN_SIZE - 1) * 0.5f,
                                         sample_height);
}

static void player_follow_camera_port(EngineRuntimeState *state)
{
    if (!state)
        return;

    runtime_player_runtime_follow_camera(state->renderer,
                                         state->world,
                                         state->player_id,
                                         state->viewport_width,
                                         state->viewport_height,
                                         state->camera_eye,
                                         state->camera_target,
                                         &state->camera_valid);
}

static void player_apply_click_input_port(EngineRuntimeState *state,
                                          float normalized_x,
                                          float normalized_y,
                                          RuntimeTerrainSampleHeightFn sample_height)
{
    RuntimeScreenNormalizedPoint normalized = {0};
    RuntimeViewportSize viewport = {0};

    if (!state || !sample_height)
        return;

    normalized.x = normalized_x;
    normalized.y = normalized_y;
    viewport.width = state->viewport_width;
    viewport.height = state->viewport_height;

    (void)runtime_move_target_apply_click(&state->move_target_state,
                                          normalized,
                                          state->camera_eye,
                                          state->camera_target,
                                          state->camera_valid,
                                          viewport,
                                          (float)(BANANA_ENGINE_TERRAIN_SIZE - 1) * 0.5f,
                                          sample_height);
}

static const RuntimeApplicationServicePorts k_ports = {
    .terrain = {
        .sample_height = terrain_sample_height_port,
        .rebuild_dirty_chunks = terrain_rebuild_dirty_chunks_port,
        .draw = terrain_draw_port,
        .set_height = terrain_set_height_port,
        .mark_region_dirty = terrain_mark_region_dirty_port,
    },
    .player = {
        .update_motion = player_update_motion_port,
        .follow_camera = player_follow_camera_port,
        .apply_click_input = player_apply_click_input_port,
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
