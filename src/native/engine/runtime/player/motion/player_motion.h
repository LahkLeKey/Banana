#ifndef BANANA_ENGINE_RUNTIME_PLAYER_MOTION_H
#define BANANA_ENGINE_RUNTIME_PLAYER_MOTION_H

#include "../../../world/world.h"
#include "../player_registry.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef float (*RuntimeTerrainSampleHeightFn)(float x, float z);

    void runtime_player_motion_apply(World *world,
                                     NativePlayerBinding **bindings,
                                     int binding_count,
                                     EntityId primary_player_id,
                                     float move_input_x,
                                     float move_input_z,
                                     const float *forward,
                                     const float *right,
                                     float speed,
                                     float dt,
                                     float island_span,
                                     RuntimeTerrainSampleHeightFn sample_height);

#ifdef __cplusplus
}
#endif

#endif