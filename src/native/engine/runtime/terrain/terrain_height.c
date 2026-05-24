#include "terrain_height.h"

#include <math.h>

static int clampi_local(int v, int lo, int hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

float runtime_terrain_sample_height(const unsigned char *height_grid,
                                    int terrain_size,
                                    float x,
                                    float z,
                                    float layer_height,
                                    float world_y_offset)
{
    float world_origin = 0.0f;
    int grid_x = 0;
    int grid_z = 0;

    if (!height_grid || terrain_size <= 0)
        return world_y_offset;

    world_origin = (float)(terrain_size - 1) * 0.5f;
    grid_x = clampi_local((int)roundf(x + world_origin), 0, terrain_size - 1);
    grid_z = clampi_local((int)roundf(z + world_origin), 0, terrain_size - 1);

    return world_y_offset + ((float)height_grid[grid_z * terrain_size + grid_x] * layer_height);
}
