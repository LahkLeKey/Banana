#include "move_target_service.h"

#include "input/input_contract.h"

#include <math.h>
#include <stdio.h>

#define BANANA_COMPILE_ASSERT(name, expr) typedef char banana_compile_assert_##name[(expr) ? 1 : -1]

static const int k_move_target_apply_result_to_abi_status[RUNTIME_MOVE_TARGET_APPLY_RESULT_COUNT] = {
    0, /* RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_INPUT */
    1, /* RUNTIME_MOVE_TARGET_APPLY_ACCEPTED */
    0, /* RUNTIME_MOVE_TARGET_APPLY_RAYCAST_FAILED */
    0, /* RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_TARGET */
};

BANANA_COMPILE_ASSERT(move_target_apply_result_table_size,
                             (sizeof(k_move_target_apply_result_to_abi_status) /
                              sizeof(k_move_target_apply_result_to_abi_status[0])) ==
                                  RUNTIME_MOVE_TARGET_APPLY_RESULT_COUNT);

static int runtime_move_target_raycast_to_world(float normalized_x,
                                                float normalized_y,
                                                const float *camera_eye,
                                                const float *camera_target,
                                                int camera_valid,
                                                int viewport_width,
                                                int viewport_height,
                                                float island_span,
                                                RuntimeTerrainSampleHeightFn sample_height,
                                                float *out_target_x,
                                                float *out_target_z)
{
    float forward[3] = {0.f, 0.f, 0.f};
    float right[3] = {0.f, 0.f, 0.f};
    float up[3] = {0.f, 1.f, 0.f};
    float world_up[3] = {0.f, 1.f, 0.f};
    float dir[3] = {0.f, 0.f, 0.f};
    const float fov_radians = 44.f * (3.14159265358979323846f / 180.f);
    const float tan_half_fov = tanf(fov_radians * 0.5f);
    float aspect = 1.f;
    float ground_y = 0.f;
    float t = 0.f;
    float target_x = 0.f;
    float target_z = 0.f;

    if (!camera_eye || !camera_target || !sample_height || !out_target_x || !out_target_z ||
        !camera_valid || viewport_height <= 0)
        return 0;

    forward[0] = camera_target[0] - camera_eye[0];
    forward[1] = camera_target[1] - camera_eye[1];
    forward[2] = camera_target[2] - camera_eye[2];
    {
        float forward_len = sqrtf(forward[0] * forward[0] +
                                  forward[1] * forward[1] +
                                  forward[2] * forward[2]);
        if (forward_len <= 1e-6f)
            return 0;
        forward[0] /= forward_len;
        forward[1] /= forward_len;
        forward[2] /= forward_len;
    }

    right[0] = forward[1] * world_up[2] - forward[2] * world_up[1];
    right[1] = forward[2] * world_up[0] - forward[0] * world_up[2];
    right[2] = forward[0] * world_up[1] - forward[1] * world_up[0];
    {
        float right_len = sqrtf(right[0] * right[0] + right[1] * right[1] + right[2] * right[2]);
        if (right_len <= 1e-6f)
            return 0;
        right[0] /= right_len;
        right[1] /= right_len;
        right[2] /= right_len;
    }

    up[0] = right[1] * forward[2] - right[2] * forward[1];
    up[1] = right[2] * forward[0] - right[0] * forward[2];
    up[2] = right[0] * forward[1] - right[1] * forward[0];
    {
        float up_len = sqrtf(up[0] * up[0] + up[1] * up[1] + up[2] * up[2]);
        if (up_len <= 1e-6f)
            return 0;
        up[0] /= up_len;
        up[1] /= up_len;
        up[2] /= up_len;
    }

    if (up[1] <= 1e-4f)
        return 0;

    aspect = (float)viewport_width / (float)viewport_height;
    dir[0] = forward[0] + right[0] * normalized_x * aspect * tan_half_fov + up[0] * normalized_y * tan_half_fov;
    dir[1] = forward[1] + right[1] * normalized_x * aspect * tan_half_fov + up[1] * normalized_y * tan_half_fov;
    dir[2] = forward[2] + right[2] * normalized_x * aspect * tan_half_fov + up[2] * normalized_y * tan_half_fov;

    {
        float dir_len = sqrtf(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
        if (dir_len <= 1e-6f)
            return 0;
        dir[0] /= dir_len;
        dir[1] /= dir_len;
        dir[2] /= dir_len;
    }

    if (fabsf(dir[1]) <= 1e-6f)
        return 0;

    ground_y = sample_height(camera_target[0], camera_target[2]);
    t = (ground_y - camera_eye[1]) / dir[1];
    if (t <= 0.f)
        return 0;

    target_x = camera_eye[0] + dir[0] * t;
    target_z = camera_eye[2] + dir[2] * t;

    {
        int i = 0;
        for (i = 0; i < 2; i++)
        {
            ground_y = sample_height(target_x, target_z);
            t = (ground_y - camera_eye[1]) / dir[1];
            if (t <= 0.f)
                break;
            target_x = camera_eye[0] + dir[0] * t;
            target_z = camera_eye[2] + dir[2] * t;
        }
    }

    if (target_x < -island_span)
        target_x = -island_span;
    if (target_x > island_span)
        target_x = island_span;
    if (target_z < -island_span)
        target_z = -island_span;
    if (target_z > island_span)
        target_z = island_span;

    *out_target_x = target_x;
    *out_target_z = target_z;
    return 1;
}

RuntimeMoveTargetApplyResult runtime_move_target_apply_click(RuntimeMoveTargetState *state,
                                                             RuntimeScreenNormalizedPoint normalized,
                                                             const float *camera_eye,
                                                             const float *camera_target,
                                                             int camera_valid,
                                                             RuntimeViewportSize viewport,
                                                             float island_span,
                                                             RuntimeTerrainSampleHeightFn sample_height)
{
    float target_x = 0.f;
    float target_z = 0.f;
    const float normalized_x = normalized.x;
    const float normalized_y = normalized.y;

    if (!state)
        return RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_INPUT;

    if (runtime_move_target_raycast_to_world(normalized_x,
                                             normalized_y,
                                             camera_eye,
                                             camera_target,
                                             camera_valid,
                                             viewport.width,
                                             viewport.height,
                                             island_span,
                                             sample_height,
                                             &target_x,
                                             &target_z))
    {
        RuntimeMoveTargetPoint target = {target_x, target_z};
        if (runtime_move_target_set(state, target))
        {
            runtime_input_contract_set_has_move_target(1);
            fprintf(stdout,
                    "[engine] banana-raycast target set x=%.2f z=%.2f from normalized=(%.2f, %.2f)\n",
                    target_x,
                    target_z,
                    normalized_x,
                    normalized_y);
            return RUNTIME_MOVE_TARGET_APPLY_ACCEPTED;
        }

        runtime_move_target_clear(state);
        runtime_input_contract_set_has_move_target(0);
        fprintf(stderr,
                "[engine] banana-raycast rejected invalid target from normalized=(%.2f, %.2f)\n",
                normalized_x,
                normalized_y);
        return RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_TARGET;
    }

    runtime_move_target_clear(state);
    runtime_input_contract_set_has_move_target(0);
    fprintf(stderr,
            "[engine] banana-raycast failed for normalized=(%.2f, %.2f)\n",
            normalized_x,
            normalized_y);
    return RUNTIME_MOVE_TARGET_APPLY_RAYCAST_FAILED;
}

int runtime_move_target_apply_result_to_abi_status(RuntimeMoveTargetApplyResult result)
{
    if (result < 0 || result >= RUNTIME_MOVE_TARGET_APPLY_RESULT_COUNT)
        return 0;

    return k_move_target_apply_result_to_abi_status[result];
}
