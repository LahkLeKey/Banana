#include "terrain_height.h"

#include <math.h>

float runtime_terrain_sample_height(const unsigned char *height_grid,
                                    int terrain_size,
                                    float x,
                                    float z,
                                    float layer_height,
                                    float world_y_offset)
{
    static const unsigned char fallback_height_cell = 0;
    const unsigned char *effective_grid = &fallback_height_cell;
    int has_valid_grid = 0;
    int effective_terrain_size = 1;
    int max_index = 0;
    float world_origin = 0.0f;
    float grid_xf = 0.0f;
    float grid_zf = 0.0f;
    float sampled_height = 0.0f;
    int grid_x = 0;
    int grid_z = 0;

    has_valid_grid = (height_grid != NULL && terrain_size > 0);
    effective_grid = has_valid_grid ? height_grid : &fallback_height_cell;
    effective_terrain_size = has_valid_grid ? terrain_size : 1;
    max_index = effective_terrain_size - 1;

    world_origin = (float)max_index * 0.5f;
    grid_xf = roundf(x + world_origin);
    grid_zf = roundf(z + world_origin);

    grid_xf = fminf((float)max_index, fmaxf(0.0f, grid_xf));
    grid_zf = fminf((float)max_index, fmaxf(0.0f, grid_zf));

    grid_x = (int)grid_xf;
    grid_z = (int)grid_zf;
    sampled_height = has_valid_grid
                         ? (float)effective_grid[grid_z * effective_terrain_size + grid_x]
                         : 0.0f;

    return world_y_offset + (sampled_height * layer_height);
}
