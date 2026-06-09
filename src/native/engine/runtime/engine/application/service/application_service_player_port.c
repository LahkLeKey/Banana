#include "application_service_player_port.h"

#include "../../../input/move_target/move_target_service.h"
#include "../../../player/runtime/player_runtime_service.h"

void runtime_application_player_update_motion_port(EngineRuntimeState *state,
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

void runtime_application_player_follow_camera_port(EngineRuntimeState *state)
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

void runtime_application_player_apply_click_input_port(EngineRuntimeState *state,
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