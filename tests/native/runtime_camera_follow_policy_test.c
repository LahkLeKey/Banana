#include "runtime/camera_follow_policy.h"

#include <math.h>
#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_float(const char *label, float actual, float expected, float tolerance)
{
    float delta = fabsf(actual - expected);
    if (delta <= tolerance)
        return 1;

    fprintf(stderr,
            "%s expected=%.5f actual=%.5f delta=%.5f tolerance=%.5f\n",
            label,
            expected,
            actual,
            delta,
            tolerance);
    return 0;
}

int main(void)
{
    RuntimeCameraFollowPose pose = {0};
    float player[3] = {2.5f, 0.7f, -4.0f};

    if (!expect_int("null player rejected",
                    runtime_camera_follow_policy_compute(NULL, 1280, 720, &pose),
                    0))
        return 1;

    if (!expect_int("zero width rejected",
                    runtime_camera_follow_policy_compute(player, 0, 720, &pose),
                    0))
        return 1;

    if (!expect_int("null pose rejected",
                    runtime_camera_follow_policy_compute(player, 1280, 720, NULL),
                    0))
        return 1;

    if (!expect_int("valid pose accepted",
                    runtime_camera_follow_policy_compute(player, 1280, 720, &pose),
                    1))
        return 1;

    if (!expect_float("eye x", pose.eye[0], -7.5f, 0.001f))
        return 1;
    if (!expect_float("eye y", pose.eye[1], 14.2f, 0.001f))
        return 1;
    if (!expect_float("eye z", pose.eye[2], -14.0f, 0.001f))
        return 1;

    if (!expect_float("target x", pose.target[0], 2.5f, 0.001f))
        return 1;
    if (!expect_float("target y", pose.target[1], 2.05f, 0.001f))
        return 1;
    if (!expect_float("target z", pose.target[2], -4.0f, 0.001f))
        return 1;

    if (!expect_float("fov", pose.fov_degrees, 52.0f, 0.001f))
        return 1;
    if (!expect_float("near", pose.near_plane, 0.1f, 0.0001f))
        return 1;
    if (!expect_float("far", pose.far_plane, 1000.0f, 0.001f))
        return 1;
    if (!expect_float("aspect", pose.aspect, 1280.0f / 720.0f, 0.0001f))
        return 1;

    return 0;
}
