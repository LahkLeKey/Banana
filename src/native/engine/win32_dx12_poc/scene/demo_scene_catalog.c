#include "demo_scene_catalog.h"

#include <stddef.h>
#include <string.h>

static const BananaPocDemoSceneCatalogEntry k_demo_scene_catalog[] = {
    {0, 1, BANANA_POC_DEMO_SCENE_KIND_CONTINENT, RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID, "port-koba", "continent-stem-territories", "Continent: Stem Territories", "continent/stem-territories-v1", "continent-slice"},
    {1, 1, BANANA_POC_DEMO_SCENE_KIND_CONTINENT, RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID, "frostmere", "continent-north-crown", "Continent: North Crown", "continent/north-crown-v1", "continent-slice"},
    {2, 1, BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_STATION, RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE, "port-koba", "banana-mainline-port-koba", "Banana Line Station: Port Koba", "continent/stem-territories-v1", "banana-line-station"},
    {3, 1, BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_CORRIDOR, RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST, RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE, "goldwave", "banana-mainline-sun-metro", "Banana Line Corridor: Sun Coast -> Metro", "continent/sun-coast-v1", "banana-line-corridor"},
    {4, 0, BANANA_POC_DEMO_SCENE_KIND_LAB, RUNTIME_TERRAIN_STATIC_MESH_REGION_IRON_SPINE, RUNTIME_TERRAIN_STATIC_MESH_REGION_GOLDEN_HOOK, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_SPINE_FREIGHT, "spinehold", "iron-spine-freight-lab", "Lab: Iron Spine Freight", "continent/iron-spine-v1", "lab-disabled"},
    {5, 0, BANANA_POC_DEMO_SCENE_KIND_CONTINENT, RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_FROST_FERRY, "icehook", "frost-peel-preview", "Preview: Frost Peel", "continent/frost-peel-v1", "preview-disabled"},
};

static int banana_poc_region_in_route(RuntimeTerrainBananaLineRouteId route_id,
                                      RuntimeTerrainStaticMeshRegionId region_id)
{
    const RuntimeTerrainBananaLineCorridor *corridor =
        runtime_terrain_banana_line_corridor_for_route(route_id);
    int stop_index = 0;

    if (!corridor)
        return 0;

    for (stop_index = 0; stop_index < corridor->stop_count; stop_index++)
    {
        if (corridor->stop_region_ids[stop_index] == region_id)
            return 1;
    }

    return 0;
}

static int banana_poc_route_has_adjacent_segment(RuntimeTerrainBananaLineRouteId route_id,
                                                 RuntimeTerrainStaticMeshRegionId from_region,
                                                 RuntimeTerrainStaticMeshRegionId to_region)
{
    const RuntimeTerrainBananaLineCorridor *corridor =
        runtime_terrain_banana_line_corridor_for_route(route_id);
    int stop_index = 1;

    if (!corridor)
        return 0;

    for (stop_index = 1; stop_index < corridor->stop_count; stop_index++)
    {
        if (corridor->stop_region_ids[stop_index - 1] == from_region &&
            corridor->stop_region_ids[stop_index] == to_region)
        {
            return 1;
        }
    }

    return 0;
}

static int banana_poc_region_has_county_anchor(RuntimeTerrainStaticMeshRegionId region_id,
                                               const char *county_id)
{
    int county_index = 0;

    if (!county_id || county_id[0] == '\0')
        return 0;

    for (county_index = 0; county_index < runtime_terrain_static_mesh_county_count(); county_index++)
    {
        const RuntimeTerrainStaticMeshCounty *county =
            runtime_terrain_static_mesh_county_at_index(county_index);

        if (!county)
            continue;

        if (county->region_id == region_id && strcmp(county->county_id, county_id) == 0)
            return 1;
    }

    return 0;
}

static unsigned int banana_poc_hash_u32(unsigned int value)
{
    value ^= value >> 16;
    value *= 0x7feb352du;
    value ^= value >> 15;
    value *= 0x846ca68bu;
    value ^= value >> 16;
    return value;
}

static unsigned int banana_poc_hash_string(unsigned int seed, const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    unsigned int hash = seed;

    if (!cursor)
        return banana_poc_hash_u32(hash ^ 0x9e3779b9u);

    while (*cursor)
    {
        hash ^= (unsigned int)(*cursor++);
        hash *= 16777619u;
    }

    return banana_poc_hash_u32(hash);
}

int banana_poc_demo_scene_catalog_count(void)
{
    return (int)(sizeof(k_demo_scene_catalog) / sizeof(k_demo_scene_catalog[0]));
}

const BananaPocDemoSceneCatalogEntry *banana_poc_demo_scene_catalog_at_index(int index)
{
    if (index < 0 || index >= banana_poc_demo_scene_catalog_count())
        return NULL;

    return &k_demo_scene_catalog[index];
}

const BananaPocDemoSceneCatalogEntry *banana_poc_demo_scene_catalog_for_browser_variant(int browser_variant)
{
    int index = banana_poc_demo_scene_catalog_index_for_browser_variant(browser_variant);

    if (index < 0)
        return NULL;

    return &k_demo_scene_catalog[index];
}

int banana_poc_demo_scene_catalog_index_for_browser_variant(int browser_variant)
{
    int index = 0;

    for (index = 0; index < banana_poc_demo_scene_catalog_count(); index++)
    {
        const BananaPocDemoSceneCatalogEntry *entry = &k_demo_scene_catalog[index];
        if (entry->browser_variant == browser_variant)
            return index;
    }

    return -1;
}

