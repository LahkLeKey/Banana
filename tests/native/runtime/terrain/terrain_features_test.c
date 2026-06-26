#include "runtime/terrain/terrain_features.h"

#include "runtime/support/test_support.h"

#include <string.h>
#include <stdint.h>

static void test_terrain_features_init_and_spawn_probability(void **state)
{
    (void)state;
    BANANA_TEST_ASSERT_INT_EQ(terrain_features_init(12345u),
                              0,
                              "terrain features init must succeed");

    /* GRASS tile */
    BANANA_TEST_ASSERT_INT_EQ(terrain_features_get_spawn_probability(2, TERRAIN_FEATURE_TREE),
                              20,
                              "grass tile tree probability should be 20");
    BANANA_TEST_ASSERT_INT_EQ(terrain_features_get_spawn_probability(2, TERRAIN_FEATURE_BERRY_BUSH),
                              10,
                              "grass tile berry bush probability should be 10");
    BANANA_TEST_ASSERT_INT_EQ(terrain_features_get_spawn_probability(2, TERRAIN_FEATURE_ORE_COPPER),
                              0,
                              "grass tile copper ore probability should be 0");

    /* GROVE tile */
    BANANA_TEST_ASSERT_INT_EQ(terrain_features_get_spawn_probability(3, TERRAIN_FEATURE_TREE),
                              70,
                              "grove tile tree probability should be 70");

    /* HILL tile */
    BANANA_TEST_ASSERT_INT_EQ(terrain_features_get_spawn_probability(4, TERRAIN_FEATURE_ORE_COPPER),
                              30,
                              "hill tile copper ore probability should be 30");
    BANANA_TEST_ASSERT_INT_EQ(terrain_features_get_spawn_probability(4, TERRAIN_FEATURE_ORE_IRON),
                              15,
                              "hill tile iron ore probability should be 15");

    /* RIDGE tile */
    BANANA_TEST_ASSERT_INT_EQ(terrain_features_get_spawn_probability(5, TERRAIN_FEATURE_ORE_COPPER),
                              25,
                              "ridge tile copper ore probability should be 25");

    /* WATER tile - nothing spawns */
    BANANA_TEST_ASSERT_INT_EQ(terrain_features_get_spawn_probability(0, TERRAIN_FEATURE_TREE),
                              0,
                              "water tile must have zero feature spawn probability");

    terrain_features_shutdown();
}

static void test_terrain_features_get_for_tile(void **state)
{
    (void)state;
    terrain_features_init(99999u);

    /* Each tile type should return a deterministic feature or NONE */
    TerrainFeatureType ft = terrain_features_get_for_tile(2, 100.0f, 100.0f);   /* GRASS */
    BANANA_TEST_ASSERT_TRUE(ft >= TERRAIN_FEATURE_NONE && ft <= TERRAIN_FEATURE_BERRY_BUSH,
                            "grass tile must return a valid feature type");

    ft = terrain_features_get_for_tile(3, 50.0f, 50.0f);    /* GROVE */
    BANANA_TEST_ASSERT_TRUE(ft >= TERRAIN_FEATURE_NONE && ft <= TERRAIN_FEATURE_BERRY_BUSH,
                            "grove tile must return a valid feature type");

    ft = terrain_features_get_for_tile(4, 0.0f, 0.0f);      /* HILL */
    BANANA_TEST_ASSERT_TRUE(ft >= TERRAIN_FEATURE_NONE && ft <= TERRAIN_FEATURE_BERRY_BUSH,
                            "hill tile must return a valid feature type");

    ft = terrain_features_get_for_tile(5, 0.0f, 0.0f);      /* RIDGE */
    BANANA_TEST_ASSERT_TRUE(ft >= TERRAIN_FEATURE_NONE && ft <= TERRAIN_FEATURE_BERRY_BUSH,
                            "ridge tile must return a valid feature type");

    ft = terrain_features_get_for_tile(0, 0.0f, 0.0f);      /* WATER */
    BANANA_TEST_ASSERT_INT_EQ((int)ft,
                              (int)TERRAIN_FEATURE_NONE,
                              "water tile must always return no feature");

    terrain_features_shutdown();
}

static void test_terrain_features_resource_yield_and_name(void **state)
{
    (void)state;
    terrain_features_init(1u);

    BANANA_TEST_ASSERT_TRUE(terrain_features_get_resource_yield(TERRAIN_FEATURE_TREE) > 0,
                            "tree feature must have positive resource yield");
    BANANA_TEST_ASSERT_TRUE(terrain_features_get_resource_yield(TERRAIN_FEATURE_ORE_COPPER) > 0,
                            "copper ore feature must have positive resource yield");
    BANANA_TEST_ASSERT_TRUE(terrain_features_get_resource_yield(TERRAIN_FEATURE_ORE_IRON) > 0,
                            "iron ore feature must have positive resource yield");
    BANANA_TEST_ASSERT_TRUE(terrain_features_get_resource_yield(TERRAIN_FEATURE_BERRY_BUSH) > 0,
                            "berry bush feature must have positive resource yield");
    BANANA_TEST_ASSERT_INT_EQ(terrain_features_get_resource_yield(TERRAIN_FEATURE_NONE),
                              0,
                              "no-feature must have zero resource yield");

    BANANA_TEST_ASSERT_TRUE(terrain_features_get_resource_name(TERRAIN_FEATURE_TREE) != NULL,
                            "tree feature must have a non-null resource name");
    BANANA_TEST_ASSERT_TRUE(terrain_features_get_resource_name(TERRAIN_FEATURE_NONE) != NULL,
                            "no-feature resource name must not be null");

    terrain_features_shutdown();
}

static void test_terrain_features_generate_for_chunk(void **state)
{
    (void)state;
    terrain_features_init(42u);

    uint8_t tile_types[81];   /* 9x9 */
    FeatureSpawn features[64];
    int i = 0;

    /* all grass */
    for (i = 0; i < 81; i++) tile_types[i] = 2;
    int n = terrain_features_generate_for_chunk(0, 0, tile_types, features, 64);
    BANANA_TEST_ASSERT_TRUE(n >= 0,
                            "generate for grass chunk must return non-negative count");

    /* all ridge */
    for (i = 0; i < 81; i++) tile_types[i] = 5;
    n = terrain_features_generate_for_chunk(3, 7, tile_types, features, 64);
    BANANA_TEST_ASSERT_TRUE(n >= 0,
                            "generate for ridge chunk must return non-negative count");

    /* null / zero guards */
    n = terrain_features_generate_for_chunk(0, 0, NULL, features, 64);
    BANANA_TEST_ASSERT_INT_EQ(n, 0,
                              "null tile array must produce zero features");
    n = terrain_features_generate_for_chunk(0, 0, tile_types, NULL, 64);
    BANANA_TEST_ASSERT_INT_EQ(n, 0,
                              "null output array must produce zero features");
    n = terrain_features_generate_for_chunk(0, 0, tile_types, features, 0);
    BANANA_TEST_ASSERT_INT_EQ(n, 0,
                              "zero max_features must produce zero features");

    terrain_features_shutdown();
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_terrain_features_init_and_spawn_probability),
    BANANA_TEST_CASE(test_terrain_features_get_for_tile),
    BANANA_TEST_CASE(test_terrain_features_resource_yield_and_name),
    BANANA_TEST_CASE(test_terrain_features_generate_for_chunk)
)
