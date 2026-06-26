#include "tools/infrastructure/banana_asset_json_writer.h"
#include "tools/domain/banana_asset_domain.h"

#include "runtime/support/test_support.h"

#include <stdio.h>
#include <string.h>

static void test_write_palette_json_null_path(void **state)
{
    (void)state;
    /* null path -> fopen fails -> returns 0 */
    int result = banana_write_palette_json(NULL);
    BANANA_TEST_ASSERT_INT_EQ(result, 0,
                              "null path must return 0 (failure)");

    /* nonexistent dir -> fopen fails -> returns 0 */
    result = banana_write_palette_json("/no-such-dir/out.json");
    BANANA_TEST_ASSERT_INT_EQ(result, 0,
                              "invalid path must return 0 (failure)");
}

static void test_write_terrain_json_null_path(void **state)
{
    (void)state;
    banana_compiler_config_t cfg = {
        .seed = 42,
        .width = 4,
        .height = 4,
        .profile = "test",
    };
    banana_terrain_cell_t cells[16] = {0};

    int result = banana_write_terrain_json(NULL, &cfg, cells, 0ULL, 0);
    BANANA_TEST_ASSERT_INT_EQ(result, 0,
                              "null path must return 0 for terrain json");
}

static void test_write_manifest_json_null_path(void **state)
{
    (void)state;
    banana_compiler_config_t cfg = {
        .seed = 42,
        .width = 4,
        .height = 4,
        .profile = "test",
    };
    int result = banana_write_manifest_json(NULL, &cfg, 0ULL, 0);
    BANANA_TEST_ASSERT_INT_EQ(result, 0,
                              "null path must return 0 for manifest json");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_write_palette_json_null_path),
    BANANA_TEST_CASE(test_write_terrain_json_null_path),
    BANANA_TEST_CASE(test_write_manifest_json_null_path)
)
