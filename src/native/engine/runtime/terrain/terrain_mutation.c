#include "terrain_mutation.h"

static int clampi(int v, int lo, int hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

int runtime_terrain_set_height(unsigned char *height_grid,
                               int terrain_size,
                               int terrain_max_layers,
                               int chunk_size,
                               int x,
                               int z,
                               int elevation,
                               RuntimeTerrainMarkChunkDirtyFn mark_chunk_dirty)
{
    int cx = 0;
    int cz = 0;
    int idx = 0;

    if (!height_grid || !mark_chunk_dirty || terrain_size <= 0 || terrain_max_layers <= 0 ||
        chunk_size <= 0)
        return 0;

    if (x < 0 || z < 0 || x >= terrain_size || z >= terrain_size)
        return 0;

    elevation = clampi(elevation, 0, terrain_max_layers - 1);
    idx = z * terrain_size + x;
    if ((int)height_grid[idx] == elevation)
        return 1;

    height_grid[idx] = (unsigned char)elevation;

    cx = x / chunk_size;
    cz = z / chunk_size;
    mark_chunk_dirty(cx, cz);

    /* Heightfield normals depend on neighboring samples, so update adjacent chunks too. */
    if (x % chunk_size == 0)
        mark_chunk_dirty(cx - 1, cz);
    if (x % chunk_size == chunk_size - 1)
        mark_chunk_dirty(cx + 1, cz);
    if (z % chunk_size == 0)
        mark_chunk_dirty(cx, cz - 1);
    if (z % chunk_size == chunk_size - 1)
        mark_chunk_dirty(cx, cz + 1);

    return 1;
}

void runtime_terrain_mark_region_dirty(int terrain_size,
                                       int chunk_size,
                                       int min_x,
                                       int min_z,
                                       int max_x,
                                       int max_z,
                                       RuntimeTerrainMarkChunkDirtyFn mark_chunk_dirty)
{
    int min_cx = 0;
    int max_cx = 0;
    int min_cz = 0;
    int max_cz = 0;

    if (!mark_chunk_dirty || terrain_size <= 0 || chunk_size <= 0)
        return;

    if (min_x > max_x)
    {
        int t = min_x;
        min_x = max_x;
        max_x = t;
    }
    if (min_z > max_z)
    {
        int t = min_z;
        min_z = max_z;
        max_z = t;
    }

    min_x = clampi(min_x, 0, terrain_size - 1);
    max_x = clampi(max_x, 0, terrain_size - 1);
    min_z = clampi(min_z, 0, terrain_size - 1);
    max_z = clampi(max_z, 0, terrain_size - 1);

    min_cx = min_x / chunk_size;
    max_cx = max_x / chunk_size;
    min_cz = min_z / chunk_size;
    max_cz = max_z / chunk_size;

    for (int cz = min_cz; cz <= max_cz; cz++)
        for (int cx = min_cx; cx <= max_cx; cx++)
            mark_chunk_dirty(cx, cz);
}

int runtime_terrain_apply_delta(unsigned char *height_grid,
                                int terrain_size,
                                int terrain_max_layers,
                                int chunk_size,
                                const RuntimeTerrainDelta *delta,
                                unsigned int *inout_applied_sequence,
                                RuntimeTerrainMarkChunkDirtyFn mark_chunk_dirty)
{
    unsigned int expected_sequence = 0u;

    if (!delta || !inout_applied_sequence)
        return 0;

    expected_sequence = (*inout_applied_sequence) + 1u;
    if (delta->sequence != expected_sequence)
        return 0;

    if (!runtime_terrain_set_height(height_grid,
                                    terrain_size,
                                    terrain_max_layers,
                                    chunk_size,
                                    delta->x,
                                    delta->z,
                                    delta->elevation,
                                    mark_chunk_dirty))
    {
        return 0;
    }

    *inout_applied_sequence = delta->sequence;
    return 1;
}