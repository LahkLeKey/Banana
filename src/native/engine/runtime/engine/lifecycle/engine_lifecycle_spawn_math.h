#ifndef BANANA_ENGINE_RUNTIME_ENGINE_LIFECYCLE_SPAWN_MATH_H
#define BANANA_ENGINE_RUNTIME_ENGINE_LIFECYCLE_SPAWN_MATH_H

#include "engine_lifecycle.h"

#ifdef __cplusplus
extern "C"
{
#endif

unsigned int runtime_engine_lifecycle_spawn_hash(unsigned int value);
float runtime_engine_lifecycle_spawn_jitter(unsigned int seed);
float runtime_engine_lifecycle_spawn_sample_max_ground(RuntimeTerrainSampleFn terrain_sample_height,
                                                       float x,
                                                       float z,
                                                       float sx,
                                                       float sz);

#ifdef __cplusplus
}
#endif

#endif