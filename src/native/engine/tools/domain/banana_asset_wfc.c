#include "banana_asset_algorithms.h"

#if !defined(BANANA_ENGINE_HAS_OPENMP)
#error "Strict parallel runtime requires OpenMP (BANANA_ENGINE_HAS_OPENMP)."
#endif

#include <stdlib.h>

#include "banana_asset_domain.h"

static int banana_pick_lowest_entropy_cell(const uint32_t *wave, int cell_count,
                                           uint32_t *rng_state)
{
    int best_index = -1;
    int best_entropy = 32;

    for (int i = 0; i < cell_count; i++)
    {
        int entropy = banana_count_bits_u32(wave[i]);
        if (entropy <= 1)
            continue;

        if (entropy < best_entropy)
        {
            best_entropy = entropy;
            best_index = i;
            continue;
        }

        if (entropy == best_entropy && best_index >= 0)
        {
            if ((banana_next_random(rng_state) & 1u) == 0u)
                best_index = i;
        }
    }

    return best_index;
}

static int banana_propagate(int width, int height, uint32_t *wave, int start_cell)
{
    int success = 1;
    int cell_count = width * height;
    int *queue = (int *)malloc((size_t)cell_count * sizeof(int));

    success = (queue != NULL);
    if (success)
    {
        int head = 0;
        int tail = 0;
        queue[tail++] = start_cell;

        while (head < tail && success)
        {
            int cell = queue[head++];
            int x = cell % width;
            int y = cell / width;
            uint32_t options = wave[cell];

            uint32_t allowed_neighbors = 0u;
            for (int tile = 0; tile < banana_tile_count(); tile++)
            {
                if (options & (1u << tile))
                    allowed_neighbors |= banana_neighbor_mask_for(tile);
            }

            const int offsets[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
            for (int i = 0; i < 4 && success; i++)
            {
                int nx = x + offsets[i][0];
                int ny = y + offsets[i][1];
                if (nx < 0 || nx >= width || ny < 0 || ny >= height)
                    continue;

                int nindex = ny * width + nx;
                uint32_t previous = wave[nindex];
                uint32_t constrained = previous & allowed_neighbors;

                success = (constrained != 0u);

                if (constrained != previous)
                {
                    wave[nindex] = constrained;
                    queue[tail++] = nindex;
                }
            }
        }

        free(queue);
    }

    return success;
}

int banana_generate_tiles_wfc(int width, int height, uint32_t seed, int *out_tiles)
{
    int success = 1;
    banana_init_neighbor_masks();

    int cell_count = width * height;
    uint32_t *wave = (uint32_t *)malloc((size_t)cell_count * sizeof(uint32_t));
    uint32_t full_mask = (1u << banana_tile_count()) - 1u;
    uint32_t rng_state = seed ^ 0xBADC0DEu;

    success = (wave != NULL);
    if (success)
    {
        int iterations = 0;
        int max_iterations = cell_count * 8;

        {
            int i = 0;
#pragma omp parallel for schedule(static)
            for (i = 0; i < cell_count; i++)
                wave[i] = full_mask;
        }

        while (iterations < max_iterations && success)
        {
            uint32_t options = 0u;
            int chosen = -1;
            int cell = -1;

            iterations++;
            cell = banana_pick_lowest_entropy_cell(wave, cell_count, &rng_state);
            if (cell < 0)
                break;

            options = wave[cell];
            chosen = banana_pick_weighted_tile(options, &rng_state);
            success = (chosen >= 0);
            if (success)
            {
                wave[cell] = (1u << chosen);
                success = banana_propagate(width, height, wave, cell);
            }
        }

        {
            int i = 0;
#pragma omp parallel for schedule(static)
            for (i = 0; i < cell_count; i++)
            {
                int tile = banana_first_bit_index(wave[i]);
                out_tiles[i] = (tile < 0) ? BANANA_TILE_GRASS : tile;
            }
        }

        free(wave);
    }

    return success;
}
