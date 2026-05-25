#ifndef BANANA_ENGINE_RUNTIME_TERRAIN_RUNTIME_H
#define BANANA_ENGINE_RUNTIME_TERRAIN_RUNTIME_H

#include "../../render/material.h"
#include "../../render/mesh.h"
#include "../../render/renderer.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RuntimeTerrainChunk
    {
        Mesh *mesh;
        unsigned char dirty;
        uint64_t baseline_signature;
    } RuntimeTerrainChunk;

    void runtime_terrain_mark_all_chunks_dirty(RuntimeTerrainChunk *chunks, int total_chunks);

    int runtime_terrain_rebuild_dirty_chunks(unsigned char *height_grid,
                                             int terrain_size,
                                             int chunk_size,
                                             RuntimeTerrainChunk *chunks,
                                             int chunk_cols,
                                             int chunk_rows,
                                             int max_chunks);

    void runtime_terrain_draw(Renderer *renderer,
                              const RuntimeTerrainChunk *chunks,
                              int terrain_initialized,
                              int terrain_size,
                              int chunk_size,
                              int chunk_cols,
                              int chunk_rows);

#ifdef __cplusplus
}
#endif

#endif
