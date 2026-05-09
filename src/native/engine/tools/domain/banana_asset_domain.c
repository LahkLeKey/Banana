#include "banana_asset_domain.h"

static const banana_tile_def_t BANANA_TILE_DEFS[BANANA_TILE_COUNT] = {
    {"water_deep", 0, 5, 0, 8},
    {"water_shallow", 1, 12, 0, 7},
    {"sand", 2, 20, 1, 6},
    {"grass", 3, 35, 2, 10},
    {"grove", 4, 46, 3, 9},
    {"hill", 5, 64, 2, 6},
    {"ridge", 6, 82, 1, 4},
};

static uint32_t BANANA_TILE_NEIGHBOR_MASKS[BANANA_TILE_COUNT];
static int BANANA_TILE_NEIGHBOR_MASKS_READY = 0;

uint32_t banana_next_random(uint32_t *state) {
  uint32_t x = *state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  *state = x;
  return x;
}

int banana_count_bits_u32(uint32_t value) {
  int count = 0;
  while (value) {
    value &= (value - 1u);
    count++;
  }
  return count;
}

int banana_first_bit_index(uint32_t value) {
  for (int i = 0; i < 32; i++) {
    if (value & (1u << i)) {
      return i;
    }
  }
  return -1;
}

int banana_abs_int(int value) {
  return (value < 0) ? -value : value;
}

const banana_tile_def_t *banana_tile_defs(void) {
  return BANANA_TILE_DEFS;
}

int banana_tile_count(void) {
  return BANANA_TILE_COUNT;
}

void banana_init_neighbor_masks(void) {
  if (BANANA_TILE_NEIGHBOR_MASKS_READY) {
    return;
  }

  for (int i = 0; i < BANANA_TILE_COUNT; i++) {
    uint32_t mask = 0u;
    for (int j = 0; j < BANANA_TILE_COUNT; j++) {
      int delta = banana_abs_int(BANANA_TILE_DEFS[i].biome - BANANA_TILE_DEFS[j].biome);
      if (delta <= 1) {
        mask |= (1u << j);
      }
    }
    BANANA_TILE_NEIGHBOR_MASKS[i] = mask;
  }

  BANANA_TILE_NEIGHBOR_MASKS_READY = 1;
}

uint32_t banana_neighbor_mask_for(int tile_index) {
  banana_init_neighbor_masks();
  if (tile_index < 0 || tile_index >= BANANA_TILE_COUNT) {
    return 0u;
  }
  return BANANA_TILE_NEIGHBOR_MASKS[tile_index];
}

int banana_pick_weighted_tile(uint32_t mask, uint32_t *rng_state) {
  int total_weight = 0;
  for (int tile = 0; tile < BANANA_TILE_COUNT; tile++) {
    if (mask & (1u << tile)) {
      total_weight += BANANA_TILE_DEFS[tile].weight;
    }
  }

  if (total_weight <= 0) {
    return -1;
  }

  uint32_t roll = banana_next_random(rng_state) % (uint32_t)total_weight;
  int cumulative = 0;
  for (int tile = 0; tile < BANANA_TILE_COUNT; tile++) {
    if (mask & (1u << tile)) {
      cumulative += BANANA_TILE_DEFS[tile].weight;
      if ((int)roll < cumulative) {
        return tile;
      }
    }
  }

  return -1;
}

int banana_pick_tile_for_biome(int biome, uint32_t *rng_state) {
  int total_weight = 0;
  for (int i = 0; i < BANANA_TILE_COUNT; i++) {
    if (BANANA_TILE_DEFS[i].biome == biome) {
      total_weight += BANANA_TILE_DEFS[i].weight;
    }
  }

  if (total_weight <= 0) {
    return BANANA_TILE_GRASS;
  }

  uint32_t roll = banana_next_random(rng_state) % (uint32_t)total_weight;
  int cumulative = 0;
  for (int i = 0; i < BANANA_TILE_COUNT; i++) {
    if (BANANA_TILE_DEFS[i].biome != biome) {
      continue;
    }
    cumulative += BANANA_TILE_DEFS[i].weight;
    if ((int)roll < cumulative) {
      return i;
    }
  }

  return BANANA_TILE_GRASS;
}
