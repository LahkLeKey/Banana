#include "move_target_domain.h"

#include "../camera/camera_basis.h"
#include "input_contract.h"

#include <math.h>

static int runtime_is_finite(float value)
{
    return isfinite(value) ? 1 : 0;
}

void runtime_move_target_reset(RuntimeMoveTargetState *state)
{
    if (!state)
        return;

    state->has_target = 0;
    state->target.x = 0.f;
    state->target.z = 0.f;
}

int runtime_move_target_set(RuntimeMoveTargetState *state,
                            RuntimeMoveTargetPoint target)
{
    if (!state)
        return 0;

    if (!runtime_is_finite(target.x) || !runtime_is_finite(target.z))
        return 0;

    state->has_target = 1;
    state->target = target;
    return 1;
}

void runtime_move_target_clear(RuntimeMoveTargetState *state)
{
    runtime_move_target_reset(state);
}

int runtime_move_target_compute_steering(const RuntimeMoveTargetState *state,
                                         const float *player_position,
                                         const float *camera_eye,
                                         const float *camera_target,
                                         int camera_valid,
                                         float arrival_threshold,
                                         RuntimeMoveTargetSteering *out_steering)
{
    float dx = 0.f;
    float dz = 0.f;
    float distance = 0.f;
    float world_dir_x = 0.f;
    float world_dir_z = 0.f;
    float basis_forward[3] = {0.f, 0.f, -1.f};
    float basis_right[3] = {1.f, 0.f, 0.f};
    float local_move_x = 0.f;
    float local_move_z = 0.f;

    if (!out_steering)
        return 0;

    out_steering->has_target = state && state->has_target ? 1 : 0;
    out_steering->reached_target = 0;
    out_steering->move_x = 0.f;
    out_steering->move_z = 0.f;

    if (!state || !state->has_target || !player_position)
        return 1;

    dx = state->target.x - player_position[0];
    dz = state->target.z - player_position[2];
    distance = sqrtf(dx * dx + dz * dz);

    if (distance <= arrival_threshold)
    {
        out_steering->has_target = 0;
        out_steering->reached_target = 1;
        return 1;
    }

    world_dir_x = dx / distance;
    world_dir_z = dz / distance;

    /* Align auto-move steering with the same camera-relative basis used by motion tick. */
    (void)runtime_camera_compute_ground_basis(camera_eye,
                                              camera_target,
                                              camera_valid,
                                              basis_forward,
                                              basis_right);

    local_move_x = (world_dir_x * basis_right[0]) + (world_dir_z * basis_right[2]);
    local_move_z = (world_dir_x * basis_forward[0]) + (world_dir_z * basis_forward[2]);

    out_steering->move_x = local_move_x;
    out_steering->move_z = local_move_z;
    runtime_input_contract_sanitize_move_input(out_steering->move_x,
                                               out_steering->move_z,
                                               &out_steering->move_x,
                                               &out_steering->move_z);
    return 1;
}
