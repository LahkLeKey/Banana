#ifndef BANANA_ENGINE_RUNTIME_TERRAIN_HOST_H
#define BANANA_ENGINE_RUNTIME_TERRAIN_HOST_H

#include "terrain_runtime.h"
#include "terrain_generation.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum RuntimeTerrainFailureClass
    {
        RUNTIME_TERRAIN_FAILURE_CLASS_NONE = 0,
        RUNTIME_TERRAIN_FAILURE_CLASS_RECOVERABLE = 1,
        RUNTIME_TERRAIN_FAILURE_CLASS_NON_RECOVERABLE = 2,
    } RuntimeTerrainFailureClass;

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

    RuntimeTerrainFailureClass runtime_terrain_host_classify_generation_failure(int status_code);

#ifdef __cplusplus
}
#endif

#endif
