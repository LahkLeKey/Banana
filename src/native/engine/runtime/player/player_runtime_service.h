#ifndef BANANA_ENGINE_RUNTIME_PLAYER_RUNTIME_SERVICE_H
#define BANANA_ENGINE_RUNTIME_PLAYER_RUNTIME_SERVICE_H

#include "../move_target_domain.h"
#include "player_motion.h"

#include "../../render/renderer.h"
#include "../../world/world.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_player_runtime_update_motion(World *world,
                                              EntityId player_id,
                                              RuntimeMoveTargetState *move_target_state,
                                              float *move_input_x,
                                              float *move_input_z,
                                              const float *camera_eye,
                                              const float *camera_target,
                                              int camera_valid,
                                              float speed,
                                              float dt,
                                              float island_span,
                                              RuntimeTerrainSampleHeightFn sample_height);

    void runtime_player_runtime_follow_camera(Renderer *renderer,
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
