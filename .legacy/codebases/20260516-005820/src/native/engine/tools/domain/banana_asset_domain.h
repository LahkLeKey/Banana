#ifndef BANANA_ASSET_DOMAIN_H
#define BANANA_ASSET_DOMAIN_H

#include <stdint.h>

typedef struct
{
    const char *id;
    int biome;
    int elevation_base;
    int resource_bias;
    int weight;
    const char *display_color_role;
    const char *display_color_hex;
} banana_tile_def_t;

enum
{
    BANANA_TILE_WATER_DEEP = 0,
    BANANA_TILE_WATER_SHALLOW = 1,
    BANANA_TILE_SAND = 2,
    BANANA_TILE_GRASS = 3,
    BANANA_TILE_GROVE = 4,
    BANANA_TILE_HILL = 5,
    BANANA_TILE_RIDGE = 6,
    BANANA_TILE_COUNT = 7,
};

uint32_t banana_next_random(uint32_t *state);
int banana_count_bits_u32(uint32_t value);
int banana_first_bit_index(uint32_t value);
int banana_abs_int(int value);

const banana_tile_def_t *banana_tile_defs(void);
int banana_tile_count(void);
void banana_init_neighbor_masks(void);
uint32_t banana_neighbor_mask_for(int tile_index);

int banana_pick_weighted_tile(uint32_t mask, uint32_t *rng_state);
int banana_pick_tile_for_biome(int biome, uint32_t *rng_state);

#endif