int banana_poc_demo_scene_catalog_clamp_index(int requested_index)
{
    int max_index = banana_poc_demo_scene_catalog_count() - 1;

    if (max_index < 0)
        return 0;

    if (requested_index < 0)
        return 0;

    if (requested_index > max_index)
        return max_index;

    return requested_index;
}

int banana_poc_demo_scene_catalog_first_enabled_index(void)
{
    int index = 0;

    for (index = 0; index < banana_poc_demo_scene_catalog_count(); index++)
    {
        if (banana_poc_demo_scene_catalog_is_launchable_index(index))
            return index;
    }

    return 0;
}

int banana_poc_demo_scene_catalog_is_launchable_index(int index)
{
    return banana_poc_demo_scene_catalog_validate_index(index) == BANANA_POC_DEMO_SCENE_VALIDATION_OK;
}

BananaPocDemoSceneValidationStatus banana_poc_demo_scene_catalog_validate_index(int index)
{
    const BananaPocDemoSceneCatalogEntry *entry = banana_poc_demo_scene_catalog_at_index(index);
    const BananaPocContinentAssetPack *pack = NULL;

    if (!entry)
        return BANANA_POC_DEMO_SCENE_VALIDATION_UNKNOWN_SCENE;

    if (!entry->enabled)
        return BANANA_POC_DEMO_SCENE_VALIDATION_DISABLED;

    if (!banana_poc_continent_asset_pack_is_registered(entry->asset_pack_id))
        return BANANA_POC_DEMO_SCENE_VALIDATION_MISSING_ASSET_PACK;

    pack = banana_poc_continent_asset_pack_for_region(entry->primary_region_id);
    if (!pack || !pack->asset_pack_id || strcmp(pack->asset_pack_id, entry->asset_pack_id) != 0)
        return BANANA_POC_DEMO_SCENE_VALIDATION_CANONICAL_REGION_MISMATCH;

    if (!banana_poc_region_has_county_anchor(entry->primary_region_id, entry->anchor_county_id))
        return BANANA_POC_DEMO_SCENE_VALIDATION_ANCHOR_MISMATCH;

    if (entry->kind == BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_STATION)
    {
        if (entry->route_id == RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID ||
            !banana_poc_region_in_route(entry->route_id, entry->primary_region_id))
        {
            return BANANA_POC_DEMO_SCENE_VALIDATION_ROUTE_MISMATCH;
        }
    }

    if (entry->kind == BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_CORRIDOR)
    {
        if (entry->secondary_region_id == RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID ||
            entry->route_id == RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID ||
            !banana_poc_route_has_adjacent_segment(entry->route_id,
                                                   entry->primary_region_id,
                                                   entry->secondary_region_id))
        {
            return BANANA_POC_DEMO_SCENE_VALIDATION_ROUTE_MISMATCH;
        }
    }

    return BANANA_POC_DEMO_SCENE_VALIDATION_OK;
}

const char *banana_poc_demo_scene_catalog_validation_message(BananaPocDemoSceneValidationStatus status)
{
    switch (status)
    {
    case BANANA_POC_DEMO_SCENE_VALIDATION_OK:
        return "ok";
    case BANANA_POC_DEMO_SCENE_VALIDATION_UNKNOWN_SCENE:
        return "unknown-scene";
    case BANANA_POC_DEMO_SCENE_VALIDATION_DISABLED:
        return "scene-disabled";
    case BANANA_POC_DEMO_SCENE_VALIDATION_MISSING_ASSET_PACK:
        return "missing-asset-pack";
    case BANANA_POC_DEMO_SCENE_VALIDATION_CANONICAL_REGION_MISMATCH:
        return "canonical-region-pack-mismatch";
    case BANANA_POC_DEMO_SCENE_VALIDATION_ROUTE_MISMATCH:
        return "route-anchor-mismatch";
    case BANANA_POC_DEMO_SCENE_VALIDATION_ANCHOR_MISMATCH:
        return "county-anchor-mismatch";
    default:
        return "unknown-validation-status";
    }
}

const char *banana_poc_demo_scene_catalog_display_name_for_variant(int browser_variant)
{
    const BananaPocDemoSceneCatalogEntry *entry =
        banana_poc_demo_scene_catalog_for_browser_variant(browser_variant);

    if (!entry)
        return "Unknown Scene Variant";

    return entry->display_name ? entry->display_name : "Unnamed Scene Variant";
}

unsigned int banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(int browser_variant)
{
    const BananaPocDemoSceneCatalogEntry *entry =
        banana_poc_demo_scene_catalog_for_browser_variant(browser_variant);
    unsigned int hash = 2166136261u;

    if (!entry)
        return 0u;

    hash = banana_poc_hash_string(hash, entry->scene_key);
    hash = banana_poc_hash_string(hash, entry->asset_pack_id);
    hash = banana_poc_hash_string(hash, entry->diagnostics_tag);
    hash ^= (unsigned int)(entry->browser_variant + 31);
    hash ^= (unsigned int)(entry->enabled + 131);
    hash ^= (unsigned int)(entry->kind + 257);
    hash ^= (unsigned int)(entry->primary_region_id + 521);
    hash ^= (unsigned int)(entry->route_id + 1021);

    return banana_poc_hash_u32(hash);
}
