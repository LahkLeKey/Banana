#ifndef BANANA_ENGINE_RUNTIME_TERRAIN_MUTATION_H
#define BANANA_ENGINE_RUNTIME_TERRAIN_MUTATION_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*RuntimeTerrainMarkChunkDirtyFn)(int chunk_x, int chunk_z);

    typedef struct RuntimeTerrainDelta
    {
        int x;
        int z;
        int elevation;
        unsigned int sequence;
    } RuntimeTerrainDelta;

    int runtime_terrain_set_height(unsigned char *height_grid,
                                   int terrain_size,
                                   int terrain_max_layers,
                                   int chunk_size,
                                   int x,
                                   int z,
                                   int elevation,
                                   RuntimeTerrainMarkChunkDirtyFn mark_chunk_dirty);

    void runtime_terrain_mark_region_dirty(int terrain_size,
                                           int chunk_size,
                                           int min_x,
                                           int min_z,
                                           int max_x,
                                           int max_z,
                                           RuntimeTerrainMarkChunkDirtyFn mark_chunk_dirty);

    int runtime_terrain_apply_delta(unsigned char *height_grid,
                                    int terrain_size,
                                    int terrain_max_layers,
                                    int chunk_size,
                                    const RuntimeTerrainDelta *delta,
                                    unsigned int *inout_applied_sequence,
                                    RuntimeTerrainMarkChunkDirtyFn mark_chunk_dirty);

#ifdef __cplusplus
}
#endif

#endif