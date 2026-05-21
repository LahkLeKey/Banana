/**
 * @file terrain_features_test.c
 * @brief Unit tests for terrain feature system
 */

#include "runtime/terrain_features.h"

#include <assert.h>
#include <stdio.h>

#define BANANA_TILE_WATER 0
#define BANANA_TILE_SAND 1
#define BANANA_TILE_GRASS 2
#define BANANA_TILE_GROVE 3
#define BANANA_TILE_HILL 4
#define BANANA_TILE_RIDGE 5

/* Test: Initialize system */
int test_terrain_features_init(void) {
  int status = terrain_features_init(12345);
  assert(status == 0);
  printf("✓ Test: init\n");
  return 0;
}

/* Test: Spawn probability mapping */
int test_terrain_features_spawn_probability(void) {
  terrain_features_init(0);

  /* Grass should spawn trees */
  int prob = terrain_features_get_spawn_probability(BANANA_TILE_GRASS, TERRAIN_FEATURE_TREE);
  assert(prob > 0); /* Should have some probability */

  /* Water should spawn nothing */
  prob = terrain_features_get_spawn_probability(BANANA_TILE_WATER, TERRAIN_FEATURE_TREE);
  assert(prob == 0);

  /* Hill should spawn ore */
  prob =
      terrain_features_get_spawn_probability(BANANA_TILE_HILL, TERRAIN_FEATURE_ORE_COPPER);
  assert(prob > 0);

  printf("✓ Test: spawn_probability\n");
  return 0;
}

/* Test: Resource yields */
int test_terrain_features_resource_yield(void) {
  int yield_tree = terrain_features_get_resource_yield(TERRAIN_FEATURE_TREE);
  assert(yield_tree > 0);

  int yield_ore = terrain_features_get_resource_yield(TERRAIN_FEATURE_ORE_COPPER);
  assert(yield_ore > 0);

  int yield_none = terrain_features_get_resource_yield(TERRAIN_FEATURE_NONE);
  assert(yield_none == 0);

  printf("✓ Test: resource_yield\n");
  return 0;
}

/* Test: Resource names */
int test_terrain_features_resource_names(void) {
  const char *name_tree = terrain_features_get_resource_name(TERRAIN_FEATURE_TREE);
  assert(name_tree != NULL);
  assert(name_tree[0] != '\0');

  const char *name_ore = terrain_features_get_resource_name(TERRAIN_FEATURE_ORE_COPPER);
  assert(name_ore != NULL);

  printf("✓ Test: resource_names\n");
  return 0;
}

/* Test: Feature placement determinism */
int test_terrain_features_determinism(void) {
  terrain_features_init(42);

  uint8_t chunk_tiles[81]; /* 9x9 */
  for (int i = 0; i < 81; i++) {
    chunk_tiles[i] = BANANA_TILE_GRASS; /* All grass */
  }

  FeatureSpawn features1[81];
  int count1 = terrain_features_generate_for_chunk(0, 0, chunk_tiles, features1, 81);

  /* Reset and generate again with same seed */
  terrain_features_init(42);
  FeatureSpawn features2[81];
  int count2 = terrain_features_generate_for_chunk(0, 0, chunk_tiles, features2, 81);

  assert(count1 == count2);

  /* Verify identical placements */
  for (int i = 0; i < count1; i++) {
    assert(features1[i].x == features2[i].x);
    assert(features1[i].z == features2[i].z);
    assert(features1[i].type == features2[i].type);
  }

  printf("✓ Test: determinism\n");
  return 0;
}

/* Test: Biome-specific features */
int test_terrain_features_biome_mapping(void) {
  terrain_features_init(0);

  /* Grove should spawn trees */
  uint8_t grove_chunk[81];
  for (int i = 0; i < 81; i++) {
    grove_chunk[i] = BANANA_TILE_GROVE;
  }

  FeatureSpawn features[81];
  int count = terrain_features_generate_for_chunk(0, 0, grove_chunk, features, 81);

  /* Grove should spawn many trees */
  assert(count > 0);

  /* Verify all are trees */
  int tree_count = 0;
  for (int i = 0; i < count; i++) {
    if (features[i].type == TERRAIN_FEATURE_TREE) {
      tree_count++;
    }
  }
  assert(tree_count > 0);

  printf("✓ Test: biome_mapping\n");
  return 0;
}

/* Test: Hill ore generation */
int test_terrain_features_hill_ore(void) {
  terrain_features_init(0);

  uint8_t hill_chunk[81];
  for (int i = 0; i < 81; i++) {
    hill_chunk[i] = BANANA_TILE_HILL;
  }

  FeatureSpawn features[81];
  int count = terrain_features_generate_for_chunk(0, 0, hill_chunk, features, 81);

  /* Hill should generate ore features */
  assert(count > 0);

  int ore_count = 0;
  for (int i = 0; i < count; i++) {
    if (features[i].type == TERRAIN_FEATURE_ORE_COPPER ||
        features[i].type == TERRAIN_FEATURE_ORE_IRON) {
      ore_count++;
    }
  }
  assert(ore_count > 0);

  printf("✓ Test: hill_ore\n");
  return 0;
}

/* Test: Water/sand no features */
int test_terrain_features_water_sand_empty(void) {
  terrain_features_init(0);

  uint8_t water_chunk[81];
  for (int i = 0; i < 81; i++) {
    water_chunk[i] = BANANA_TILE_WATER;
  }

  FeatureSpawn features[81];
  int count = terrain_features_generate_for_chunk(0, 0, water_chunk, features, 81);

  /* Water should not spawn features */
  assert(count == 0);

  uint8_t sand_chunk[81];
  for (int i = 0; i < 81; i++) {
    sand_chunk[i] = BANANA_TILE_SAND;
  }

  count = terrain_features_generate_for_chunk(0, 0, sand_chunk, features, 81);
  assert(count == 0);

  printf("✓ Test: water_sand_empty\n");
  return 0;
}

/* Test: Max features cap */
int test_terrain_features_max_features_cap(void) {
  terrain_features_init(0);

  uint8_t chunk[81];
  for (int i = 0; i < 81; i++) {
    chunk[i] = BANANA_TILE_GROVE; /* Dense feature area */
  }

  FeatureSpawn features[10];
  int count = terrain_features_generate_for_chunk(0, 0, chunk, features, 10);

  /* Should respect max count */
  assert(count <= 10);

  printf("✓ Test: max_features_cap\n");
  return 0;
}

int main(void) {
  printf("Running terrain features tests...\n\n");

  int failed = 0;

  failed += test_terrain_features_init();
  failed += test_terrain_features_spawn_probability();
  failed += test_terrain_features_resource_yield();
  failed += test_terrain_features_resource_names();
  failed += test_terrain_features_determinism();
  failed += test_terrain_features_biome_mapping();
  failed += test_terrain_features_hill_ore();
  failed += test_terrain_features_water_sand_empty();
  failed += test_terrain_features_max_features_cap();

  printf("\n%s\n", failed == 0 ? "All tests passed! ✓" : "Some tests failed! ✗");
  return failed;
}
