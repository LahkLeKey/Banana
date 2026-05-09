#ifndef BANANA_ASSET_JSON_WRITER_H
#define BANANA_ASSET_JSON_WRITER_H

#include <stdint.h>

#include "../banana_asset_types.h"

int banana_write_palette_json(const char *path);
int banana_write_terrain_json(
    const char *path,
    const banana_compiler_config_t *config,
    const banana_terrain_cell_t *cells,
    uint64_t checksum,
    int cellular_iterations);
int banana_write_manifest_json(
    const char *path,
    const banana_compiler_config_t *config,
    uint64_t checksum,
    int cellular_iterations);

#endif
