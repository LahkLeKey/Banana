#include "demo_scene_catalog.h"
#include "../../runtime/engine/gameplay_model_profile.h"

#include <stddef.h>
#include <string.h>

static const BananaPocDemoSceneCatalogEntry k_demo_scene_catalog[] = {
    {0, 1, BANANA_POC_DEMO_SCENE_KIND_CONTINENT, RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID, "port-koba", "continent-stem-territories", "Continent: Stem Territories", "continent/stem-territories-v1", "continent-slice"},
    {1, 1, BANANA_POC_DEMO_SCENE_KIND_CONTINENT, RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID, "frostmere", "continent-north-crown", "Continent: North Crown", "continent/north-crown-v1", "continent-slice"},
    {2, 1, BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_STATION, RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE, "neo-musa", "banana-mainline-neo-musa", "Banana Line Station: Neo Musa", "continent/metro-banana-v1", "banana-line-station"},
    {3, 1, BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_CORRIDOR, RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA, RUNTIME_TERRAIN_STATIC_MESH_REGION_CENTRAL_CRESCENT, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE, "neo-musa", "banana-mainline-metro-crescent", "Banana Line Corridor: Metro -> Crescent", "continent/metro-banana-v1", "banana-line-corridor"},
    {4, 0, BANANA_POC_DEMO_SCENE_KIND_LAB, RUNTIME_TERRAIN_STATIC_MESH_REGION_IRON_SPINE, RUNTIME_TERRAIN_STATIC_MESH_REGION_GOLDEN_HOOK, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_SPINE_FREIGHT, "spinehold", "iron-spine-freight-lab", "Lab: Iron Spine Freight", "continent/iron-spine-v1", "lab-disabled"},
    {5, 0, BANANA_POC_DEMO_SCENE_KIND_CONTINENT, RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL, RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID, RUNTIME_TERRAIN_BANANA_LINE_ROUTE_FROST_FERRY, "icehook", "frost-peel-preview", "Preview: Frost Peel", "continent/frost-peel-v1", "preview-disabled"},
};

typedef struct BananaPocGameplayModelCatalogEntry
{
    const char *model_key;
    const char *model_id;
    int enabled;
} BananaPocGameplayModelCatalogEntry;

typedef struct BananaPocGameplayPlacementTemplate
{
    int browser_variant;
    BananaPocGameplayPlacementRole role;
    BananaPocGameplayTeam team;
    const char *model_key;
    const char *fallback_model_key;
    const char *diagnostics_tag;
    float x;
    float y;
    float z;
} BananaPocGameplayPlacementTemplate;

typedef struct BananaPocGameplayModelOverrideEntry
{
    const char *model_key;
    const char *model_id;
} BananaPocGameplayModelOverrideEntry;

static const BananaPocGameplayModelCatalogEntry k_demo_scene_gameplay_model_catalog[] = {
    {"reference-banana", "gameplay/reference/banana-basic-v1", 1},
    {"reference-bean", "gameplay/reference/banana-bean-green-v1", 1},
    {"tropical-landmark", "gameplay/tropical-coastal/volcanic-arch-v1", 1},
    {"tropical-traversal", "gameplay/tropical-coastal/palm-cluster-v1", 1},
    {"rugged-landmark", "gameplay/rugged-wild/ice-gate-v1", 1},
    {"rugged-traversal", "gameplay/rugged-wild/pine-cluster-v1", 1},
    {"urban-station-landmark", "gameplay/urban-industrial/rail-hub-v1", 1},
    {"urban-station-traversal", "gameplay/urban-industrial/platform-barrier-v1", 1},
    {"urban-corridor-landmark", "gameplay/urban-industrial/archive-gate-v1", 1},
    {"urban-corridor-traversal", "gameplay/urban-industrial/market-stall-v1", 1},
};

