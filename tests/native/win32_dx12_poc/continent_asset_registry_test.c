#include "win32_dx12_poc/scene/continent_asset_registry.h"

#include "../runtime/support/test_support.h"

static void test_continent_asset_registry_lookups(void **state)
{
    (void)state;
    BANANA_TEST_ASSERT_TRUE(banana_poc_continent_asset_pack_count() > 0,
                            "continent asset pack count must be positive");
    BANANA_TEST_ASSERT_TRUE(banana_poc_continent_asset_pack_at_index(-1) == NULL,
                            "negative asset pack index must return NULL");
    BANANA_TEST_ASSERT_TRUE(banana_poc_continent_asset_pack_at_index(999) == NULL,
                            "out-of-range asset pack index must return NULL");
    const BananaPocContinentAssetPack *pack = banana_poc_continent_asset_pack_for_region(RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA);
    BANANA_TEST_ASSERT_TRUE(pack != NULL,
                            "known region must resolve to an asset pack");
    BANANA_TEST_ASSERT_TRUE(banana_poc_continent_asset_pack_is_registered(pack->asset_pack_id),
                            "known asset pack id must be registered");
    BANANA_TEST_ASSERT_TRUE(!banana_poc_continent_asset_pack_is_registered(NULL),
                            "null asset pack id must be rejected");
    BANANA_TEST_ASSERT_TRUE(!banana_poc_continent_asset_pack_is_registered("unknown-pack"),
                            "unknown asset pack id must not be registered");
    BANANA_TEST_ASSERT_TRUE(banana_poc_continent_asset_pack_has_gameplay_models(pack),
                            "known asset pack must have gameplay models");
    BANANA_TEST_ASSERT_TRUE(!banana_poc_continent_asset_pack_has_gameplay_models(NULL),
                            "null pack must report no gameplay models");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_continent_asset_registry_lookups)
)
