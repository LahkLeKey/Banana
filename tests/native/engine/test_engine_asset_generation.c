#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../src/native/engine/tools/application/banana_asset_generation_service.h"
#include "../../../src/native/engine/tools/banana_asset_types.h"
#include "../../../src/native/engine/tools/domain/banana_asset_algorithms.h"
#include "../../../src/native/engine/tools/domain/banana_asset_domain.h"

static int s_pass = 0;
static int s_fail = 0;

#define TEST(name) do { printf("  %-62s", name); } while (0)
#define PASS() do { printf("PASS\n"); s_pass++; } while (0)
#define FAIL(msg) do { printf("FAIL (%s)\n", msg); s_fail++; } while (0)

#define ASSERT_TRUE(expr, msg) \
  do { \
    if (!(expr)) { \
      FAIL(msg); \
      return; \
    } \
  } while (0)

static uint64_t checksum_cells(const banana_terrain_cell_t *cells, int count) {
  uint64_t hash = 1469598103934665603ull;
  for (int i = 0; i < count; i++) {
    hash ^= (uint64_t)(unsigned int)cells[i].tile_index;
    hash *= 1099511628211ull;
    hash ^= (uint64_t)(unsigned int)cells[i].elevation;
    hash *= 1099511628211ull;
    hash ^= (uint64_t)(unsigned int)cells[i].resource;
    hash *= 1099511628211ull;
  }
  return hash;
}

static void test_same_seed_is_deterministic(void) {
  TEST("asset_generation: same seed/profile yields identical outputs");

  banana_compiler_config_t config = {
      .seed = 20260509u,
      .profile = "test",
      .out_dir = "unused",
      .width = 24,
      .height = 24,
  };

  int count = config.width * config.height;
  banana_terrain_cell_t *cells_a = (banana_terrain_cell_t *)malloc((size_t)count * sizeof(banana_terrain_cell_t));
  banana_terrain_cell_t *cells_b = (banana_terrain_cell_t *)malloc((size_t)count * sizeof(banana_terrain_cell_t));
  ASSERT_TRUE(cells_a != NULL && cells_b != NULL, "allocation failed");

  uint64_t checksum_a = 0;
  uint64_t checksum_b = 0;

  ASSERT_TRUE(banana_generate_terrain_cells(&config, 3, cells_a, &checksum_a) == 1, "generation A failed");
  ASSERT_TRUE(banana_generate_terrain_cells(&config, 3, cells_b, &checksum_b) == 1, "generation B failed");

  ASSERT_TRUE(checksum_a == checksum_b, "checksums differ");
  ASSERT_TRUE(checksum_cells(cells_a, count) == checksum_cells(cells_b, count), "cell payload differs");

  free(cells_a);
  free(cells_b);
  PASS();
}

static void test_different_seed_changes_output(void) {
  TEST("asset_generation: different seeds produce different checksum");

  banana_compiler_config_t config_a = {
      .seed = 1111u,
      .profile = "test",
      .out_dir = "unused",
      .width = 24,
      .height = 24,
  };

  banana_compiler_config_t config_b = config_a;
  config_b.seed = 2222u;

  int count = config_a.width * config_a.height;
  banana_terrain_cell_t *cells_a = (banana_terrain_cell_t *)malloc((size_t)count * sizeof(banana_terrain_cell_t));
  banana_terrain_cell_t *cells_b = (banana_terrain_cell_t *)malloc((size_t)count * sizeof(banana_terrain_cell_t));
  ASSERT_TRUE(cells_a != NULL && cells_b != NULL, "allocation failed");

  uint64_t checksum_a = 0;
  uint64_t checksum_b = 0;

  ASSERT_TRUE(banana_generate_terrain_cells(&config_a, 3, cells_a, &checksum_a) == 1, "generation A failed");
  ASSERT_TRUE(banana_generate_terrain_cells(&config_b, 3, cells_b, &checksum_b) == 1, "generation B failed");

  ASSERT_TRUE(checksum_a != checksum_b, "checksums unexpectedly equal");

  free(cells_a);
  free(cells_b);
  PASS();
}

static void test_output_value_ranges(void) {
  TEST("asset_generation: tiles are valid and resources non-negative");

  banana_compiler_config_t config = {
      .seed = 424242u,
      .profile = "test",
      .out_dir = "unused",
      .width = 20,
      .height = 20,
  };

  int count = config.width * config.height;
  banana_terrain_cell_t *cells = (banana_terrain_cell_t *)malloc((size_t)count * sizeof(banana_terrain_cell_t));
  ASSERT_TRUE(cells != NULL, "allocation failed");

  uint64_t checksum = 0;
  ASSERT_TRUE(banana_generate_terrain_cells(&config, 3, cells, &checksum) == 1, "generation failed");
  (void)checksum;

  for (int i = 0; i < count; i++) {
    ASSERT_TRUE(cells[i].tile_index >= 0 && cells[i].tile_index < banana_tile_count(), "tile index out of range");
    ASSERT_TRUE(cells[i].resource >= 0, "resource is negative");
  }

  free(cells);
  PASS();
}

