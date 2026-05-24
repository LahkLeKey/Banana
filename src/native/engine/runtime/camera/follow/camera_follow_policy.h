#ifndef BANANA_ENGINE_RUNTIME_CAMERA_FOLLOW_POLICY_H
#define BANANA_ENGINE_RUNTIME_CAMERA_FOLLOW_POLICY_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeCameraFollowPose
    {
        float eye[3];
        float target[3];
        float fov_degrees;
        float near_plane;
        float far_plane;
        float aspect;
    } RuntimeCameraFollowPose;

    int runtime_camera_follow_policy_compute(const float *player_position,
                                             int viewport_width,
                                             int viewport_height,
                                             RuntimeCameraFollowPose *out_pose);

#ifdef __cplusplus
}
#endif

#endif
