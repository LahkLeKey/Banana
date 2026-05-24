#include "camera_follow.h"

#include "camera_follow_policy.h"

#include "../render/camera.h"

#include <stddef.h>

void runtime_camera_follow_player(Renderer *renderer,
                                  World *world,
                                  EntityId player_id,
                                  int viewport_width,
                                  int viewport_height,
                                  float *camera_eye,
                                  float *camera_target,
                                  int *camera_valid)
{
    Entity *player = NULL;
    Camera camera = {0};
    RuntimeCameraFollowPose pose = {0};

    if (!renderer || !world || !player_id || viewport_width <= 0 || viewport_height <= 0 ||
        !camera_eye || !camera_target || !camera_valid)
        return;

    player = world_get_entity(world, player_id);
    if (!player || !player->active)
        return;

    if (!runtime_camera_follow_policy_compute(player->position,
                                              viewport_width,
                                              viewport_height,
                                              &pose))
        return;

    camera = camera_create(pose.fov_degrees,
                           pose.aspect,
                           pose.near_plane,
                           pose.far_plane);
    camera_look_at(&camera,
                   pose.eye[0],
                   pose.eye[1],
                   pose.eye[2],
                   pose.target[0],
                   pose.target[1],
                   pose.target[2]);

    camera_eye[0] = pose.eye[0];
    camera_eye[1] = pose.eye[1];
    camera_eye[2] = pose.eye[2];

    camera_target[0] = pose.target[0];
    camera_target[1] = pose.target[1];
    camera_target[2] = pose.target[2];

    *camera_valid = 1;
    renderer_set_camera(renderer, &camera);
}