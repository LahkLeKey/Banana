#ifndef BANANA_ENGINE_RUNTIME_TERRAIN_GENERATION_H
#define BANANA_ENGINE_RUNTIME_TERRAIN_GENERATION_H

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_terrain_generate_island(unsigned char *height_grid,
                                        int terrain_size,
                                        int terrain_max_layers);

#ifdef __cplusplus
}
#endif

#endif