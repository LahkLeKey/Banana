#include "runtime/terrain/static_mesh/terrain_static_mesh_domain.h"

#include "../../support/test_support.h"

static void test_profile_and_county_lookup_paths(void **state)
{
    (void)state;
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_static_mesh_profile_count(),
                              RUNTIME_TERRAIN_STATIC_MESH_REGION_COUNT,
                              "profile count must match region count constant");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_static_mesh_profile_index_for_region_id(RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID),
                              -1,
                              "invalid region id must map to -1 index");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_static_mesh_profile_index_for_region_id(RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES),
                              0,
                              "first valid region id must map to index 0");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_static_mesh_profile_at_index(-1) == NULL,
                            "negative profile index must return NULL");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_static_mesh_profile_at_index(999) == NULL,
                            "out-of-range profile index must return NULL");
    const RuntimeTerrainStaticMeshProfile *profile = runtime_terrain_static_mesh_profile_for_region(RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA);
    BANANA_TEST_ASSERT_TRUE(profile != NULL,
                            "known region must return a profile");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_static_mesh_region_id_from_slug("metro-banana") == RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA,
                            "known region slug must resolve to region id");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_static_mesh_region_id_from_slug(NULL) == RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID,
                            "null region slug must return invalid region id");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_static_mesh_region_id_from_slug("unknown") == RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID,
                            "unknown region slug must return invalid region id");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_static_mesh_region_slug(RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA) != NULL,
                            "known region id must return non-null slug");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_static_mesh_region_slug(RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID) == NULL,
                            "invalid region id must return NULL slug");

    BANANA_TEST_ASSERT_TRUE(runtime_terrain_static_mesh_county_count() > 0,
                            "county count must be positive");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_static_mesh_county_at_index(-1) == NULL,
                            "negative county index must return NULL");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_static_mesh_county_at_index(999) == NULL,
                            "out-of-range county index must return NULL");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_static_mesh_county_at_index(0) != NULL,
                            "first county entry must exist");
}

static void test_corridor_lookup_and_route_generation_paths(void **state)
{
    (void)state;
    int steps[2048][2] = {{0}};
    int step_count = 0;

    BANANA_TEST_ASSERT_TRUE(runtime_terrain_banana_line_corridor_count() > 0,
                            "corridor count must be positive");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_banana_line_corridor_at_index(-1) == NULL,
                            "negative corridor index must return NULL");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_banana_line_corridor_at_index(999) == NULL,
                            "out-of-range corridor index must return NULL");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_banana_line_corridor_for_route(RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE) != NULL,
                            "known route id must resolve to a corridor");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_banana_line_corridor_for_route(RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID) == NULL,
                            "invalid route id must return NULL corridor");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_banana_line_route_id_from_slug("banana-mainline") == RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE,
                            "known route slug must resolve to route id");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_banana_line_route_id_from_slug(NULL) == RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID,
                            "null route slug must return invalid route id");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_banana_line_route_slug(RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE) != NULL,
                            "known route id must return slug");
    BANANA_TEST_ASSERT_TRUE(runtime_terrain_banana_line_route_slug(RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID) == NULL,
                            "invalid route id must return NULL slug");

    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_banana_line_route_between_regions(RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES,
                                                                                RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST,
                                                                                steps,
                                                                                2048,
                                                                                &step_count),
                              1,
                              "route_between_regions must succeed for valid regions");
    BANANA_TEST_ASSERT_TRUE(step_count > 0,
                            "route_between_regions must produce at least one step");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_banana_line_route_between_regions(RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID,
                                                                                RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST,
                                                                                steps,
                                                                                2048,
                                                                                &step_count),
                              0,
                              "route_between_regions must fail for invalid source region");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_banana_line_route_for_id(RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE,
                                                                       steps,
                                                                       2048,
                                                                       &step_count),
                              1,
                              "route_for_id must succeed for known route");
    BANANA_TEST_ASSERT_TRUE(step_count > 0,
                            "route_for_id must produce path steps");
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_banana_line_route_for_id(RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID,
                                                                       steps,
                                                                       2048,
                                                                       &step_count),
                              0,
                              "route_for_id must fail for invalid route id");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_profile_and_county_lookup_paths),
    BANANA_TEST_CASE(test_corridor_lookup_and_route_generation_paths)
)
