#ifndef BANANA_ENGINE_RUNTIME_TERRAIN_HOST_H
#define BANANA_ENGINE_RUNTIME_TERRAIN_HOST_H

#include "terrain_runtime.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void runtime_terrain_host_mark_chunk_dirty(RuntimeTerrainChunk *chunks,
                                               int chunk_cols,
                                               int chunk_rows,
                                               int chunk_x,
                                               int chunk_z);

    int runtime_terrain_host_rebuild_dirty(unsigned char *height_grid,
                                           int terrain_size,
                                           int chunk_size,
                                           RuntimeTerrainChunk *chunks,
                                           int chunk_cols,
                                           int chunk_rows,
                                           int max_chunks);

#ifdef __cplusplus
}
#endif

#endif
