#include "terrain_host.h"

static int runtime_terrain_host_chunk_index(int chunk_x, int chunk_z, int chunk_cols)
{
    return chunk_z * chunk_cols + chunk_x;
}

void runtime_terrain_host_mark_chunk_dirty(RuntimeTerrainChunk *chunks,
                                           int chunk_cols,
                                           int chunk_rows,
                                           int chunk_x,
                                           int chunk_z)
{
    if (!chunks || chunk_cols <= 0 || chunk_rows <= 0)
        return;

    if (chunk_x < 0 || chunk_z < 0 || chunk_x >= chunk_cols || chunk_z >= chunk_rows)
        return;

    chunks[runtime_terrain_host_chunk_index(chunk_x, chunk_z, chunk_cols)].dirty = 1u;
}

int runtime_terrain_host_rebuild_dirty(unsigned char *height_grid,
                                       int terrain_size,
                                       int chunk_size,
                                       RuntimeTerrainChunk *chunks,
                                       int chunk_cols,
                                       int chunk_rows,
                                       int max_chunks)
{
    return runtime_terrain_rebuild_dirty_chunks(height_grid,
                                                terrain_size,
                                                chunk_size,
                                                chunks,
                                                chunk_cols,
                                                chunk_rows,
                                                max_chunks);
}

RuntimeTerrainFailureClass runtime_terrain_host_classify_generation_failure(int status_code)
{
    if (status_code == RUNTIME_TERRAIN_GENERATION_STATUS_OK)
        return RUNTIME_TERRAIN_FAILURE_CLASS_NONE;

    switch (status_code)
    {
        case RUNTIME_TERRAIN_GENERATION_STATUS_TIMEOUT:
        case RUNTIME_TERRAIN_GENERATION_STATUS_DEPENDENCY_UNAVAILABLE:
            return RUNTIME_TERRAIN_FAILURE_CLASS_RECOVERABLE;

        case RUNTIME_TERRAIN_GENERATION_STATUS_INVALID_AREA_IDENTITY:
        case RUNTIME_TERRAIN_GENERATION_STATUS_CONTRACT_VERSION_UNSUPPORTED:
        default:
            return RUNTIME_TERRAIN_FAILURE_CLASS_NON_RECOVERABLE;
    }
}
