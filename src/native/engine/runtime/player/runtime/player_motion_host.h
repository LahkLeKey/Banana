#ifndef BANANA_ENGINE_RUNTIME_PLAYER_MOTION_HOST_H
#define BANANA_ENGINE_RUNTIME_PLAYER_MOTION_HOST_H

#include "../motion/player_motion.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_player_motion_tick(World *world,
                                    EntityId primary_player_id,
                                    float move_input_x,
                                    float move_input_z,
                                    const float *camera_eye,
                                    const float *camera_target,
                                    int camera_valid,
                                    float speed,
                                    float dt,
                                    float island_span,
                                    RuntimeTerrainSampleHeightFn sample_height);

#ifdef __cplusplus
}
#endif

#endif
