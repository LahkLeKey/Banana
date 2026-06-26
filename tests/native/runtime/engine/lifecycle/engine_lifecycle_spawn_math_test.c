#include "runtime/engine/lifecycle/engine_lifecycle_spawn_math.h"

#include "runtime/support/test_support.h"

static float g_sample_base_height = 0.0f;

static float fake_sample_height(float x, float z)
{
    return g_sample_base_height + x * 0.5f + z * 0.25f;
}

static float flat_sample_height(float x, float z)
{
    (void)x;
    (void)z;
    return 7.0f;
}

static float staged_corner_sample_height(float x, float z)
{
    if (x == -1.0f && z == -1.0f)
        return 1.0f;

    if (x == 1.0f && z == -1.0f)
        return 2.0f;

    if (x == -1.0f && z == 1.0f)
        return 3.0f;

    if (x == 1.0f && z == 1.0f)
        return 2.0f;

    return 0.0f;
}

static void test_spawn_hash_is_deterministic(void **state)
{
    (void)state;
    unsigned int a = runtime_engine_lifecycle_spawn_hash(123u);
    unsigned int b = runtime_engine_lifecycle_spawn_hash(123u);
    unsigned int c = runtime_engine_lifecycle_spawn_hash(124u);

    BANANA_TEST_ASSERT_INT_EQ((int)a,
                              (int)b,
                              "spawn hash must be deterministic for the same seed");
    BANANA_TEST_ASSERT_TRUE(a != c,
                            "spawn hash should vary across different seeds");
}

static void test_spawn_jitter_stays_within_expected_range(void **state)
{
    (void)state;
    float j0 = runtime_engine_lifecycle_spawn_jitter(0u);
    float j255 = runtime_engine_lifecycle_spawn_jitter(255u);

    BANANA_TEST_ASSERT_FLOAT_EQ(j0, -1.2f, 0.0002f,
                                "spawn jitter seed 0 must map to lower bound");
    BANANA_TEST_ASSERT_FLOAT_EQ(j255, 1.2f, 0.0002f,
                                "spawn jitter seed 255 must map to upper bound");
}

static void test_spawn_sample_max_ground_uses_center_and_corners(void **state)
{
    (void)state;
    float sampled = 0.0f;

    g_sample_base_height = 10.0f;
    sampled = runtime_engine_lifecycle_spawn_sample_max_ground(fake_sample_height,
                                                               4.0f,
                                                               6.0f,
                                                               2.0f,
                                                               4.0f);

    BANANA_TEST_ASSERT_FLOAT_EQ(sampled, 14.5f, 0.0001f,
                                "spawn max-ground sampling must return the highest point among center and footprint corners");
}

static void test_spawn_sample_max_ground_keeps_center_when_corners_are_equal(void **state)
{
    (void)state;
    float sampled = runtime_engine_lifecycle_spawn_sample_max_ground(flat_sample_height,
                                                                      12.0f,
                                                                      -6.0f,
                                                                      3.0f,
                                                                      5.0f);

    BANANA_TEST_ASSERT_FLOAT_EQ(sampled, 7.0f, 0.0001f,
                                "spawn max-ground sampling must keep center height when all corners are equal");
}

static void test_spawn_sample_max_ground_updates_with_each_new_higher_corner(void **state)
{
    (void)state;
    float sampled = runtime_engine_lifecycle_spawn_sample_max_ground(staged_corner_sample_height,
                                                                      0.0f,
                                                                      0.0f,
                                                                      2.0f,
                                                                      2.0f);

    BANANA_TEST_ASSERT_FLOAT_EQ(sampled, 3.0f, 0.0001f,
                                "spawn max-ground sampling must keep the highest value encountered across corner scans");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_spawn_hash_is_deterministic),
    BANANA_TEST_CASE(test_spawn_jitter_stays_within_expected_range),
    BANANA_TEST_CASE(test_spawn_sample_max_ground_uses_center_and_corners),
    BANANA_TEST_CASE(test_spawn_sample_max_ground_keeps_center_when_corners_are_equal),
    BANANA_TEST_CASE(test_spawn_sample_max_ground_updates_with_each_new_higher_corner)
)
