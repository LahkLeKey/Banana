#include "continent_asset_registry.h"

#include <string.h>

static const BananaPocContinentAssetPack k_continent_asset_packs[] = {
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES, "stem-territories", "continent/stem-territories-v1", "terrain/basic-overworld", 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST, "sun-coast", "continent/sun-coast-v1", "terrain/basic-overworld", 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA, "metro-banana", "continent/metro-banana-v1", "terrain/basic-overworld", 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_CENTRAL_CRESCENT, "central-crescent", "continent/central-crescent-v1", "terrain/basic-overworld", 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_IRON_SPINE, "iron-spine", "continent/iron-spine-v1", "terrain/basic-overworld", 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_GOLDEN_HOOK, "golden-hook", "continent/golden-hook-v1", "terrain/basic-overworld", 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN, "north-crown", "continent/north-crown-v1", "terrain/basic-overworld", 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL, "frost-peel", "continent/frost-peel-v1", "terrain/basic-overworld", 0},
};

int banana_poc_continent_asset_pack_count(void)
{
    return (int)(sizeof(k_continent_asset_packs) / sizeof(k_continent_asset_packs[0]));
}

const BananaPocContinentAssetPack *banana_poc_continent_asset_pack_at_index(int index)
{
    if (index < 0 || index >= banana_poc_continent_asset_pack_count())
        return NULL;

    return &k_continent_asset_packs[index];
}

const BananaPocContinentAssetPack *banana_poc_continent_asset_pack_for_region(RuntimeTerrainStaticMeshRegionId region_id)
{
    int index = 0;

    for (index = 0; index < banana_poc_continent_asset_pack_count(); index++)
    {
        const BananaPocContinentAssetPack *pack = &k_continent_asset_packs[index];
        if (pack->region_id == region_id)
            return pack;
    }

    return NULL;
}

int banana_poc_continent_asset_pack_is_registered(const char *asset_pack_id)
{
    int index = 0;

    if (!asset_pack_id || asset_pack_id[0] == '\0')
        return 0;

    for (index = 0; index < banana_poc_continent_asset_pack_count(); index++)
    {
        const BananaPocContinentAssetPack *pack = &k_continent_asset_packs[index];
        if (pack->asset_pack_id && strcmp(pack->asset_pack_id, asset_pack_id) == 0)
            return 1;
    }

    return 0;
}
