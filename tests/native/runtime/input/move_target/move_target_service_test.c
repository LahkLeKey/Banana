#include "runtime/input/contract/input_contract.h"
#include "runtime/input/move_target/move_target_domain.h"
#include "runtime/input/move_target/move_target_service.h"
#include "../../support/test_support.h"

#include <math.h>

static float flat_terrain_height(float x, float z)
{
    (void)x;
    (void)z;
    return 0.0f;
}

static float nan_terrain_height(float x, float z)
{
    (void)x;
    (void)z;
    return NAN;
}

static int move_target_test_setup(void **state)
{
    (void)state;
    runtime_input_contract_reset();
    return 0;
}

static int move_target_test_teardown(void **state)
{
    (void)state;
    runtime_input_contract_reset();
    return 0;
}

static void test_move_target_domain_state_helpers(void **state)
{
    (void)state;
    RuntimeMoveTargetState move_state = {1, {5.0f, -7.0f}};
    RuntimeMoveTargetPoint target = {12.5f, -4.5f};

    runtime_move_target_reset(&move_state);
    BANANA_TEST_ASSERT_INT_EQ(move_state.has_target, 0, "reset must clear the target flag");
    BANANA_TEST_ASSERT_FLOAT_EQ(move_state.target.x, 0.0f, 0.0001f,
                                "reset must clear target x");
    BANANA_TEST_ASSERT_FLOAT_EQ(move_state.target.z, 0.0f, 0.0001f,
                                "reset must clear target z");

    BANANA_TEST_ASSERT_INT_EQ(runtime_move_target_set(&move_state, target), 1,
                              "set must accept finite targets");
    BANANA_TEST_ASSERT_INT_EQ(move_state.has_target, 1,
                              "set must mark the target as active");
    BANANA_TEST_ASSERT_FLOAT_EQ(move_state.target.x, 12.5f, 0.0001f,
                                "set must store the x coordinate");
    BANANA_TEST_ASSERT_FLOAT_EQ(move_state.target.z, -4.5f, 0.0001f,
                                "set must store the z coordinate");

    BANANA_TEST_ASSERT_INT_EQ(runtime_move_target_set(&move_state, (RuntimeMoveTargetPoint){NAN, 1.0f}), 0,
                              "set must reject non-finite coordinates");

    runtime_move_target_clear(&move_state);
    BANANA_TEST_ASSERT_INT_EQ(move_state.has_target, 0,
                              "clear must reset the state");
}

static void test_move_target_compute_steering(void **state)
{
    (void)state;
    RuntimeMoveTargetState move_state = {0};
    RuntimeMoveTargetSteering steering = {0};
    float player_position[3] = {0.0f, 0.0f, 0.0f};
    float camera_eye[3] = {0.0f, 10.0f, 10.0f};
    float camera_target[3] = {0.0f, 0.0f, 0.0f};

    runtime_move_target_set(&move_state, (RuntimeMoveTargetPoint){0.0f, 8.0f});
    BANANA_TEST_ASSERT_INT_EQ(runtime_move_target_compute_steering(&move_state,
                                                                   player_position,
                                                                   camera_eye,
                                                                   camera_target,
                                                                   1,
                                                                   0.5f,
                                                                   &steering),
                              1,
                              "compute_steering must succeed for a valid move target");
    BANANA_TEST_ASSERT_INT_EQ(steering.has_target, 1,
                              "compute_steering must preserve the target flag when the target is far enough away");
    BANANA_TEST_ASSERT_INT_EQ(steering.reached_target, 0,
                              "compute_steering must not mark a distant target as reached");
    BANANA_TEST_ASSERT_TRUE(isfinite(steering.move_x) && isfinite(steering.move_z),
                            "compute_steering must produce finite motion values");

    runtime_move_target_set(&move_state, (RuntimeMoveTargetPoint){0.0f, 0.1f});
    BANANA_TEST_ASSERT_INT_EQ(runtime_move_target_compute_steering(&move_state,
                                                                   player_position,
                                                                   camera_eye,
                                                                   camera_target,
                                                                   1,
                                                                   1.0f,
                                                                   &steering),
                              1,
                              "compute_steering must still succeed when the target is close");
    BANANA_TEST_ASSERT_INT_EQ(steering.has_target, 0,
                              "compute_steering must clear the target flag when arrival threshold is reached");
    BANANA_TEST_ASSERT_INT_EQ(steering.reached_target, 1,
                              "compute_steering must mark close targets as reached");
}

