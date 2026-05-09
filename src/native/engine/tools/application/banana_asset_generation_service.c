#include "banana_asset_generation_service.h"

#include <stdlib.h>

#include "../domain/banana_asset_algorithms.h"
#include "../domain/banana_asset_domain.h"

int banana_generate_terrain_cells(const banana_compiler_config_t *config, int cellular_iterations,
                                  banana_terrain_cell_t *out_cells, uint64_t *out_checksum)
{
    return banana_generate_terrain_cells_with_contract(config, cellular_iterations,
                                                       banana_default_asset_algorithm_contract(),
                                                       out_cells, out_checksum);
}

int banana_generate_terrain_cells_with_contract(const banana_compiler_config_t *config,
                                                int cellular_iterations,
                                                const banana_asset_algorithm_contract_t *contract,
                                                banana_terrain_cell_t *out_cells,
                                                uint64_t *out_checksum)
{
    if (!config || !contract || !contract->generate_tiles || !contract->post_process_tiles ||
        !out_cells || !out_checksum)
    {
        return 0;
    }

    int cell_count = config->width * config->height;
    int *tiles = (int *)malloc((size_t)cell_count * sizeof(int));
    if (!tiles)
    {
        return 0;
    }

    if (!contract->generate_tiles(config->width, config->height, config->seed, tiles))
    {
        free(tiles);
        return 0;
    }

    contract->post_process_tiles(tiles, config->width, config->height, cellular_iterations,
                                 config->seed);

    const banana_tile_def_t *defs = banana_tile_defs();
    uint32_t rng_state = config->seed ^ 0xDEADBEEFu;
    uint64_t checksum = 1469598103934665603ull;

    for (int i = 0; i < cell_count; i++)
    {
        int tile = tiles[i];
        int elevation_noise = (int)(banana_next_random(&rng_state) % 11u) - 5;
        int resource_noise = (int)(banana_next_random(&rng_state) % 7u) - 3;

        out_cells[i].tile_index = tile;
        out_cells[i].elevation = defs[tile].elevation_base + elevation_noise;
        out_cells[i].resource = defs[tile].resource_bias + resource_noise;
        if (out_cells[i].resource < 0)
        {
            out_cells[i].resource = 0;
        }

        checksum ^= (uint64_t)(unsigned int)out_cells[i].tile_index;
        checksum *= 1099511628211ull;
        checksum ^= (uint64_t)(unsigned int)out_cells[i].elevation;
        checksum *= 1099511628211ull;
        checksum ^= (uint64_t)(unsigned int)out_cells[i].resource;
        checksum *= 1099511628211ull;
    }

    *out_checksum = checksum;
    free(tiles);
    return 1;
}
