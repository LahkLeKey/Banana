#include "engine_lifecycle_spawn_math.h"

unsigned int runtime_engine_lifecycle_spawn_hash(unsigned int value)
{
    unsigned int n = value * 374761393u + 20260523u;
    n = (n ^ (n >> 13)) * 1274126177u;
    return n ^ (n >> 16);
}

float runtime_engine_lifecycle_spawn_jitter(unsigned int seed)
{
    return (((float)(seed & 0xFFu) / 255.0f) - 0.5f) * 2.4f;
}

/* Sample center + footprint corners and return the highest ground point.
   This avoids actor corners clipping out of sloped terrain near map edges. */
float runtime_engine_lifecycle_spawn_sample_max_ground(RuntimeTerrainSampleFn terrain_sample_height,
                                                       float x,
                                                       float z,
                                                       float sx,
                                                       float sz)
{
    float half_x = sx * 0.5f;
    float half_z = sz * 0.5f;
    float max_h = terrain_sample_height(x, z);
    float h = 0.0f;

    h = terrain_sample_height(x - half_x, z - half_z);
    if (h > max_h)
        max_h = h;
    h = terrain_sample_height(x + half_x, z - half_z);
    if (h > max_h)
        max_h = h;
    h = terrain_sample_height(x - half_x, z + half_z);
    if (h > max_h)
        max_h = h;
    h = terrain_sample_height(x + half_x, z + half_z);
    if (h > max_h)
        max_h = h;

    return max_h;
}