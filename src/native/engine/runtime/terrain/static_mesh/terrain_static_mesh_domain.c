#include "terrain_static_mesh_domain.h"

#include <stddef.h>
#include <string.h>

static const RuntimeTerrainStaticMeshProfile k_runtime_static_mesh_profiles[RUNTIME_TERRAIN_STATIC_MESH_REGION_COUNT] = {
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES, "stem-territories", "port-koba", "banana-line-stem", "tropical", "island-chain", "volcanic", "agriculture|fishing", "medium", "freight-port|naval-base|ferry-node", -210, 130, 78, 1u},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST, "sun-coast", "goldwave", "banana-line-sun", "warm-temperate", "lowland", "cyclone", "agriculture|fishing|energy", "dense", "freight-port|rail-hub", -130, 95, 74, 1u},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA, "metro-banana", "neo-musa", "banana-line-metro", "warm-temperate", "lowland", "seismic", "finance|manufacturing", "megacity", "rail-hub|freight-port|tunnel-corridor", -20, 80, 82, 1u},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_CENTRAL_CRESCENT, "central-crescent", "yelora", "banana-line-crescent", "temperate", "basin", "landslide", "culture|agriculture|education", "dense", "rail-hub|tunnel-corridor", 55, 68, 72, 1u},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_IRON_SPINE, "iron-spine", "spinehold", "banana-line-spine", "alpine", "ridge", "landslide", "mining|manufacturing|energy", "medium", "dam-network|tunnel-corridor|rail-hub", 122, 38, 66, 1u},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_GOLDEN_HOOK, "golden-hook", "hookport", "banana-line-hook", "subarctic", "cliff-coast", "cyclone", "fishing|shipbuilding", "medium", "freight-port|rail-hub|naval-base", 188, 22, 64, 1u},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN, "north-crown", "frostmere", "banana-line-crown", "subarctic", "plateau", "blizzard", "energy|fishing", "sparse", "rail-hub|dam-network", 246, -18, 60, 1u},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL, "frost-peel", "icehook", "banana-line-frost", "polar-maritime", "island-chain", "blizzard|rough-sea", "research|military|fishing", "sparse", "ferry-node|naval-base", 322, -94, 54, 2u},
};

static const RuntimeTerrainStaticMeshCounty k_runtime_static_mesh_counties[] = {
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES, "port-koba", 0, 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES, "ashen-reef", -10, 5},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES, "minari-gulf", 12, -6},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES, "south-stem", -18, -8},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST, "goldwave", 0, 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST, "citrine-bay", 10, 4},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST, "lowleaf", -12, -3},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST, "haruna-coast", 18, -6},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA, "neo-musa", 0, 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA, "central-peel", 8, 3},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA, "kiro-district", -7, 4},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA, "east-arc", 16, -5},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_CENTRAL_CRESCENT, "yelora", 0, 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_CENTRAL_CRESCENT, "crescent-vale", 9, -4},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_CENTRAL_CRESCENT, "old-peel", -8, 6},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_CENTRAL_CRESCENT, "nami-basin", 14, 8},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_IRON_SPINE, "spinehold", 0, 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_IRON_SPINE, "black-ridge", -9, 7},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_IRON_SPINE, "north-forge", 12, 6},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_IRON_SPINE, "rail-pass", 5, -10},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_GOLDEN_HOOK, "hookport", 0, 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_GOLDEN_HOOK, "gale-bay", -7, 5},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_GOLDEN_HOOK, "amber-reach", 10, -3},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_GOLDEN_HOOK, "cliffwater", 16, 6},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN, "frostmere", 0, 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN, "white-pine", -9, 8},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN, "crown-plateau", 11, -4},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN, "elk-valley", 4, 10},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL, "icehook", 0, 0},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL, "outer-peel", 12, 6},
    {RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL, "north-watch", -10, 9},
};

static const RuntimeTerrainBananaLineCorridor k_runtime_banana_line_corridors[] = {
    {RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE, "banana-mainline", {RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES, RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST, RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA, RUNTIME_TERRAIN_STATIC_MESH_REGION_CENTRAL_CRESCENT, RUNTIME_TERRAIN_STATIC_MESH_REGION_GOLDEN_HOOK, RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID}, 6},
    {RUNTIME_TERRAIN_BANANA_LINE_ROUTE_SPINE_FREIGHT, "banana-spine-freight", {RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST, RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA, RUNTIME_TERRAIN_STATIC_MESH_REGION_IRON_SPINE, RUNTIME_TERRAIN_STATIC_MESH_REGION_GOLDEN_HOOK, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID}, 4},
    {RUNTIME_TERRAIN_BANANA_LINE_ROUTE_FROST_FERRY, "banana-frost-ferry", {RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN, RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID}, 2},
};