static const BananaPocGameplayPlacementTemplate k_demo_scene_gameplay_placement_templates[] = {
    {0, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_REFERENCE, BANANA_POC_GAMEPLAY_TEAM_BANANA, "reference-banana", NULL, "reference-anchor", -2.0f, 0.0f, 1.0f},
    {0, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK, BANANA_POC_GAMEPLAY_TEAM_BEAN, "reference-bean", "reference-banana", "bean-landmark-cluster", 4.0f, 0.0f, -3.5f},
    {0, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_TRAVERSAL, BANANA_POC_GAMEPLAY_TEAM_BANANA, "tropical-traversal", "reference-banana", "traversal-route", 1.5f, 0.0f, 6.0f},
    {1, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_REFERENCE, BANANA_POC_GAMEPLAY_TEAM_BANANA, "reference-banana", NULL, "reference-anchor", -1.0f, 0.0f, 0.5f},
    {1, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK, BANANA_POC_GAMEPLAY_TEAM_BANANA, "rugged-landmark", "reference-banana", "landmark-gate", 6.0f, 0.0f, -4.0f},
    {1, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_TRAVERSAL, BANANA_POC_GAMEPLAY_TEAM_BEAN, "reference-bean", "reference-banana", "bean-raider-route", 2.0f, 0.0f, 5.0f},
    {2, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_REFERENCE, BANANA_POC_GAMEPLAY_TEAM_BANANA, "reference-banana", NULL, "reference-anchor", -1.5f, 0.0f, 0.0f},
    {2, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK, BANANA_POC_GAMEPLAY_TEAM_BANANA, "urban-station-landmark", "reference-banana", "station-landmark", 3.0f, 0.0f, -2.0f},
    {2, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_TRAVERSAL, BANANA_POC_GAMEPLAY_TEAM_BEAN, "reference-bean", "reference-banana", "bean-station-skirmish", 0.5f, 0.0f, 4.5f},
    {3, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_REFERENCE, BANANA_POC_GAMEPLAY_TEAM_BANANA, "reference-banana", NULL, "reference-anchor", -0.5f, 0.0f, -1.0f},
    {3, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK, BANANA_POC_GAMEPLAY_TEAM_BEAN, "reference-bean", "reference-banana", "bean-corridor-front", 5.0f, 0.0f, -3.0f},
    {3, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_TRAVERSAL, BANANA_POC_GAMEPLAY_TEAM_BANANA, "urban-corridor-traversal", "reference-banana", "corridor-traversal", 1.0f, 0.0f, 5.5f},
};

#define BANANA_POC_GAMEPLAY_MODEL_OVERRIDE_CAPACITY 16
static BananaPocGameplayModelOverrideEntry s_gameplay_model_overrides[BANANA_POC_GAMEPLAY_MODEL_OVERRIDE_CAPACITY];
static int s_gameplay_model_override_count = 0;
static BananaPocDemoSceneGameplayPlacement s_demo_scene_resolved_gameplay_placements[
    sizeof(k_demo_scene_gameplay_placement_templates) / sizeof(k_demo_scene_gameplay_placement_templates[0])];
static int s_demo_scene_resolved_gameplay_placements_dirty = 1;

static const char *banana_poc_demo_scene_gameplay_model_id_for_key_with_overrides(const char *model_key)
{
    int index = 0;

    if (!model_key || model_key[0] == '\0')
        return NULL;

    for (index = 0; index < s_gameplay_model_override_count; index++)
    {
        if (s_gameplay_model_overrides[index].model_key &&
            strcmp(s_gameplay_model_overrides[index].model_key, model_key) == 0)
        {
            return s_gameplay_model_overrides[index].model_id;
        }
    }

    for (index = 0; index < (int)(sizeof(k_demo_scene_gameplay_model_catalog) / sizeof(k_demo_scene_gameplay_model_catalog[0])); index++)
    {
        const BananaPocGameplayModelCatalogEntry *entry = &k_demo_scene_gameplay_model_catalog[index];
        if (entry->enabled && entry->model_key && strcmp(entry->model_key, model_key) == 0)
            return entry->model_id;
    }

    return NULL;
}

