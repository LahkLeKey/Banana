/**
 * @file terrain_features.c
 * @brief Terrain feature placement implementation
 */

#include "terrain_features.h"

#include <math.h>
#include <string.h>

/* Tile type constants (from banana_asset_domain.h) */
#define BANANA_TILE_WATER 0
#define BANANA_TILE_SAND 1
#define BANANA_TILE_GRASS 2
#define BANANA_TILE_GROVE 3
#define BANANA_TILE_HILL 4
#define BANANA_TILE_RIDGE 5

/* Simple deterministic PRNG for feature variation */
static uint64_t g_terrain_seed = 0;

static uint32_t terrain_features_hash(uint64_t x) {
  x = (x ^ (x >> 33)) * 0xff51afd7ed558ccdULL;
  return (x ^ (x >> 33)) >> 32;
}

static float terrain_features_random_for_pos(float world_x, float world_z) {
  uint32_t ix = (uint32_t)((world_x + 10000.0f) * 10.0f);
  uint32_t iz = (uint32_t)((world_z + 10000.0f) * 10.0f);
  uint64_t combined = ((uint64_t)ix << 32) | iz;
  uint32_t hash = terrain_features_hash(combined ^ g_terrain_seed);
  return (float)(hash % 100) / 100.0f;
}

int terrain_features_init(uint64_t seed) {
  g_terrain_seed = seed;
  return 0;
}

int terrain_features_get_spawn_probability(uint8_t tile_type,
                                            TerrainFeatureType feature_type) {
  /* Define biome-specific spawn rates */
  switch (tile_type) {
    case BANANA_TILE_GRASS:
      if (feature_type == TERRAIN_FEATURE_TREE) return 20;      /* 20% trees */
      if (feature_type == TERRAIN_FEATURE_BERRY_BUSH) return 10; /* 10% bushes */
      break;

    case BANANA_TILE_GROVE:
      if (feature_type == TERRAIN_FEATURE_TREE) return 70;       /* 70% trees */
      if (feature_type == TERRAIN_FEATURE_BERRY_BUSH) return 20; /* 20% bushes */
      break;

    case BANANA_TILE_HILL:
      if (feature_type == TERRAIN_FEATURE_ORE_COPPER) return 30; /* 30% copper ore */
      if (feature_type == TERRAIN_FEATURE_ORE_IRON) return 15;    /* 15% iron ore */
      break;

    case BANANA_TILE_RIDGE:
      if (feature_type == TERRAIN_FEATURE_ORE_COPPER) return 25; /* 25% copper ore */
      if (feature_type == TERRAIN_FEATURE_ORE_IRON) return 40;    /* 40% iron ore */
      break;

    case BANANA_TILE_WATER:
    case BANANA_TILE_SAND:
    default:
      return 0; /* No features on water/sand */
  }

  return 0;
}

TerrainFeatureType terrain_features_get_for_tile(uint8_t tile_type, float x, float z) {
  float rand = terrain_features_random_for_pos(x, z);
  float cumulative = 0.0f;

  /* Determine primary feature for this tile */
  TerrainFeatureType primary = TERRAIN_FEATURE_TREE;
  if (tile_type == BANANA_TILE_HILL || tile_type == BANANA_TILE_RIDGE) {
    primary = TERRAIN_FEATURE_ORE_COPPER;
  }

  /* Check against spawn probability */
  int prob = terrain_features_get_spawn_probability(tile_type, primary);
  if (rand * 100.0f < prob) {
    return primary;
  }

  /* Check secondary features */
  if (tile_type == BANANA_TILE_GRASS || tile_type == BANANA_TILE_GROVE) {
    TerrainFeatureType secondary = TERRAIN_FEATURE_BERRY_BUSH;
    int secondary_prob = terrain_features_get_spawn_probability(tile_type, secondary);
    cumulative = prob;
    if (rand * 100.0f < cumulative + secondary_prob) {
      return secondary;
    }
  }

  return TERRAIN_FEATURE_NONE;
}

int terrain_features_get_resource_yield(TerrainFeatureType feature_type) {
  switch (feature_type) {
    case TERRAIN_FEATURE_TREE:
      return 5; /* 5 wood per tree */
    case TERRAIN_FEATURE_ORE_COPPER:
      return 3; /* 3 ore per deposit */
    case TERRAIN_FEATURE_ORE_IRON:
      return 4; /* 4 ore per deposit */
    case TERRAIN_FEATURE_BERRY_BUSH:
      return 2; /* 2 berries per bush */
    default:
      return 0;
  }
}

const char *terrain_features_get_resource_name(TerrainFeatureType feature_type) {
  switch (feature_type) {
    case TERRAIN_FEATURE_TREE:
      return "wood";
    case TERRAIN_FEATURE_ORE_COPPER:
    case TERRAIN_FEATURE_ORE_IRON:
      return "ore";
    case TERRAIN_FEATURE_BERRY_BUSH:
      return "berries";
    default:
      return "unknown";
  }
}

int terrain_features_generate_for_chunk(int chunk_x, int chunk_z,
                                         const uint8_t *tile_types,
                                         FeatureSpawn *out_features, int max_features) {
  if (!tile_types || !out_features || max_features <= 0) {
    return 0;
  }

  int feature_count = 0;
  const float TILE_SIZE = 1.0f; /* Each tile is 1 unit */

  /* Iterate over 9x9 tiles in chunk */
  for (int ty = 0; ty < 9; ty++) {
    for (int tx = 0; tx < 9; tx++) {
      if (feature_count >= max_features) {
        return feature_count;
      }

      uint8_t tile_type = tile_types[ty * 9 + tx];

      /* Calculate world position of tile center */
      float world_x = chunk_x * 9.0f * TILE_SIZE + tx * TILE_SIZE + 0.5f;
      float world_z = chunk_z * 9.0f * TILE_SIZE + ty * TILE_SIZE + 0.5f;

      TerrainFeatureType feature = terrain_features_get_for_tile(tile_type, world_x, world_z);

      if (feature != TERRAIN_FEATURE_NONE) {
        FeatureSpawn spawn = {
            .x = world_x,
            .z = world_z,
            .type = feature,
            .seed = (int)terrain_features_hash(
                (uint64_t)chunk_x * 73856093U ^ (uint64_t)chunk_z * 19349663U ^
                (uint64_t)tx * 83492791U ^ (uint64_t)ty * 97816541U ^ g_terrain_seed),
        };

        out_features[feature_count++] = spawn;
      }
    }
  }

  return feature_count;
}

void terrain_features_shutdown(void) {
  /* No cleanup needed */
}
