#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

#ifndef BANANA_ASSET_COMPILER_PATH
#define BANANA_ASSET_COMPILER_PATH "banana_asset_compiler"
#endif

static int s_pass = 0;
static int s_fail = 0;

#define TEST(name) do { printf("  %-66s", name); } while (0)
#define PASS() do { printf("PASS\n"); s_pass++; } while (0)
#define FAIL(msg) do { printf("FAIL (%s)\n", msg); s_fail++; } while (0)
#define ASSERT_TRUE(expr, msg) do { if (!(expr)) { FAIL(msg); return; } } while (0)

static int ensure_dir_exists(const char *path) {
  if (!path || !*path) {
    return 0;
  }

  if (MKDIR(path) == 0 || errno == EEXIST) {
    return 1;
  }

  return 0;
}

static char *read_text_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    return NULL;
  }

  if (fseek(f, 0, SEEK_END) != 0) {
    fclose(f);
    return NULL;
  }

  long size = ftell(f);
  if (size < 0) {
    fclose(f);
    return NULL;
  }

  if (fseek(f, 0, SEEK_SET) != 0) {
    fclose(f);
    return NULL;
  }

  char *buffer = (char *)malloc((size_t)size + 1u);
  if (!buffer) {
    fclose(f);
    return NULL;
  }

  size_t read = fread(buffer, 1, (size_t)size, f);
  fclose(f);
  if (read != (size_t)size) {
    free(buffer);
    return NULL;
  }

  buffer[size] = '\0';
  return buffer;
}

static int file_exists(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    return 0;
  }
  fclose(f);
  return 1;
}

static int run_compiler_command(
    const char *out_dir,
    unsigned int seed,
    const char *profile,
    int width,
    int height,
    int ca_iterations) {
  char command[2048];
  int written = snprintf(
      command,
      sizeof(command),
      "%s --out-dir %s --seed %u --profile %s --width %d --height %d --ca-iterations %d",
      BANANA_ASSET_COMPILER_PATH,
      out_dir,
      seed,
      profile,
      width,
      height,
      ca_iterations);
  if (written <= 0 || (size_t)written >= sizeof(command)) {
    return 0;
  }

  return system(command) == 0;
}

static int extract_checksum(const char *json_text, char *out, size_t out_size) {
  const char *needle = "\"checksum\": \"0x";
  const char *start = strstr(json_text, needle);
  if (!start) {
    return 0;
  }

  start += strlen("\"checksum\": \"");
  const char *end = strchr(start, '"');
  if (!end || end <= start) {
    return 0;
  }

  size_t length = (size_t)(end - start);
  if (length + 1u > out_size) {
    return 0;
  }

  memcpy(out, start, length);
  out[length] = '\0';
  return 1;
}

static void test_compiler_emits_metadata_contract(void) {
  TEST("asset_compiler: emits algorithm/postProcess/checksum metadata contract");

  const char *out_dir = "generated-assets-integration";
  const char *terrain_path = "generated-assets-integration/banana-generated-terrain.json";
  const char *manifest_path = "generated-assets-integration/banana-generated-assets.manifest.json";
  const char *palette_path = "generated-assets-integration/banana-generated-palette.json";

  ASSERT_TRUE(ensure_dir_exists(out_dir) == 1, "failed to create output dir");

  ASSERT_TRUE(run_compiler_command(out_dir, 424242u, "integration", 18, 18, 4), "compiler process failed");

  ASSERT_TRUE(file_exists(palette_path) == 1, "palette file missing");
  ASSERT_TRUE(file_exists(terrain_path) == 1, "terrain file missing");
  ASSERT_TRUE(file_exists(manifest_path) == 1, "manifest file missing");

  char *terrain = read_text_file(terrain_path);
  ASSERT_TRUE(terrain != NULL, "failed to read terrain file");
  ASSERT_TRUE(strstr(terrain, "\"algorithm\": \"wave-function-collapse+cellular-automata\"") != NULL,
              "algorithm metadata missing");
  ASSERT_TRUE(strstr(terrain, "\"postProcess\": {\"cellularAutomataIterations\": 4}") != NULL,
              "postProcess metadata missing");
  ASSERT_TRUE(strstr(terrain, "\"checksum\": \"0x") != NULL, "terrain checksum format missing");
  free(terrain);

  char *manifest = read_text_file(manifest_path);
  ASSERT_TRUE(manifest != NULL, "failed to read manifest file");
  ASSERT_TRUE(strstr(manifest, "\"kind\": \"banana-generated-assets\"") != NULL, "manifest kind missing");
  ASSERT_TRUE(strstr(manifest, "\"compiler\": \"banana_asset_compiler\"") != NULL, "manifest compiler missing");
  ASSERT_TRUE(strstr(manifest, "\"profile\": \"integration\"") != NULL, "manifest profile missing");
  ASSERT_TRUE(strstr(manifest, "\"seed\": 424242") != NULL, "manifest seed missing");
  ASSERT_TRUE(strstr(manifest, "\"width\": 18") != NULL, "manifest width missing");
  ASSERT_TRUE(strstr(manifest, "\"height\": 18") != NULL, "manifest height missing");
  ASSERT_TRUE(strstr(manifest, "\"postProcess\": {\"cellularAutomataIterations\": 4}") != NULL,
              "manifest postProcess missing");
  ASSERT_TRUE(strstr(manifest, "\"adjacency\": \"neighbor biomes differ by at most 1\"") != NULL,
              "manifest adjacency rule missing");
  ASSERT_TRUE(strstr(manifest, "\"checksum\":\"0x") != NULL, "manifest checksum format missing");
  free(manifest);

  PASS();
}