static void banana_poc_demo_scene_rebuild_resolved_placements(void)
{
    int index = 0;

    if (!s_demo_scene_resolved_gameplay_placements_dirty)
        return;

    for (index = 0; index < (int)(sizeof(k_demo_scene_gameplay_placement_templates) / sizeof(k_demo_scene_gameplay_placement_templates[0])); index++)
    {
        const BananaPocGameplayPlacementTemplate *template = &k_demo_scene_gameplay_placement_templates[index];
        BananaPocDemoSceneGameplayPlacement *resolved = &s_demo_scene_resolved_gameplay_placements[index];

        resolved->browser_variant = template->browser_variant;
        resolved->role = template->role;
        resolved->team = template->team;
        resolved->model_id = banana_poc_demo_scene_gameplay_model_id_for_key_with_overrides(template->model_key);
        resolved->fallback_model_id = banana_poc_demo_scene_gameplay_model_id_for_key_with_overrides(template->fallback_model_key);
        resolved->diagnostics_tag = template->diagnostics_tag;
        resolved->x = template->x;
        resolved->y = template->y;
        resolved->z = template->z;
    }

    s_demo_scene_resolved_gameplay_placements_dirty = 0;
}

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

static int banana_poc_demo_scene_profile_matches_catalog_identity(const BananaPocCoherentWorldProfile *profile)
{
    const BananaPocDemoSceneCatalogEntry *entry = NULL;
    const BananaPocContinentAssetPack *pack = NULL;

    if (!profile)
        return 0;

    entry = banana_poc_demo_scene_catalog_for_browser_variant(profile->browser_variant);
    if (!entry)
        return 0;

    pack = banana_poc_continent_asset_pack_for_region(entry->primary_region_id);

    if (profile->kind != entry->kind)
        return 0;

    if (profile->primary_region_id != entry->primary_region_id ||
        profile->secondary_region_id != entry->secondary_region_id ||
        profile->route_id != entry->route_id)
    {
        return 0;
    }

    if (!profile->scene_key || !entry->scene_key || strcmp(profile->scene_key, entry->scene_key) != 0)
        return 0;

    if (!profile->asset_pack_id || !entry->asset_pack_id || strcmp(profile->asset_pack_id, entry->asset_pack_id) != 0)
        return 0;

    if (!profile->anchor_county_id || !entry->anchor_county_id ||
        strcmp(profile->anchor_county_id, entry->anchor_county_id) != 0)
    {
        return 0;
    }

    if (!profile->gameplay_theme_id ||
        !pack || !pack->gameplay_theme_id ||
        strcmp(profile->gameplay_theme_id, pack->gameplay_theme_id) != 0)
    {
        return 0;
    }

    return 1;
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

static BananaPocDemoSceneValidationStatus banana_poc_demo_scene_validate_gameplay_records(int browser_variant)
{
    int placement_count = banana_poc_demo_scene_gameplay_placement_count_for_variant(browser_variant);
    int placement_index = 0;
    int saw_landmark = 0;
    int saw_traversal = 0;

    if (placement_count < 2)
        return BANANA_POC_DEMO_SCENE_VALIDATION_GAMEPLAY_MODEL_MISMATCH;

    for (placement_index = 0; placement_index < placement_count; placement_index++)
    {
        const BananaPocDemoSceneGameplayPlacement *placement =
            banana_poc_demo_scene_gameplay_placement_at(browser_variant, placement_index);

        if (!placement)
            return BANANA_POC_DEMO_SCENE_VALIDATION_GAMEPLAY_MODEL_MISMATCH;

        if ((!placement->model_id || placement->model_id[0] == '\0') &&
            (!placement->fallback_model_id || placement->fallback_model_id[0] == '\0'))
        {
            return BANANA_POC_DEMO_SCENE_VALIDATION_GAMEPLAY_MODEL_MISMATCH;
        }

        if (!placement->diagnostics_tag || placement->diagnostics_tag[0] == '\0')
            return BANANA_POC_DEMO_SCENE_VALIDATION_GAMEPLAY_MODEL_MISMATCH;

        if (placement->role == BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK)
            saw_landmark = 1;

        if (placement->role == BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_TRAVERSAL)
            saw_traversal = 1;
    }

    if (!saw_landmark || !saw_traversal)
        return BANANA_POC_DEMO_SCENE_VALIDATION_GAMEPLAY_MODEL_MISMATCH;

    return BANANA_POC_DEMO_SCENE_VALIDATION_OK;
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

BananaPocDemoSceneValidationStatus banana_poc_demo_scene_catalog_validate_gameplay_for_variant(int browser_variant)
{
    return banana_poc_demo_scene_validate_gameplay_records(browser_variant);
}

BananaPocDemoSceneValidationStatus banana_poc_demo_scene_catalog_validate_index(int index)
{
    const BananaPocDemoSceneCatalogEntry *entry = banana_poc_demo_scene_catalog_at_index(index);
    const BananaPocContinentAssetPack *pack = NULL;
    BananaPocDemoSceneValidationStatus gameplay_status = BANANA_POC_DEMO_SCENE_VALIDATION_OK;

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

    gameplay_status = banana_poc_demo_scene_validate_gameplay_records(entry->browser_variant);
    if (gameplay_status != BANANA_POC_DEMO_SCENE_VALIDATION_OK)
        return gameplay_status;

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
    case BANANA_POC_DEMO_SCENE_VALIDATION_GAMEPLAY_MODEL_MISMATCH:
        return "gameplay-model-mismatch";
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

const char *banana_poc_demo_scene_catalog_gameplay_theme_for_variant(int browser_variant)
{
    const BananaPocDemoSceneCatalogEntry *entry =
        banana_poc_demo_scene_catalog_for_browser_variant(browser_variant);
    const BananaPocContinentAssetPack *pack = NULL;

    if (!entry)
        return "unknown-theme";

    pack = banana_poc_continent_asset_pack_for_region(entry->primary_region_id);

    if (!pack || !pack->gameplay_theme_id || pack->gameplay_theme_id[0] == '\0')
        return "unknown-theme";

    return pack->gameplay_theme_id;
}

int banana_poc_demo_scene_catalog_war_policy_for_variant(int browser_variant,
                                                         float *out_radius,
                                                         int *out_reinforcements_per_tick,
                                                         int *out_population_cap)
{
    const BananaPocDemoSceneCatalogEntry *entry =
        banana_poc_demo_scene_catalog_for_browser_variant(browser_variant);
    float radius = 8.0f;
    int reinforcements_per_tick = 2;
    int population_cap = 96;

    if (!entry || !out_radius || !out_reinforcements_per_tick || !out_population_cap)
        return 0;

    if (entry->kind == BANANA_POC_DEMO_SCENE_KIND_CONTINENT)
    {
        radius = 12.0f;
        reinforcements_per_tick = 2;
        population_cap = 128;
    }
    else if (entry->kind == BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_STATION)
    {
        radius = 8.0f;
        reinforcements_per_tick = 3;
        population_cap = 144;
    }
    else if (entry->kind == BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_CORRIDOR)
    {
        radius = 6.0f;
        reinforcements_per_tick = 4;
        population_cap = 176;
    }
    else if (entry->kind == BANANA_POC_DEMO_SCENE_KIND_LAB)
    {
        radius = 10.0f;
        reinforcements_per_tick = 1;
        population_cap = 64;
    }

    *out_radius = radius;
    *out_reinforcements_per_tick = reinforcements_per_tick;
    *out_population_cap = population_cap;
    return 1;
}

int banana_poc_demo_scene_catalog_build_coherent_profile(int browser_variant,
                                                         BananaPocCoherentWorldProfile *out_profile)
{
    const BananaPocDemoSceneCatalogEntry *entry =
        banana_poc_demo_scene_catalog_for_browser_variant(browser_variant);
    const BananaPocContinentAssetPack *pack = NULL;

    if (!out_profile || !entry)
        return 0;

    pack = banana_poc_continent_asset_pack_for_region(entry->primary_region_id);

    out_profile->browser_variant = entry->browser_variant;
    out_profile->kind = entry->kind;
    out_profile->primary_region_id = entry->primary_region_id;
    out_profile->secondary_region_id = entry->secondary_region_id;
    out_profile->route_id = entry->route_id;
    out_profile->anchor_county_id = entry->anchor_county_id;
    out_profile->scene_key = entry->scene_key;
    out_profile->asset_pack_id = entry->asset_pack_id;
    out_profile->gameplay_theme_id = (pack && pack->gameplay_theme_id) ? pack->gameplay_theme_id : "unknown-theme";
    out_profile->gameplay_placement_count =
        banana_poc_demo_scene_gameplay_placement_count_for_variant(browser_variant);
    out_profile->bootstrap_signature =
        banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(browser_variant);
    out_profile->validation_status =
        banana_poc_demo_scene_catalog_validate_index(
            banana_poc_demo_scene_catalog_index_for_browser_variant(browser_variant));

    return 1;
}

int banana_poc_demo_scene_catalog_coherent_profile_consistent(const BananaPocCoherentWorldProfile *profile)
{
    if (!profile)
        return 0;

    if (profile->validation_status != BANANA_POC_DEMO_SCENE_VALIDATION_OK)
        return 0;

    if (!profile->scene_key || profile->scene_key[0] == '\0')
        return 0;

    if (!banana_poc_demo_scene_profile_matches_catalog_identity(profile))
        return 0;

    if (!profile->asset_pack_id || !banana_poc_continent_asset_pack_is_registered(profile->asset_pack_id))
        return 0;

    if (!profile->anchor_county_id ||
        !banana_poc_region_has_county_anchor(profile->primary_region_id, profile->anchor_county_id))
    {
        return 0;
    }

    if (!profile->gameplay_theme_id || profile->gameplay_theme_id[0] == '\0')
        return 0;

    if (profile->gameplay_placement_count < 2)
        return 0;

    if (profile->bootstrap_signature == 0u ||
        profile->bootstrap_signature !=
            banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(profile->browser_variant))
    {
        return 0;
    }

    if (profile->kind == BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_STATION)
    {
        if (profile->route_id == RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID ||
            !banana_poc_region_in_route(profile->route_id, profile->primary_region_id))
        {
            return 0;
        }
    }

    if (profile->kind == BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_CORRIDOR)
    {
        if (profile->secondary_region_id == RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID ||
            profile->route_id == RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID ||
            !banana_poc_route_has_adjacent_segment(profile->route_id,
                                                   profile->primary_region_id,
                                                   profile->secondary_region_id))
        {
            return 0;
        }
    }

    return 1;
}

int banana_poc_demo_scene_catalog_coherent_transition_connected(int from_browser_variant,
                                                                int to_browser_variant)
{
    BananaPocCoherentWorldProfile from_profile;
    BananaPocCoherentWorldProfile to_profile;

    if (!banana_poc_demo_scene_catalog_build_coherent_profile(from_browser_variant, &from_profile) ||
        !banana_poc_demo_scene_catalog_build_coherent_profile(to_browser_variant, &to_profile))
    {
        return 0;
    }

    if (!banana_poc_demo_scene_catalog_coherent_profile_consistent(&from_profile) ||
        !banana_poc_demo_scene_catalog_coherent_profile_consistent(&to_profile))
    {
        return 0;
    }

    if (from_profile.browser_variant == to_profile.browser_variant)
        return 1;

    if (from_profile.route_id != to_profile.route_id ||
        from_profile.route_id == RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID)
    {
        return 0;
    }

    if (from_profile.primary_region_id == to_profile.primary_region_id)
        return 1;

    if (banana_poc_route_has_adjacent_segment(from_profile.route_id,
                                              from_profile.primary_region_id,
                                              to_profile.primary_region_id) ||
        banana_poc_route_has_adjacent_segment(from_profile.route_id,
                                              to_profile.primary_region_id,
                                              from_profile.primary_region_id))
    {
        return 1;
    }

    return 0;
}

unsigned int banana_poc_demo_scene_catalog_coherent_transition_signature(int from_browser_variant,
                                                                         int to_browser_variant)
{
    BananaPocCoherentWorldProfile from_profile;
    BananaPocCoherentWorldProfile to_profile;
    unsigned int hash = 2166136261u;

    if (!banana_poc_demo_scene_catalog_coherent_transition_connected(from_browser_variant, to_browser_variant))
        return 0u;

    if (!banana_poc_demo_scene_catalog_build_coherent_profile(from_browser_variant, &from_profile) ||
        !banana_poc_demo_scene_catalog_build_coherent_profile(to_browser_variant, &to_profile))
    {
        return 0u;
    }

    hash = banana_poc_hash_string(hash, from_profile.scene_key);
    hash = banana_poc_hash_string(hash, to_profile.scene_key);
    hash = banana_poc_hash_string(hash, from_profile.gameplay_theme_id);
    hash = banana_poc_hash_string(hash, to_profile.gameplay_theme_id);
    hash ^= (unsigned int)(from_profile.primary_region_id + 103);
    hash ^= (unsigned int)(to_profile.primary_region_id + 307);
    hash ^= (unsigned int)(from_profile.route_id + 509);
    hash ^= (unsigned int)(from_profile.bootstrap_signature + 701);
    hash ^= (unsigned int)(to_profile.bootstrap_signature + 907);

    return banana_poc_hash_u32(hash);
}

const char *banana_poc_demo_scene_gameplay_team_label(BananaPocGameplayTeam team)
{
    switch (team)
    {
    case BANANA_POC_GAMEPLAY_TEAM_BANANA:
        return "banana";
    case BANANA_POC_GAMEPLAY_TEAM_BEAN:
        return "bean";
    default:
        return "unknown-team";
    }
}

int banana_poc_demo_scene_gameplay_teams_are_enemies(BananaPocGameplayTeam first,
                                                     BananaPocGameplayTeam second)
{
    if (first == second)
        return 0;

    return (first == BANANA_POC_GAMEPLAY_TEAM_BANANA && second == BANANA_POC_GAMEPLAY_TEAM_BEAN) ||
           (first == BANANA_POC_GAMEPLAY_TEAM_BEAN && second == BANANA_POC_GAMEPLAY_TEAM_BANANA);
}

const char *banana_poc_demo_scene_gameplay_model_id_for_key(const char *model_key)
{
    return banana_poc_demo_scene_gameplay_model_id_for_key_with_overrides(model_key);
}

int banana_poc_demo_scene_gameplay_model_override(const char *model_key, const char *model_id)
{
    int index = 0;

    if (!model_key || model_key[0] == '\0' || !model_id || model_id[0] == '\0')
        return 0;

    for (index = 0; index < s_gameplay_model_override_count; index++)
    {
        if (s_gameplay_model_overrides[index].model_key &&
            strcmp(s_gameplay_model_overrides[index].model_key, model_key) == 0)
        {
            s_gameplay_model_overrides[index].model_id = model_id;
            s_demo_scene_resolved_gameplay_placements_dirty = 1;
            return 1;
        }
    }

    if (s_gameplay_model_override_count >= BANANA_POC_GAMEPLAY_MODEL_OVERRIDE_CAPACITY)
        return 0;

    s_gameplay_model_overrides[s_gameplay_model_override_count].model_key = model_key;
    s_gameplay_model_overrides[s_gameplay_model_override_count].model_id = model_id;
    s_gameplay_model_override_count++;
    s_demo_scene_resolved_gameplay_placements_dirty = 1;
    return 1;
}

void banana_poc_demo_scene_gameplay_model_clear_overrides(void)
{
    s_gameplay_model_override_count = 0;
    s_demo_scene_resolved_gameplay_placements_dirty = 1;
}

int banana_poc_demo_scene_vector_profile_for_model_id(const char *model_id,
                                                      float *out_radius_scale,
                                                      float *out_length_scale,
                                                      float *out_curve_scale,
                                                      float *out_tip_taper,
                                                      int *out_quality)
{
    return runtime_gameplay_model_vector_profile_for_model_id(model_id,
                                                               out_radius_scale,
                                                               out_length_scale,
                                                               out_curve_scale,
                                                               out_tip_taper,
                                                               out_quality);
}

unsigned int banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(int browser_variant)
{
    const BananaPocDemoSceneCatalogEntry *entry =
        banana_poc_demo_scene_catalog_for_browser_variant(browser_variant);
    const BananaPocContinentAssetPack *pack = NULL;
    unsigned int hash = 2166136261u;

    if (!entry)
        return 0u;

    pack = banana_poc_continent_asset_pack_for_region(entry->primary_region_id);

    hash = banana_poc_hash_string(hash, entry->scene_key);
    hash = banana_poc_hash_string(hash, entry->asset_pack_id);
    hash = banana_poc_hash_string(hash, entry->diagnostics_tag);
    hash = banana_poc_hash_string(hash, pack ? pack->gameplay_theme_id : NULL);
    hash = banana_poc_hash_string(hash, pack ? pack->reference_model_id : NULL);
    hash = banana_poc_hash_string(hash, pack ? pack->landmark_model_id : NULL);
    hash = banana_poc_hash_string(hash, pack ? pack->traversal_model_id : NULL);
    hash ^= (unsigned int)(banana_poc_demo_scene_gameplay_placement_count_for_variant(browser_variant) + 1543);
    hash ^= (unsigned int)(entry->browser_variant + 31);
    hash ^= (unsigned int)(entry->enabled + 131);
    hash ^= (unsigned int)(entry->kind + 257);
    hash ^= (unsigned int)(entry->primary_region_id + 521);
    hash ^= (unsigned int)(entry->route_id + 1021);

    return banana_poc_hash_u32(hash);
}

int banana_poc_demo_scene_gameplay_placement_count_for_variant(int browser_variant)
{
    int placement_count = 0;
    int index = 0;

    banana_poc_demo_scene_rebuild_resolved_placements();

    for (index = 0; index < (int)(sizeof(s_demo_scene_resolved_gameplay_placements) / sizeof(s_demo_scene_resolved_gameplay_placements[0])); index++)
    {
        if (s_demo_scene_resolved_gameplay_placements[index].browser_variant == browser_variant)
            placement_count++;
    }

    return placement_count;
}

const BananaPocDemoSceneGameplayPlacement *banana_poc_demo_scene_gameplay_placement_at(int browser_variant,
                                                                                        int placement_index)
{
    int matched_index = 0;
    int index = 0;

    if (placement_index < 0)
        return NULL;

    banana_poc_demo_scene_rebuild_resolved_placements();

    for (index = 0; index < (int)(sizeof(s_demo_scene_resolved_gameplay_placements) / sizeof(s_demo_scene_resolved_gameplay_placements[0])); index++)
    {
        const BananaPocDemoSceneGameplayPlacement *placement = &s_demo_scene_resolved_gameplay_placements[index];

        if (placement->browser_variant != browser_variant)
            continue;

        if (matched_index == placement_index)
            return placement;

        matched_index++;
    }

    return NULL;
}
