#include "runtime/terrain/terrain_mutation.h"

#include "../support/test_support.h"

#include <string.h>
#include <stdint.h>

static int g_dirty_calls = 0;
static int g_last_dirty_cx = -1;
static int g_last_dirty_cz = -1;

static void stub_mark_chunk_dirty(int cx, int cz)
{
    g_dirty_calls += 1;
    g_last_dirty_cx = cx;
    g_last_dirty_cz = cz;
}

static void reset(void)
{
    g_dirty_calls = 0;
    g_last_dirty_cx = -1;
    g_last_dirty_cz = -1;
}

static void test_set_height_guards(void **state)
{
    (void)state;
    unsigned char grid[64 * 64] = {0};

    /* null guards */
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_set_height(NULL, 64, 4, 8, 0, 0, 1, stub_mark_chunk_dirty),
                              0,
                              "null height grid must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_set_height(grid, 64, 4, 8, 0, 0, 1, NULL),
                              0,
                              "null mark_chunk_dirty callback must be rejected");

    /* invalid dimensions */
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_set_height(grid, 0, 4, 8, 0, 0, 1, stub_mark_chunk_dirty),
                              0,
                              "zero terrain size must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_set_height(grid, 64, 0, 8, 0, 0, 1, stub_mark_chunk_dirty),
                              0,
                              "zero max layers must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_set_height(grid, 64, 4, 0, 0, 0, 1, stub_mark_chunk_dirty),
                              0,
                              "zero chunk size must be rejected");

    /* out-of-bounds coords */
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_set_height(grid, 64, 4, 8, -1, 0, 1, stub_mark_chunk_dirty),
                              0,
                              "negative x must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_set_height(grid, 64, 4, 8, 64, 0, 1, stub_mark_chunk_dirty),
                              0,
                              "x == terrain_size must be rejected");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_set_height(grid, 64, 4, 8, 0, -1, 1, stub_mark_chunk_dirty),
                              0,
                              "negative z must be rejected");
}

static void test_set_height_writes_and_marks_dirty(void **state)
{
    (void)state;
    unsigned char grid[64 * 64] = {0};
    reset();

    /* set interior tile */
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_set_height(grid, 64, 4, 8, 9, 9, 2, stub_mark_chunk_dirty),
                              1,
                              "valid set_height must return 1");
    BANANA_TEST_ASSERT_INT_EQ((int)grid[9 * 64 + 9],
                              2,
                              "height grid must reflect new elevation");
    BANANA_TEST_ASSERT_TRUE(g_dirty_calls >= 1,
                            "at least one chunk must be marked dirty");

    /* set to same value - should still return 1 but not re-mark dirty */
    reset();
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_set_height(grid, 64, 4, 8, 9, 9, 2, stub_mark_chunk_dirty),
                              1,
                              "setting same elevation must still return success");
    BANANA_TEST_ASSERT_INT_EQ(g_dirty_calls,
                              0,
                              "setting same elevation must not re-mark chunk dirty");

    /* tile on chunk boundary - should mark extra adjacent chunks */
    reset();
    runtime_terrain_set_height(grid, 64, 4, 8, 8, 8, 3, stub_mark_chunk_dirty); /* x==8 = chunk boundary for chunk_size=8 */
    BANANA_TEST_ASSERT_TRUE(g_dirty_calls >= 2,
                            "chunk-boundary tile must mark at least two chunks dirty");

    /* elevation clamping */
    runtime_terrain_set_height(grid, 64, 4, 8, 5, 5, 99, stub_mark_chunk_dirty);
    BANANA_TEST_ASSERT_INT_EQ((int)grid[5 * 64 + 5],
                              3,  /* terrain_max_layers - 1 = 3 */
                              "elevation above max_layers must be clamped");
    runtime_terrain_set_height(grid, 64, 4, 8, 5, 5, -5, stub_mark_chunk_dirty);
    BANANA_TEST_ASSERT_INT_EQ((int)grid[5 * 64 + 5],
                              0,
                              "negative elevation must be clamped to zero");
}

static void test_mark_region_dirty(void **state)
{
    (void)state;
    reset();
    runtime_terrain_mark_region_dirty(64, 8, 0, 0, 15, 15, stub_mark_chunk_dirty);
    BANANA_TEST_ASSERT_TRUE(g_dirty_calls > 0,
                            "mark_region_dirty must invoke chunk dirty callback for covered chunks");
}

static void test_apply_delta(void **state)
{
    (void)state;
    unsigned char grid[64 * 64] = {0};
    unsigned int applied_seq = 0;
    /* sequence must be applied_seq+1=1 for first apply to succeed */
    RuntimeTerrainDelta delta = { .x = 12, .z = 15, .elevation = 2, .sequence = 1 };
    reset();

    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_apply_delta(NULL, 64, 4, 8, &delta, &applied_seq, stub_mark_chunk_dirty),
                              0,
                              "null grid must be rejected by apply_delta");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_apply_delta(grid, 64, 4, 8, NULL, &applied_seq, stub_mark_chunk_dirty),
                              0,
                              "null delta must be rejected by apply_delta");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_apply_delta(grid, 64, 4, 8, &delta, NULL, stub_mark_chunk_dirty),
                              0,
                              "null sequence pointer must be rejected by apply_delta");

    int result = runtime_terrain_apply_delta(grid, 64, 4, 8, &delta, &applied_seq, stub_mark_chunk_dirty);
    BANANA_TEST_ASSERT_INT_EQ(result,
                              1,
                              "valid delta must apply successfully");
    BANANA_TEST_ASSERT_INT_EQ((int)grid[15 * 64 + 12],
                              2,
                              "apply_delta must write elevation to grid");
    BANANA_TEST_ASSERT_INT_EQ((int)applied_seq,
                              1,
                              "applied sequence must be updated after success");

    /* Replaying same sequence must be a no-op (sequence != applied+1) */
    reset();
    result = runtime_terrain_apply_delta(grid, 64, 4, 8, &delta, &applied_seq, stub_mark_chunk_dirty);
    BANANA_TEST_ASSERT_INT_EQ(result,
                              0,
                              "replaying same delta sequence must return 0");
    BANANA_TEST_ASSERT_INT_EQ(g_dirty_calls,
                              0,
                              "replaying old delta sequence must not mark chunks dirty");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_set_height_guards),
    BANANA_TEST_CASE(test_set_height_writes_and_marks_dirty),
    BANANA_TEST_CASE(test_mark_region_dirty),
    BANANA_TEST_CASE(test_apply_delta)
)