static void test_compiler_same_seed_is_deterministic(void) {
  TEST("asset_compiler: repeated same-seed runs keep identical checksum");

  const char *out_dir = "generated-assets-integration";
  const char *terrain_path = "generated-assets-integration/banana-generated-terrain.json";

  ASSERT_TRUE(ensure_dir_exists(out_dir) == 1, "failed to create output dir");

  ASSERT_TRUE(run_compiler_command(out_dir, 777777u, "deterministic", 22, 22, 3), "first run failed");
  char *terrain_first = read_text_file(terrain_path);
  ASSERT_TRUE(terrain_first != NULL, "failed reading first terrain output");

  ASSERT_TRUE(run_compiler_command(out_dir, 777777u, "deterministic", 22, 22, 3), "second run failed");
  char *terrain_second = read_text_file(terrain_path);
  ASSERT_TRUE(terrain_second != NULL, "failed reading second terrain output");

  char checksum_first[64];
  char checksum_second[64];
  ASSERT_TRUE(extract_checksum(terrain_first, checksum_first, sizeof(checksum_first)) == 1,
              "missing checksum in first run");
  ASSERT_TRUE(extract_checksum(terrain_second, checksum_second, sizeof(checksum_second)) == 1,
              "missing checksum in second run");

  ASSERT_TRUE(strcmp(checksum_first, checksum_second) == 0, "checksums differ between deterministic runs");

  free(terrain_first);
  free(terrain_second);
  PASS();
}

static void test_compiler_different_seeds_diverge(void) {
  TEST("asset_compiler: different seeds produce different checksums");

  const char *out_dir = "generated-assets-integration";
  const char *terrain_path = "generated-assets-integration/banana-generated-terrain.json";

  ASSERT_TRUE(ensure_dir_exists(out_dir) == 1, "failed to create output dir");

  ASSERT_TRUE(run_compiler_command(out_dir, 888001u, "divergence", 22, 22, 3), "first run failed");
  char *terrain_first = read_text_file(terrain_path);
  ASSERT_TRUE(terrain_first != NULL, "failed reading first terrain output");

  ASSERT_TRUE(run_compiler_command(out_dir, 888002u, "divergence", 22, 22, 3), "second run failed");
  char *terrain_second = read_text_file(terrain_path);
  ASSERT_TRUE(terrain_second != NULL, "failed reading second terrain output");

  char checksum_first[64];
  char checksum_second[64];
  ASSERT_TRUE(extract_checksum(terrain_first, checksum_first, sizeof(checksum_first)) == 1,
              "missing checksum in first run");
  ASSERT_TRUE(extract_checksum(terrain_second, checksum_second, sizeof(checksum_second)) == 1,
              "missing checksum in second run");

  ASSERT_TRUE(strcmp(checksum_first, checksum_second) != 0, "checksums unexpectedly equal across different seeds");

  free(terrain_first);
  free(terrain_second);
  PASS();
}

int main(void) {
  printf("\n=== Banana Engine Asset Compiler Integration Tests ===\n\n");

  test_compiler_emits_metadata_contract();
  test_compiler_same_seed_is_deterministic();
  test_compiler_different_seeds_diverge();

  printf("\nResults: %d passed, %d failed\n", s_pass, s_fail);
  return s_fail == 0 ? 0 : 1;
}
