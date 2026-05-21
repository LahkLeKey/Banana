#include "wfc_worldgen_service.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static unsigned int wfc_hash(unsigned int x, unsigned int y, unsigned int seed)
{
    unsigned int n = (x * 374761393u) ^ (y * 668265263u) ^ (seed * 362437u);
    n = (n ^ (n >> 13)) * 1274126177u;
    return n ^ (n >> 16);
}

static float wfc_rand01(unsigned int x, unsigned int y, unsigned int seed, unsigned int salt)
{
    unsigned int h = wfc_hash(x + salt * 11u, y + salt * 29u, seed);
    return (float)(h & 0xFFFFu) / 65535.0f;
}

static int clampi_wfc(int v, int lo, int hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

void engine_wfc_generate_heightfield(unsigned char *out_height, int width, int height,
                                     int max_layers, unsigned int seed)
{
    if (!out_height || width <= 0 || height <= 0 || max_layers <= 1)
        return;

    {
        const int cells = width * height;
        const int relax_iterations = 4;
        float cx = (float)(width - 1) * 0.5f;
        float cy = (float)(height - 1) * 0.5f;
        float playable_radius = ((cx < cy) ? cx : cy) * 1.12f;
        unsigned char *base = (unsigned char *)malloc((size_t)cells);
        unsigned char *scratch = (unsigned char *)malloc((size_t)cells);
        unsigned char *read_buf = NULL;
        unsigned char *write_buf = NULL;

        if (!base || !scratch)
        {
            free(base);
            free(scratch);
            return;
        }

        /* Phase 1: independent candidate synthesis (parallel-friendly). */
#ifdef _OPENMP
#pragma omp parallel for collapse(2)
#endif
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                float nx = ((float)x - cx) / playable_radius;
                float ny = ((float)y - cy) / playable_radius;
                float radial = sqrtf(nx * nx + ny * ny);
                float coast = 1.0f - ((radial - 0.58f) / 0.42f);

                if (coast < 0.0f)
                    coast = 0.0f;
                if (coast > 1.0f)
                    coast = 1.0f;

                float n1 = wfc_rand01((unsigned int)x, (unsigned int)y, seed, 1u);
                float n2 = wfc_rand01((unsigned int)(x * 2), (unsigned int)(y * 2), seed, 3u);
                float n3 = wfc_rand01((unsigned int)(x * 4), (unsigned int)(y * 4), seed, 7u);
                float signal = (n1 * 0.56f) + (n2 * 0.30f) + (n3 * 0.14f);
                float normalized = (signal * 0.85f + coast * 0.95f) * coast;
                int base_layer = 0;

                if (normalized < 0.0f)
                    normalized = 0.0f;
                if (normalized > 1.0f)
                    normalized = 1.0f;

                base_layer = (int)floorf(normalized * (float)(max_layers - 1));
                base[y * width + x] = (unsigned char)clampi_wfc(base_layer, 0, max_layers - 1);
            }
        }

        memcpy(out_height, base, (size_t)cells);
        read_buf = out_height;
        write_buf = scratch;

        /* Phase 2: compatibility relaxation (Jacobi-style, still parallel-safe). */
        for (int iter = 0; iter < relax_iterations; iter++)
        {
#ifdef _OPENMP
#pragma omp parallel for collapse(2)
#endif
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int idx = y * width + x;
                    int min_allowed = 0;
                    int max_allowed = max_layers - 1;

                    for (int oy = -1; oy <= 1; oy++)
                    {
                        for (int ox = -1; ox <= 1; ox++)
                        {
                            int nx = x + ox;
                            int ny = y + oy;
                            int neigh = 0;
                            if ((ox == 0 && oy == 0) || nx < 0 || ny < 0 || nx >= width ||
                                ny >= height)
                                continue;

                            neigh = (int)read_buf[ny * width + nx];
                            if (neigh - 1 > min_allowed)
                                min_allowed = neigh - 1;
                            if (neigh + 1 < max_allowed)
                                max_allowed = neigh + 1;
                        }
                    }

                    if (min_allowed > max_allowed)
                        min_allowed = max_allowed;

                    write_buf[idx] =
                        (unsigned char)clampi_wfc((int)base[idx], min_allowed, max_allowed);
                }
            }

            {
                unsigned char *tmp = read_buf;
                read_buf = write_buf;
                write_buf = tmp;
            }
        }

        if (read_buf != out_height)
            memcpy(out_height, read_buf, (size_t)cells);

        free(base);
        free(scratch);
    }
}