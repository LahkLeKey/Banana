#ifndef BANANA_ENGINE_RUNTIME_TERRAIN_HEIGHT_H
#define BANANA_ENGINE_RUNTIME_TERRAIN_HEIGHT_H

#ifdef __cplusplus
extern "C"
{
#endif

    float runtime_terrain_sample_height(const unsigned char *height_grid,
                                        int terrain_size,
                                        float x,
                                        float z,
                                        float layer_height,
                                        float world_y_offset);

#ifdef __cplusplus
}
#endif

#endif
