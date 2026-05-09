#ifndef BANANA_ASSET_TYPES_H
#define BANANA_ASSET_TYPES_H

#include <stdint.h>

typedef struct
{
    uint32_t seed;
    const char *profile;
    const char *out_dir;
    int width;
    int height;
} banana_compiler_config_t;

typedef struct
{
    int tile_index;
    int elevation;
    int resource;
} banana_terrain_cell_t;

#endif
