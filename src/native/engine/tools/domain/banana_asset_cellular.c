#include "banana_asset_algorithms.h"

#if !defined(BANANA_ENGINE_HAS_OPENMP)
#error "Strict parallel runtime requires OpenMP (BANANA_ENGINE_HAS_OPENMP)."
#endif

#include <stdlib.h>
#include <string.h>

#include "banana_asset_domain.h"

static uint32_t banana_cell_seed(uint32_t seed, int iter, int index)
{
    uint32_t mixed = seed ^ 0xC011A17Au;
    mixed ^= (uint32_t)iter * 2654435761u;
    mixed ^= (uint32_t)index * 2246822519u;
    mixed ^= mixed >> 13;
    mixed *= 1274126177u;
    return mixed ^ (mixed >> 16);
}

void banana_apply_cellular_automata(int *tiles, int width, int height, int iterations,
                                    uint32_t seed)
{
    int should_process = (iterations > 0);

    if (should_process)
    {
        int cell_count = width * height;
        int *scratch = (int *)malloc((size_t)cell_count * sizeof(int));

        if (scratch)
        {
            const banana_tile_def_t *defs = banana_tile_defs();

            for (int iter = 0; iter < iterations; iter++)
            {
                memcpy(scratch, tiles, (size_t)cell_count * sizeof(int));

                {
                    int index = 0;
#pragma omp parallel for schedule(static)
                    for (index = 0; index < cell_count; index++)
                    {
                        int x = index % width;
                        int y = index / width;
                        int biome_votes[BANANA_TILE_COUNT];
                        int current_tile = scratch[index];
                        int current_biome = defs[current_tile].biome;
                        int target_biome = current_biome;
                        int best_votes = 0;
                        uint32_t rng_state = banana_cell_seed(seed, iter, index);

                        for (int i = 0; i < BANANA_TILE_COUNT; i++)
                            biome_votes[i] = 0;

                        for (int oy = -1; oy <= 1; oy++)
                        {
                            for (int ox = -1; ox <= 1; ox++)
                            {
                                if (ox == 0 && oy == 0)
                                    continue;

                                int nx = x + ox;
                                int ny = y + oy;
                                if (nx < 0 || nx >= width || ny < 0 || ny >= height)
                                    continue;

                                int nindex = ny * width + nx;
                                int ntile = scratch[nindex];
                                int nbiome = defs[ntile].biome;
                                biome_votes[nbiome] += 1;
                            }
                        }

                        best_votes = biome_votes[current_biome];
                        for (int biome = 0; biome < BANANA_TILE_COUNT; biome++)
                        {
                            if (biome_votes[biome] > best_votes)
                            {
                                best_votes = biome_votes[biome];
                                target_biome = biome;
                            }
                            else if (biome_votes[biome] == best_votes && biome != target_biome)
                            {
                                if ((banana_next_random(&rng_state) & 1u) == 0u)
                                    target_biome = biome;
                            }
                        }

                        {
                            int delta = target_biome - current_biome;
                            int step = (delta > 1) - (delta < -1);
                            target_biome = current_biome + (step != 0 ? step : delta);
                        }

                        if (target_biome != current_biome)
                            tiles[index] = banana_pick_tile_for_biome(target_biome, &rng_state);
                    }
                }
            }

            free(scratch);
        }
    }
}
