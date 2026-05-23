#include "runtime/move_target_domain.h"

#include <stdio.h>

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

    return 0;
}