static void test_move_target_apply_click(void **state)
{
    (void)state;
    RuntimeMoveTargetState move_state = {0};
    RuntimeViewportSize viewport = {1280, 720};
    RuntimeScreenNormalizedPoint normalized = {0.0f, 0.0f};
    RuntimeMoveTargetApplyResult result = RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_INPUT;

    result = runtime_move_target_apply_click(&move_state,
                                             normalized,
                                             (const float[]){0.0f, 10.0f, 10.0f},
                                             (const float[]){0.0f, 0.0f, 0.0f},
                                             1,
                                             viewport,
                                             64.0f,
                                             flat_terrain_height);
    BANANA_TEST_ASSERT_INT_EQ(result, RUNTIME_MOVE_TARGET_APPLY_ACCEPTED,
                              "apply_click must accept a valid click raycast");
    BANANA_TEST_ASSERT_INT_EQ(runtime_input_contract_get_has_move_target(), 1,
                              "accepted clicks must enable the move-target contract flag");
    BANANA_TEST_ASSERT_INT_EQ(move_state.has_target, 1,
                              "accepted clicks must store a move target");
    BANANA_TEST_ASSERT_INT_EQ(runtime_move_target_apply_result_to_abi_status(result), 1,
                              "accepted results must map to abi status 1");

    result = runtime_move_target_apply_click(&move_state,
                                             normalized,
                                             (const float[]){0.0f, 10.0f, 10.0f},
                                             (const float[]){0.0f, 0.0f, 0.0f},
                                             0,
                                             viewport,
                                             64.0f,
                                             flat_terrain_height);
    BANANA_TEST_ASSERT_INT_EQ(result, RUNTIME_MOVE_TARGET_APPLY_RAYCAST_FAILED,
                              "apply_click must reject invalid camera state");
    BANANA_TEST_ASSERT_INT_EQ(runtime_input_contract_get_has_move_target(), 0,
                              "failed clicks must clear the move-target contract flag");
    BANANA_TEST_ASSERT_INT_EQ(runtime_move_target_apply_result_to_abi_status(result), 0,
                              "raycast failures must map to abi status 0");

    result = runtime_move_target_apply_click(&move_state,
                                             normalized,
                                             (const float[]){0.0f, 10.0f, 10.0f},
                                             (const float[]){0.0f, 0.0f, 0.0f},
                                             1,
                                             viewport,
                                             64.0f,
                                             nan_terrain_height);
    BANANA_TEST_ASSERT_INT_EQ(result, RUNTIME_MOVE_TARGET_APPLY_REJECTED_INVALID_TARGET,
                              "apply_click must reject non-finite projected targets");
    BANANA_TEST_ASSERT_INT_EQ(runtime_input_contract_get_has_move_target(), 0,
                              "invalid targets must clear the move-target contract flag");
    BANANA_TEST_ASSERT_INT_EQ(runtime_move_target_apply_result_to_abi_status((RuntimeMoveTargetApplyResult)99), 0,
                              "out-of-range result values must map to zero");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_move_target_domain_state_helpers, move_target_test_setup, move_target_test_teardown),
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_move_target_compute_steering, move_target_test_setup, move_target_test_teardown),
    BANANA_TEST_CASE_SETUP_TEARDOWN(test_move_target_apply_click, move_target_test_setup, move_target_test_teardown)
)