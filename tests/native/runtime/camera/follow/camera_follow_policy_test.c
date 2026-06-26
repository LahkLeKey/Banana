#include "runtime/camera/follow/camera_follow_policy.h"

#include "runtime/support/test_support.h"

static void test_follow_policy_rejects_invalid_inputs(void **state)
{
    RuntimeCameraFollowPose pose;
    float player[3] = {1.0f, 2.0f, 3.0f};

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(runtime_camera_follow_policy_compute(NULL, 1280, 720, &pose),
                              0,
                              "follow policy should reject null player position");
    BANANA_TEST_ASSERT_INT_EQ(runtime_camera_follow_policy_compute(player, 0, 720, &pose),
                              0,
                              "follow policy should reject non-positive viewport width");
    BANANA_TEST_ASSERT_INT_EQ(runtime_camera_follow_policy_compute(player, 1280, 0, &pose),
                              0,
                              "follow policy should reject non-positive viewport height");
    BANANA_TEST_ASSERT_INT_EQ(runtime_camera_follow_policy_compute(player, 1280, 720, NULL),
                              0,
                              "follow policy should reject null output pose");
}

static void test_follow_policy_populates_pose_for_valid_input(void **state)
{
    RuntimeCameraFollowPose pose;
    float player[3] = {10.0f, 2.0f, -4.0f};

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(runtime_camera_follow_policy_compute(player, 1920, 1080, &pose),
                              1,
                              "follow policy should accept valid input");

    BANANA_TEST_ASSERT_FLOAT_EQ(pose.eye[0], 0.0f, 0.0001f, "eye x should trail player");
    BANANA_TEST_ASSERT_FLOAT_EQ(pose.eye[1], 15.5f, 0.0001f, "eye y should elevate over player");
    BANANA_TEST_ASSERT_FLOAT_EQ(pose.eye[2], -14.0f, 0.0001f, "eye z should trail player");

    BANANA_TEST_ASSERT_FLOAT_EQ(pose.target[0], 10.0f, 0.0001f, "target x should match player");
    BANANA_TEST_ASSERT_FLOAT_EQ(pose.target[1], 3.35f, 0.0001f, "target y should offset from player");
    BANANA_TEST_ASSERT_FLOAT_EQ(pose.target[2], -4.0f, 0.0001f, "target z should match player");

    BANANA_TEST_ASSERT_FLOAT_EQ(pose.fov_degrees, 52.0f, 0.0001f, "fov should match policy default");
    BANANA_TEST_ASSERT_FLOAT_EQ(pose.near_plane, 0.1f, 0.0001f, "near plane should match policy default");
    BANANA_TEST_ASSERT_FLOAT_EQ(pose.far_plane, 1000.0f, 0.0001f, "far plane should match policy default");
    BANANA_TEST_ASSERT_FLOAT_EQ(pose.aspect, 1920.0f / 1080.0f, 0.0001f, "aspect should derive from viewport");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_follow_policy_rejects_invalid_inputs),
    BANANA_TEST_CASE(test_follow_policy_populates_pose_for_valid_input)
)
