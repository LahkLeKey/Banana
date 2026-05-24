#ifndef BANANA_ENGINE_RUNTIME_CAMERA_FOLLOW_H
#define BANANA_ENGINE_RUNTIME_CAMERA_FOLLOW_H

#include "../../render/renderer.h"
#include "../../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_camera_follow_player(Renderer *renderer,
                                      World *world,
                                      EntityId player_id,
                                      int viewport_width,
                                      int viewport_height,
                                      float *camera_eye,
                                      float *camera_target,
                                      int *camera_valid);

#ifdef __cplusplus
}
#endif

#endif