static void test_default_algorithm_contract_is_exposed(void) {
  TEST("asset_generation: default algorithm contract is composable");

  const banana_asset_algorithm_contract_t *contract = banana_default_asset_algorithm_contract();
  ASSERT_TRUE(contract != NULL, "contract is null");
  ASSERT_TRUE(contract->generate_tiles != NULL, "generate_tiles missing");
  ASSERT_TRUE(contract->post_process_tiles != NULL, "post_process_tiles missing");
  ASSERT_TRUE(contract->tile_generation_name != NULL, "tile generation name missing");
  ASSERT_TRUE(contract->post_process_name != NULL, "post process name missing");
  ASSERT_TRUE(strcmp(contract->tile_generation_name, "wave-function-collapse") == 0,
              "unexpected tile generation name");
  ASSERT_TRUE(strcmp(contract->post_process_name, "cellular-automata") == 0,
              "unexpected post process name");

  banana_compiler_config_t config = {
      .seed = 606060u,
      .profile = "contract",
      .out_dir = "unused",
      .width = 16,
      .height = 16,
  };

  int count = config.width * config.height;
  banana_terrain_cell_t *cells = (banana_terrain_cell_t *)malloc((size_t)count * sizeof(banana_terrain_cell_t));
  ASSERT_TRUE(cells != NULL, "allocation failed");

  uint64_t checksum = 0;
  ASSERT_TRUE(
      banana_generate_terrain_cells_with_contract(&config, 3, contract, cells, &checksum) == 1,
      "generation with contract failed");
  ASSERT_TRUE(checksum != 0ull, "checksum not populated");

  free(cells);
  PASS();
}

static void test_contract_path_preserves_determinism(void) {
  TEST("asset_generation: contract path preserves deterministic output");

  const banana_asset_algorithm_contract_t *contract = banana_default_asset_algorithm_contract();
  ASSERT_TRUE(contract != NULL, "contract is null");

  banana_compiler_config_t config = {
      .seed = 909090u,
      .profile = "contract-deterministic",
      .out_dir = "unused",
      .width = 18,
      .height = 18,
  };

  int count = config.width * config.height;
  banana_terrain_cell_t *cells_a = (banana_terrain_cell_t *)malloc((size_t)count * sizeof(banana_terrain_cell_t));
  banana_terrain_cell_t *cells_b = (banana_terrain_cell_t *)malloc((size_t)count * sizeof(banana_terrain_cell_t));
  ASSERT_TRUE(cells_a != NULL && cells_b != NULL, "allocation failed");

  uint64_t checksum_a = 0;
  uint64_t checksum_b = 0;

  ASSERT_TRUE(
      banana_generate_terrain_cells_with_contract(&config, 3, contract, cells_a, &checksum_a) == 1,
      "first contract generation failed");
  ASSERT_TRUE(
      banana_generate_terrain_cells_with_contract(&config, 3, contract, cells_b, &checksum_b) == 1,
      "second contract generation failed");
  ASSERT_TRUE(checksum_a == checksum_b, "contract path checksums differ");
  ASSERT_TRUE(checksum_cells(cells_a, count) == checksum_cells(cells_b, count),
              "contract path payload differs");

  free(cells_a);
  free(cells_b);
  PASS();
}

static void test_contract_guard_rejects_missing_callbacks(void) {
  TEST("asset_generation: contract guard rejects missing callbacks");

  banana_asset_algorithm_contract_t invalid_contract = {
      .tile_generation_name = "invalid",
      .post_process_name = "invalid",
      .generate_tiles = NULL,
      .post_process_tiles = NULL,
  };

  banana_compiler_config_t config = {
      .seed = 123u,
      .profile = "guard",
      .out_dir = "unused",
      .width = 8,
      .height = 8,
  };

  banana_terrain_cell_t cells[64];
  uint64_t checksum = 0;

  ASSERT_TRUE(
      banana_generate_terrain_cells_with_contract(&config, 3, &invalid_contract, cells, &checksum) == 0,
      "invalid contract unexpectedly accepted");

  PASS();
}

int main(void) {
  printf("\n=== Banana Engine Asset Generation Tests ===\n\n");
  test_same_seed_is_deterministic();
  test_different_seed_changes_output();
  test_output_value_ranges();
  test_default_algorithm_contract_is_exposed();
  test_contract_path_preserves_determinism();
  test_contract_guard_rejects_missing_callbacks();

  printf("\nResults: %d passed, %d failed\n", s_pass, s_fail);
  return (s_fail == 0) ? 0 : 1;
}
