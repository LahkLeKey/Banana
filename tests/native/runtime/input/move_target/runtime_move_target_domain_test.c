#include "runtime/input/move_target/move_target_domain.h"
#include "runtime/input/move_target/move_target_service.h"
#include "runtime/input/contract/input_contract.h"

#include <stdio.h>

static float flat_terrain_sample_height(float x, float z)
{
    (void)x;
    (void)z;
    return 0.0f;
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_true(const char *label, int actual)
{
    return expect_int(label, actual ? 1 : 0, 1);
}

int main(void)
{
    RuntimeMoveTargetState state = {0};
    RuntimeMoveTargetPoint target = {10.0f, 0.0f};
    RuntimeMoveTargetSteering steering = {0};
    float player_position[3] = {0.0f, 0.0f, 0.0f};
    float camera_eye[3] = {0.0f, 10.0f, 10.0f};
    float camera_target[3] = {0.0f, 0.0f, 0.0f};
    float first_target_x = 0.0f;
    float first_target_z = 0.0f;
    RuntimeMoveTargetApplyResult apply_result = RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_INPUT;
    RuntimeViewportSize invalid_viewport = {640, 0};
    RuntimeViewportSize valid_viewport = {640, 360};

    runtime_move_target_reset(&state);
    if (!expect_int("initial no target", state.has_target, 0))
        return 1;

    if (!expect_true("set target", runtime_move_target_set(&state, target)))
        return 1;
    if (!expect_int("target set flag", state.has_target, 1))
        return 1;

    if (!expect_true("compute steering", runtime_move_target_compute_steering(&state,
                                                                               player_position,
                                                                               camera_eye,
                                                                               camera_target,
                                                                               1,
                                                                               0.35f,
                                                                               &steering)))
        return 1;
    if (!expect_int("steering has target", steering.has_target, 1))
        return 1;
    if (!expect_int("not reached", steering.reached_target, 0))
        return 1;

    player_position[0] = 10.0f;
    player_position[2] = 0.0f;
    if (!expect_true("compute reached", runtime_move_target_compute_steering(&state,
                                                                              player_position,
                                                                              camera_eye,
                                                                              camera_target,
                                                                              1,
                                                                              0.35f,
                                                                              &steering)))
        return 1;
    if (!expect_int("reached target", steering.reached_target, 1))
        return 1;
    if (!expect_int("target cleared in steering", steering.has_target, 0))
        return 1;

    runtime_move_target_clear(&state);
    if (!expect_int("state cleared", state.has_target, 0))
        return 1;

    runtime_input_contract_reset();
    apply_result = runtime_move_target_apply_click(&state,
                                                   (RuntimeScreenNormalizedPoint){0.0f, 0.0f},
                                                   camera_eye,
                                                   camera_target,
                                                   1,
                                                   invalid_viewport,
                                                   64.0f,
                                                   flat_terrain_sample_height);
    if (!expect_int("invalid viewport click rejected",
                    apply_result,
                    RUNTIME_MOVE_TARGET_APPLY_RAYCAST_FAILED))
        return 1;
    if (!expect_int("invalid viewport maps to abi reject",
                    runtime_move_target_apply_result_to_abi_status(apply_result),
                    0))
        return 1;
    if (!expect_int("state remains cleared after invalid viewport", state.has_target, 0))
        return 1;
    if (!expect_int("input contract cleared after invalid viewport",
                    runtime_input_contract_get_has_move_target(),
                    0))
        return 1;

    apply_result = runtime_move_target_apply_click(&state,
                                                   (RuntimeScreenNormalizedPoint){-0.8f, 0.0f},
                                                   camera_eye,
                                                   camera_target,
                                                   1,
                                                   valid_viewport,
                                                   64.0f,
                                                   flat_terrain_sample_height);
    if (!expect_int("first click accepted",
                    apply_result,
                    RUNTIME_MOVE_TARGET_APPLY_ACCEPTED))
        return 1;
    if (!expect_int("accepted maps to abi success",
                    runtime_move_target_apply_result_to_abi_status(apply_result),
                    1))
        return 1;
    if (!expect_int("state active after first click", state.has_target, 1))
        return 1;
    first_target_x = state.target.x;
    first_target_z = state.target.z;

    apply_result = runtime_move_target_apply_click(&state,
                                                   (RuntimeScreenNormalizedPoint){0.8f, 0.0f},
                                                   camera_eye,
                                                   camera_target,
                                                   1,
                                                   valid_viewport,
                                                   64.0f,
                                                   flat_terrain_sample_height);
    if (!expect_int("second click accepted",
                    apply_result,
                    RUNTIME_MOVE_TARGET_APPLY_ACCEPTED))
        return 1;
    if (!expect_int("state active after second click", state.has_target, 1))
        return 1;
    if (!expect_true("second click retargets x",
                     state.target.x > first_target_x + 0.01f))
        return 1;
    if (!expect_true("repeated click updates target",
                     state.target.x != first_target_x ||
                         state.target.z != first_target_z))
        return 1;
    if (!expect_int("input contract active after valid click",
                    runtime_input_contract_get_has_move_target(),
                    1))
        return 1;

    return 0;
}
