#include "cellular_worldgen_service.h"
#include <stdlib.h>

static int clampi_cell(int v, int lo, int hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

void engine_cellular_smooth_heightfield(unsigned char *heightfield, int width, int height,
                                        int max_layers, int iterations)
{
    if (!heightfield || width <= 0 || height <= 0 || max_layers <= 1 || iterations <= 0)
        return;

    {
        int cells = width * height;
        unsigned char *scratch = (unsigned char *)malloc((size_t)cells);
        if (!scratch)
            return;

        for (int iter = 0; iter < iterations; iter++)
        {
#ifdef _OPENMP
#pragma omp parallel for collapse(2)
#endif
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int sum = 0;
                    int count = 0;

                    for (int oy = -1; oy <= 1; oy++)
                    {
                        int sy = y + oy;
                        if (sy < 0 || sy >= height)
                            continue;
                        for (int ox = -1; ox <= 1; ox++)
                        {
                            int sx = x + ox;
                            if (sx < 0 || sx >= width)
                                continue;
                            sum += (int)heightfield[sy * width + sx];
                            count++;
                        }
                    }

                    {
                        int current = (int)heightfield[y * width + x];
                        int avg = (count > 0) ? ((sum + (count / 2)) / count) : current;
                        int blended = (current * 2 + avg) / 3;
                        scratch[y * width + x] =
                            (unsigned char)clampi_cell(blended, 0, max_layers - 1);
                    }
                }
            }

            for (int i = 0; i < cells; i++)
                heightfield[i] = scratch[i];
        }

        free(scratch);
    }
}