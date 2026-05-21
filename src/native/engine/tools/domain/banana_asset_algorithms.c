#include "banana_asset_algorithms.h"

static const banana_asset_algorithm_contract_t BANANA_DEFAULT_ASSET_ALGORITHM_CONTRACT = {
    .tile_generation_name = "wave-function-collapse",
    .post_process_name = "cellular-automata",
    .generate_tiles = banana_generate_tiles_wfc,
    .post_process_tiles = banana_apply_cellular_automata,
};

const banana_asset_algorithm_contract_t *banana_default_asset_algorithm_contract(void)
{
    return &BANANA_DEFAULT_ASSET_ALGORITHM_CONTRACT;
}
