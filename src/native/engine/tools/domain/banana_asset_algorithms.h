#ifndef BANANA_ASSET_ALGORITHMS_H
#define BANANA_ASSET_ALGORITHMS_H

#include <stdint.h>

typedef int (*banana_generate_tiles_fn)(int width, int height, uint32_t seed, int *out_tiles);
typedef void (*banana_post_process_tiles_fn)(
		int *tiles,
		int width,
		int height,
		int iterations,
		uint32_t seed);

typedef struct {
	const char *tile_generation_name;
	const char *post_process_name;
	banana_generate_tiles_fn generate_tiles;
	banana_post_process_tiles_fn post_process_tiles;
} banana_asset_algorithm_contract_t;

int banana_generate_tiles_wfc(int width, int height, uint32_t seed, int *out_tiles);
void banana_apply_cellular_automata(int *tiles, int width, int height, int iterations, uint32_t seed);
const banana_asset_algorithm_contract_t *banana_default_asset_algorithm_contract(void);

#endif
