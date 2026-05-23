#include "camera_follow.h"

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

    if (!renderer || !world || !player_id || viewport_width <= 0 || viewport_height <= 0 ||
        !camera_eye || !camera_target || !camera_valid)
        return;

    player = world_get_entity(world, player_id);
    if (!player || !player->active)
        return;

    camera = camera_create(52.f, (float)viewport_width / (float)viewport_height, 0.1f, 1000.f);
    camera_look_at(&camera,
                   player->position[0],
                   player->position[1] + 14.0f,
                   player->position[2] + 11.0f,
                   player->position[0],
                   player->position[1],
                   player->position[2]);

    camera_eye[0] = player->position[0];
    camera_eye[1] = player->position[1] + 14.0f;
    camera_eye[2] = player->position[2] + 11.0f;

    camera_target[0] = player->position[0];
    camera_target[1] = player->position[1];
    camera_target[2] = player->position[2];

    *camera_valid = 1;
    renderer_set_camera(renderer, &camera);
}