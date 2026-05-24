#include "mesh.h"

#include <math.h>
#include <stdlib.h>

Mesh *mesh_create_terrain_heightfield(const unsigned char *heights, int width, int depth,
                                      float tile_size, float height_scale)
{
    if (!heights || width < 2 || depth < 2 || tile_size <= 0.0f)
        return NULL;

    const int vertex_count = width * depth;
    const int index_count = (width - 1) * (depth - 1) * 6;

    float *verts = (float *)malloc((size_t)vertex_count * 8 * sizeof(float));
    unsigned int *indices = (unsigned int *)malloc((size_t)index_count * sizeof(unsigned int));
    if (!verts || !indices)
    {
        free(verts);
        free(indices);
        return NULL;
    }

    const float x_origin = ((float)(width - 1) * tile_size) * 0.5f;
    const float z_origin = ((float)(depth - 1) * tile_size) * 0.5f;

    {
        int idx = 0;
#if defined(BANANA_ENGINE_HAS_OPENMP)
#pragma omp parallel for schedule(static)
#endif
        for (idx = 0; idx < vertex_count; idx++)
        {
            int z = idx / width;
            int x = idx % width;

            int xl = x > 0 ? x - 1 : x;
            int xr = x + 1 < width ? x + 1 : x;
            int zd = z > 0 ? z - 1 : z;
            int zu = z + 1 < depth ? z + 1 : z;

            float h = (float)heights[idx] * height_scale;
            float hl = (float)heights[z * width + xl] * height_scale;
            float hr = (float)heights[z * width + xr] * height_scale;
            float hd = (float)heights[zd * width + x] * height_scale;
            float hu = (float)heights[zu * width + x] * height_scale;

            float nx = hl - hr;
            float ny = tile_size * 2.0f;
            float nz = hd - hu;
            float len = sqrtf(nx * nx + ny * ny + nz * nz);
            if (len < 1e-5f)
                len = 1.0f;
            nx /= len;
            ny /= len;
            nz /= len;

            int base = idx * 8;
            verts[base + 0] = (float)x * tile_size - x_origin;
            verts[base + 1] = h;
            verts[base + 2] = (float)z * tile_size - z_origin;
            verts[base + 3] = nx;
            verts[base + 4] = ny;
            verts[base + 5] = nz;
            verts[base + 6] = (float)x / (float)(width - 1);
            verts[base + 7] = (float)z / (float)(depth - 1);
        }
    }

    {
        int cell_count = (width - 1) * (depth - 1);
        int cell = 0;
#if defined(BANANA_ENGINE_HAS_OPENMP)
#pragma omp parallel for schedule(static)
#endif
        for (cell = 0; cell < cell_count; cell++)
        {
            int z = cell / (width - 1);
            int x = cell % (width - 1);

            unsigned int a = (unsigned int)(z * width + x);
            unsigned int b = a + 1u;
            unsigned int c = (unsigned int)((z + 1) * width + x);
            unsigned int d = c + 1u;

            int base = cell * 6;
            indices[base + 0] = a;
            indices[base + 1] = c;
            indices[base + 2] = b;
            indices[base + 3] = b;
            indices[base + 4] = c;
            indices[base + 5] = d;
        }
    }

    Mesh *m = mesh_create(verts, vertex_count, indices, index_count);
    free(verts);
    free(indices);
    return m;
}
