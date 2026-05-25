#ifndef BANANA_ENGINE_WIN32_DX12_POC_DEMO_SCENE_CATALOG_H
#define BANANA_ENGINE_WIN32_DX12_POC_DEMO_SCENE_CATALOG_H

#include "continent_asset_registry.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum BananaPocDemoSceneKind
{
    BANANA_POC_DEMO_SCENE_KIND_CONTINENT = 0,
    BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_STATION = 1,
    BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_CORRIDOR = 2,
    BANANA_POC_DEMO_SCENE_KIND_LAB = 3,
} BananaPocDemoSceneKind;

typedef enum BananaPocDemoSceneValidationStatus
{
    BANANA_POC_DEMO_SCENE_VALIDATION_OK = 0,
    BANANA_POC_DEMO_SCENE_VALIDATION_UNKNOWN_SCENE = 1,
    BANANA_POC_DEMO_SCENE_VALIDATION_DISABLED = 2,
    BANANA_POC_DEMO_SCENE_VALIDATION_MISSING_ASSET_PACK = 3,
    BANANA_POC_DEMO_SCENE_VALIDATION_CANONICAL_REGION_MISMATCH = 4,
    BANANA_POC_DEMO_SCENE_VALIDATION_ROUTE_MISMATCH = 5,
    BANANA_POC_DEMO_SCENE_VALIDATION_ANCHOR_MISMATCH = 6,
} BananaPocDemoSceneValidationStatus;

typedef struct BananaPocDemoSceneCatalogEntry
{
    int browser_variant;
    int enabled;
    BananaPocDemoSceneKind kind;
    RuntimeTerrainStaticMeshRegionId primary_region_id;
    RuntimeTerrainStaticMeshRegionId secondary_region_id;
    RuntimeTerrainBananaLineRouteId route_id;
    const char *anchor_county_id;
    const char *scene_key;
    const char *display_name;
    const char *asset_pack_id;
    const char *diagnostics_tag;
} BananaPocDemoSceneCatalogEntry;

int banana_poc_demo_scene_catalog_count(void);
const BananaPocDemoSceneCatalogEntry *banana_poc_demo_scene_catalog_at_index(int index);
int banana_poc_demo_scene_catalog_index_for_browser_variant(int browser_variant);
const BananaPocDemoSceneCatalogEntry *banana_poc_demo_scene_catalog_for_browser_variant(int browser_variant);
int banana_poc_demo_scene_catalog_clamp_index(int requested_index);
int banana_poc_demo_scene_catalog_first_enabled_index(void);
int banana_poc_demo_scene_catalog_is_launchable_index(int index);
BananaPocDemoSceneValidationStatus banana_poc_demo_scene_catalog_validate_index(int index);
const char *banana_poc_demo_scene_catalog_validation_message(BananaPocDemoSceneValidationStatus status);
const char *banana_poc_demo_scene_catalog_display_name_for_variant(int browser_variant);
unsigned int banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(int browser_variant);

#ifdef __cplusplus
}
#endif

#endif
