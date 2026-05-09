#ifndef BANANA_ASSET_GENERATION_SERVICE_H
#define BANANA_ASSET_GENERATION_SERVICE_H

#include <stdint.h>

#include "../banana_asset_types.h"
#include "../domain/banana_asset_algorithms.h"

int banana_generate_terrain_cells(
    const banana_compiler_config_t *config,
    int cellular_iterations,
    banana_terrain_cell_t *out_cells,
    uint64_t *out_checksum);
int banana_generate_terrain_cells_with_contract(
    const banana_compiler_config_t *config,
    int cellular_iterations,
    const banana_asset_algorithm_contract_t *contract,
    banana_terrain_cell_t *out_cells,
    uint64_t *out_checksum);

#endif