int runtime_terrain_static_mesh_profile_count(void)
{
    return RUNTIME_TERRAIN_STATIC_MESH_REGION_COUNT;
}

int runtime_terrain_static_mesh_profile_index_for_region_id(RuntimeTerrainStaticMeshRegionId region_id)
{
    if (region_id < RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES || region_id >= RUNTIME_TERRAIN_STATIC_MESH_REGION_COUNT)
        return -1;

    return (int)region_id;
}

const RuntimeTerrainStaticMeshProfile *runtime_terrain_static_mesh_profile_at_index(int profile_index)
{
    if (profile_index < 0 || profile_index >= runtime_terrain_static_mesh_profile_count())
        return NULL;

    return &k_runtime_static_mesh_profiles[profile_index];
}

const RuntimeTerrainStaticMeshProfile *runtime_terrain_static_mesh_profile_for_region(RuntimeTerrainStaticMeshRegionId region_id)
{
    int profile_index = runtime_terrain_static_mesh_profile_index_for_region_id(region_id);

    if (profile_index < 0)
        return NULL;

    return &k_runtime_static_mesh_profiles[profile_index];
}

RuntimeTerrainStaticMeshRegionId runtime_terrain_static_mesh_region_id_from_slug(const char *region_slug)
{
    int profile_index = 0;

    if (!region_slug)
        return RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID;

    for (profile_index = 0; profile_index < runtime_terrain_static_mesh_profile_count(); profile_index++)
    {
        const RuntimeTerrainStaticMeshProfile *profile = &k_runtime_static_mesh_profiles[profile_index];
        if (strcmp(profile->region_slug, region_slug) == 0)
            return profile->region_id;
    }

    return RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID;
}

const char *runtime_terrain_static_mesh_region_slug(RuntimeTerrainStaticMeshRegionId region_id)
{
    const RuntimeTerrainStaticMeshProfile *profile = runtime_terrain_static_mesh_profile_for_region(region_id);

    if (!profile)
        return NULL;

    return profile->region_slug;
}

int runtime_terrain_static_mesh_county_count(void)
{
    return (int)(sizeof(k_runtime_static_mesh_counties) / sizeof(k_runtime_static_mesh_counties[0]));
}

const RuntimeTerrainStaticMeshCounty *runtime_terrain_static_mesh_county_at_index(int county_index)
{
    if (county_index < 0 || county_index >= runtime_terrain_static_mesh_county_count())
        return NULL;

    return &k_runtime_static_mesh_counties[county_index];
}

int runtime_terrain_banana_line_corridor_count(void)
{
    return (int)(sizeof(k_runtime_banana_line_corridors) / sizeof(k_runtime_banana_line_corridors[0]));
}

const RuntimeTerrainBananaLineCorridor *runtime_terrain_banana_line_corridor_at_index(int corridor_index)
{
    if (corridor_index < 0 || corridor_index >= runtime_terrain_banana_line_corridor_count())
        return NULL;

    return &k_runtime_banana_line_corridors[corridor_index];
}

const RuntimeTerrainBananaLineCorridor *runtime_terrain_banana_line_corridor_for_route(RuntimeTerrainBananaLineRouteId route_id)
{
    int corridor_index = 0;

    if (route_id < RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE || route_id >= RUNTIME_TERRAIN_BANANA_LINE_ROUTE_COUNT)
        return NULL;

    for (corridor_index = 0; corridor_index < runtime_terrain_banana_line_corridor_count(); corridor_index++)
    {
        const RuntimeTerrainBananaLineCorridor *corridor = &k_runtime_banana_line_corridors[corridor_index];
        if (corridor->route_id == route_id)
            return corridor;
    }

    return NULL;
}

RuntimeTerrainBananaLineRouteId runtime_terrain_banana_line_route_id_from_slug(const char *route_slug)
{
    int corridor_index = 0;

    if (!route_slug)
        return RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID;

    for (corridor_index = 0; corridor_index < runtime_terrain_banana_line_corridor_count(); corridor_index++)
    {
        const RuntimeTerrainBananaLineCorridor *corridor = &k_runtime_banana_line_corridors[corridor_index];
        if (strcmp(corridor->route_slug, route_slug) == 0)
            return corridor->route_id;
    }

    return RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID;
}

const char *runtime_terrain_banana_line_route_slug(RuntimeTerrainBananaLineRouteId route_id)
{
    const RuntimeTerrainBananaLineCorridor *corridor = runtime_terrain_banana_line_corridor_for_route(route_id);

    if (!corridor)
        return NULL;

    return corridor->route_slug;
}
