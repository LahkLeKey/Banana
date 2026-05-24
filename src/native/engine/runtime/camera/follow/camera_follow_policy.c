#include "camera_follow_policy.h"

int runtime_camera_follow_policy_compute(const float *player_position,
                                         int viewport_width,
                                         int viewport_height,
                                         RuntimeCameraFollowPose *out_pose)
{
    float aspect = 1.0f;

    if (!player_position || viewport_width <= 0 || viewport_height <= 0 || !out_pose)
        return 0;

    aspect = (float)viewport_width / (float)viewport_height;

    out_pose->eye[0] = player_position[0] - 10.0f;
    out_pose->eye[1] = player_position[1] + 13.5f;
    out_pose->eye[2] = player_position[2] - 10.0f;

    out_pose->target[0] = player_position[0];
    out_pose->target[1] = player_position[1] + 1.35f;
    out_pose->target[2] = player_position[2];

    out_pose->fov_degrees = 52.0f;
    out_pose->near_plane = 0.1f;
    out_pose->far_plane = 1000.0f;
    out_pose->aspect = aspect;

    return 1;
}
