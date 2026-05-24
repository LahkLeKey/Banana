#include "move_target_domain.h"

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

    (void)camera_eye;
    (void)camera_target;
    (void)camera_valid;

    out_steering->move_x = world_dir_x;
    out_steering->move_z = world_dir_z;
    runtime_input_contract_sanitize_move_input(out_steering->move_x,
                                               out_steering->move_z,
                                               &out_steering->move_x,
                                               &out_steering->move_z);
    return 1;
}
