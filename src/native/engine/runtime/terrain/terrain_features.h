/**
 * @file terrain_features.h
 * @brief Procedural terrain feature placement (trees, ore deposits, etc.)
 *
 * Places interactive features (trees, ore deposits) on terrain based on tile biome type.
 * Features are deterministically placed using terrain seed for consistency across clients.
 */

#ifndef BANANA_ENGINE_TERRAIN_FEATURES_H
#define BANANA_ENGINE_TERRAIN_FEATURES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Feature types that can appear on terrain
 */
typedef enum {
  TERRAIN_FEATURE_NONE = 0,
  TERRAIN_FEATURE_TREE = 1,     // Appears on grass/grove tiles
  TERRAIN_FEATURE_ORE_COPPER = 2,  // Appears on hill/ridge tiles
  TERRAIN_FEATURE_ORE_IRON = 3,    // Appears on hill/ridge tiles
  TERRAIN_FEATURE_BERRY_BUSH = 4,  // Appears on grass tiles
} TerrainFeatureType;

/**
 * Feature spawn data (deterministic placement)
 */
typedef struct {
  float x, z;               // World position
  TerrainFeatureType type;  // Feature type
  int seed;                 // Per-feature deterministic seed for variations
} FeatureSpawn;

/**
 * Initialize terrain feature system
 * @param seed World seed for deterministic placement
 * @return Status code (0 = success)
 */
int terrain_features_init(uint64_t seed);

/**
 * Generate features for a terrain chunk
 * Places features (trees, ore, etc.) on tile-by-tile basis
 * @param chunk_x Chunk X coordinate (in chunk grid)
 * @param chunk_z Chunk Z coordinate (in chunk grid)
 * @param tile_types Array of 9x9 tile type IDs for the chunk
 * @param out_features [out] Array of generated feature spawns
 * @param max_features Maximum features to generate
 * @return Number of features generated
 */
int terrain_features_generate_for_chunk(int chunk_x, int chunk_z,
                                         const uint8_t *tile_types,
                                         FeatureSpawn *out_features, int max_features);

/**
 * Get biome-appropriate feature for tile type
 * Determines which feature should appear on a given tile type
 * @param tile_type Tile type ID (BANANA_TILE_*)
 * @param x World X coordinate (for variation)
 * @param z World Z coordinate (for variation)
 * @return Feature type to place (may return TERRAIN_FEATURE_NONE)
 */
TerrainFeatureType terrain_features_get_for_tile(uint8_t tile_type, float x, float z);

/**
 * Get spawning probability for feature type on given tile
 * @param tile_type Tile type ID
 * @param feature_type Feature to spawn
 * @return Probability 0-100 (0 = never, 100 = always)
 */
int terrain_features_get_spawn_probability(uint8_t tile_type,
                                            TerrainFeatureType feature_type);

/**
 * Get resource yield for feature type
 * @param feature_type Feature type
 * @return Resource yield amount
 */
int terrain_features_get_resource_yield(TerrainFeatureType feature_type);

/**
 * Get resource name for feature type
 * @param feature_type Feature type
 * @return Resource type name (e.g., "wood", "ore")
 */
const char *terrain_features_get_resource_name(TerrainFeatureType feature_type);

/**
 * Shutdown terrain feature system
 */
void terrain_features_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif // BANANA_ENGINE_TERRAIN_FEATURES_H
