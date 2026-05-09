#include "banana_asset_algorithms.h"

#include <stdlib.h>
#include <string.h>

#include "banana_asset_domain.h"

static const banana_asset_algorithm_contract_t BANANA_DEFAULT_ASSET_ALGORITHM_CONTRACT = {
  .tile_generation_name = "wave-function-collapse",
  .post_process_name = "cellular-automata",
  .generate_tiles = banana_generate_tiles_wfc,
  .post_process_tiles = banana_apply_cellular_automata,
};

static int banana_pick_lowest_entropy_cell(const uint32_t *wave, int cell_count, uint32_t *rng_state) {
  int best_index = -1;
  int best_entropy = 32;

  for (int i = 0; i < cell_count; i++) {
    int entropy = banana_count_bits_u32(wave[i]);
    if (entropy <= 1) {
      continue;
    }

    if (entropy < best_entropy) {
      best_entropy = entropy;
      best_index = i;
      continue;
    }

    if (entropy == best_entropy && best_index >= 0) {
      if ((banana_next_random(rng_state) & 1u) == 0u) {
        best_index = i;
      }
    }
  }

  return best_index;
}

static int banana_propagate(int width, int height, uint32_t *wave, int start_cell) {
  int cell_count = width * height;
  int *queue = (int *)malloc((size_t)cell_count * sizeof(int));
  if (!queue) {
    return 0;
  }

  int head = 0;
  int tail = 0;
  queue[tail++] = start_cell;

  while (head < tail) {
    int cell = queue[head++];
    int x = cell % width;
    int y = cell / width;
    uint32_t options = wave[cell];

    uint32_t allowed_neighbors = 0u;
    for (int tile = 0; tile < banana_tile_count(); tile++) {
      if (options & (1u << tile)) {
        allowed_neighbors |= banana_neighbor_mask_for(tile);
      }
    }

    const int offsets[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (int i = 0; i < 4; i++) {
      int nx = x + offsets[i][0];
      int ny = y + offsets[i][1];
      if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
        continue;
      }

      int nindex = ny * width + nx;
      uint32_t previous = wave[nindex];
      uint32_t constrained = previous & allowed_neighbors;

      if (constrained == 0u) {
        free(queue);
        return 0;
      }

      if (constrained != previous) {
        wave[nindex] = constrained;
        queue[tail++] = nindex;
      }
    }
  }

  free(queue);
  return 1;
}

int banana_generate_tiles_wfc(int width, int height, uint32_t seed, int *out_tiles) {
  banana_init_neighbor_masks();

  int cell_count = width * height;
  uint32_t *wave = (uint32_t *)malloc((size_t)cell_count * sizeof(uint32_t));
  if (!wave) {
    return 0;
  }

  uint32_t full_mask = (1u << banana_tile_count()) - 1u;
  for (int i = 0; i < cell_count; i++) {
    wave[i] = full_mask;
  }

  uint32_t rng_state = seed ^ 0xBADC0DEu;
  int iterations = 0;
  int max_iterations = cell_count * 8;

  while (iterations < max_iterations) {
    iterations++;
    int cell = banana_pick_lowest_entropy_cell(wave, cell_count, &rng_state);
    if (cell < 0) {
      break;
    }

    uint32_t options = wave[cell];
    int chosen = banana_pick_weighted_tile(options, &rng_state);
    if (chosen < 0) {
      free(wave);
      return 0;
    }

    wave[cell] = (1u << chosen);
    if (!banana_propagate(width, height, wave, cell)) {
      free(wave);
      return 0;
    }
  }

  for (int i = 0; i < cell_count; i++) {
    int tile = banana_first_bit_index(wave[i]);
    if (tile < 0) {
      tile = BANANA_TILE_GRASS;
    }
    out_tiles[i] = tile;
  }

  free(wave);
  return 1;
}

void banana_apply_cellular_automata(int *tiles, int width, int height, int iterations, uint32_t seed) {
  if (iterations <= 0) {
    return;
  }

  int cell_count = width * height;
  int *scratch = (int *)malloc((size_t)cell_count * sizeof(int));
  if (!scratch) {
    return;
  }

  uint32_t rng_state = seed ^ 0xC011A17Au;
  const banana_tile_def_t *defs = banana_tile_defs();

  for (int iter = 0; iter < iterations; iter++) {
    memcpy(scratch, tiles, (size_t)cell_count * sizeof(int));

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        int index = y * width + x;
        int biome_votes[BANANA_TILE_COUNT];
        for (int i = 0; i < BANANA_TILE_COUNT; i++) {
          biome_votes[i] = 0;
        }

        for (int oy = -1; oy <= 1; oy++) {
          for (int ox = -1; ox <= 1; ox++) {
            if (ox == 0 && oy == 0) {
              continue;
            }

            int nx = x + ox;
            int ny = y + oy;
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
              continue;
            }

            int nindex = ny * width + nx;
            int ntile = scratch[nindex];
            int nbiome = defs[ntile].biome;
            biome_votes[nbiome] += 1;
          }
        }

        int current_tile = scratch[index];
        int current_biome = defs[current_tile].biome;
        int target_biome = current_biome;
        int best_votes = biome_votes[current_biome];

        for (int biome = 0; biome < BANANA_TILE_COUNT; biome++) {
          if (biome_votes[biome] > best_votes) {
            best_votes = biome_votes[biome];
            target_biome = biome;
          } else if (biome_votes[biome] == best_votes && biome != target_biome) {
            if ((banana_next_random(&rng_state) & 1u) == 0u) {
              target_biome = biome;
            }
          }
        }

        if (banana_abs_int(target_biome - current_biome) > 1) {
          if (target_biome > current_biome) {
            target_biome = current_biome + 1;
          } else {
            target_biome = current_biome - 1;
          }
        }

        if (target_biome != current_biome) {
          tiles[index] = banana_pick_tile_for_biome(target_biome, &rng_state);
        }
      }
    }
  }

  free(scratch);
}

const banana_asset_algorithm_contract_t *banana_default_asset_algorithm_contract(void) {
  return &BANANA_DEFAULT_ASSET_ALGORITHM_CONTRACT;
}
