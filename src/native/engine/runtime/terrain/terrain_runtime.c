#include "terrain_runtime.h"

#if !defined(BANANA_ENGINE_HAS_OPENMP) && !defined(__INTELLISENSE__)
#error "Strict parallel runtime requires OpenMP (BANANA_ENGINE_HAS_OPENMP)."
#endif

static int terrain_chunk_stride(int chunk_size)
{
    return chunk_size + 1;
}

static int terrain_chunk_index(int chunk_x, int chunk_z, int chunk_cols)
{
    return chunk_z * chunk_cols + chunk_x;
}

static int terrain_rebuild_chunk(unsigned char *height_grid,
                                 int terrain_size,
                                 int chunk_size,
                                 RuntimeTerrainChunk *chunks,
                                 int chunk_cols,
                                 int chunk_x,
                                 int chunk_z)
{
    int stride = terrain_chunk_stride(chunk_size);
    unsigned char local_heights[16 * 16];
    int base_x = chunk_x * chunk_size;
    int base_z = chunk_z * chunk_size;
    int idx = terrain_chunk_index(chunk_x, chunk_z, chunk_cols);

    if (stride > 16)
        return 0;

    for (int z = 0; z <= chunk_size; z++)
    {
        for (int x = 0; x <= chunk_size; x++)
        {
            int sample_x = base_x + x;
            int sample_z = base_z + z;
            if (sample_x >= terrain_size)
                sample_x = terrain_size - 1;
            if (sample_z >= terrain_size)
                sample_z = terrain_size - 1;
            local_heights[z * stride + x] = height_grid[sample_z * terrain_size + sample_x];
        }
    }

    mesh_destroy(chunks[idx].mesh);
    chunks[idx].mesh = mesh_create_terrain_heightfield(local_heights, stride, stride, 1.0f, 0.30f);
    chunks[idx].dirty = 0u;
    return chunks[idx].mesh ? 1 : 0;
}

void runtime_terrain_mark_all_chunks_dirty(RuntimeTerrainChunk *chunks, int total_chunks)
{
    int i = 0;

    if (!chunks || total_chunks <= 0)
        return;

#pragma omp parallel for schedule(static)
    for (i = 0; i < total_chunks; i++)
        chunks[i].dirty = 1u;
}

int runtime_terrain_rebuild_dirty_chunks(unsigned char *height_grid,
                                         int terrain_size,
                                         int chunk_size,
                                         RuntimeTerrainChunk *chunks,
                                         int chunk_cols,
                                         int chunk_rows,
                                         int max_chunks)
{
    int rebuilt = 0;

    if (!height_grid || !chunks || terrain_size <= 0 || chunk_size <= 0 || chunk_cols <= 0 ||
        chunk_rows <= 0)
        return 0;

    if (max_chunks == 0)
    {
        int total_chunks = chunk_cols * chunk_rows;
        int failed = 0;
        int index = 0;

#pragma omp parallel for schedule(dynamic, 1) reduction(+ : rebuilt) reduction(| : failed)
        for (index = 0; index < total_chunks; index++)
        {
            int cz = index / chunk_cols;
            int cx = index % chunk_cols;
            int idx = terrain_chunk_index(cx, cz, chunk_cols);
            if (!chunks[idx].dirty)
                continue;

            if (!terrain_rebuild_chunk(height_grid,
                                       terrain_size,
                                       chunk_size,
                                       chunks,
                                       chunk_cols,
                                       cx,
                                       cz))
            {
                failed |= 1;
                continue;
            }
            rebuilt++;
        }

        return failed ? 0 : 1;
    }

    for (int cz = 0; cz < chunk_rows; cz++)
    {
        for (int cx = 0; cx < chunk_cols; cx++)
        {
            int idx = terrain_chunk_index(cx, cz, chunk_cols);
            if (!chunks[idx].dirty)
                continue;
            if (!terrain_rebuild_chunk(height_grid,
                                       terrain_size,
                                       chunk_size,
                                       chunks,
                                       chunk_cols,
                                       cx,
                                       cz))
                return 0;
            rebuilt++;
            if (max_chunks > 0 && rebuilt >= max_chunks)
                return 1;
        }
    }
    return 1;
}

void runtime_terrain_draw(Renderer *renderer,
                          const RuntimeTerrainChunk *chunks,
                          int terrain_initialized,
                          int terrain_size,
                          int chunk_size,
                          int chunk_cols,
                          int chunk_rows)
{
    float identity[4] = {0.f, 0.f, 0.f, 1.f};
    float unit[3] = {1.f, 0.48f, 1.f};
    Material terrain_mat = material_solid(0.35f, 0.66f, 0.30f, 1.f);
    float world_origin = 0.f;
    float chunk_center_offset = 0.f;

    if (!renderer || !chunks || !terrain_initialized)
        return;

    terrain_mat.use_texture = 1.0f;
    terrain_mat.uv_scale = 8.0f;
    world_origin = (float)(terrain_size - 1) * 0.5f;
    chunk_center_offset = ((float)chunk_size * 0.5f) - 0.5f;

    for (int cz = 0; cz < chunk_rows; cz++)
    {
        for (int cx = 0; cx < chunk_cols; cx++)
        {
            int idx = terrain_chunk_index(cx, cz, chunk_cols);
            Mesh *mesh = chunks[idx].mesh;
            float pos[3];

            if (!mesh)
                continue;

            pos[0] = (float)(cx * chunk_size) - world_origin + chunk_center_offset;
            pos[1] = -1.45f;
            pos[2] = (float)(cz * chunk_size) - world_origin + chunk_center_offset;

            renderer_draw_mesh(renderer, mesh, pos, identity, unit, &terrain_mat);
        }
    }
}
