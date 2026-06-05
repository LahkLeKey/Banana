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
    BANANA_POC_DEMO_SCENE_VALIDATION_GAMEPLAY_MODEL_MISMATCH = 7,
} BananaPocDemoSceneValidationStatus;

typedef enum BananaPocGameplayPlacementRole
{
    BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_REFERENCE = 0,
    BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK = 1,
    BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_TRAVERSAL = 2,
} BananaPocGameplayPlacementRole;

typedef enum BananaPocGameplayTeam
{
    BANANA_POC_GAMEPLAY_TEAM_BANANA = 0,
    BANANA_POC_GAMEPLAY_TEAM_BEAN = 1,
} BananaPocGameplayTeam;

typedef struct BananaPocDemoSceneGameplayPlacement
{
    int browser_variant;
    BananaPocGameplayPlacementRole role;
    BananaPocGameplayTeam team;
    const char *model_id;
    const char *fallback_model_id;
    const char *diagnostics_tag;
    float x;
    float y;
    float z;
} BananaPocDemoSceneGameplayPlacement;

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

typedef struct BananaPocDemoSceneCaptureContext
{
    int browser_variant;
    const char *scene_key;
    const char *display_name;
} BananaPocDemoSceneCaptureContext;

typedef struct BananaPocCoherentWorldProfile
{
    int browser_variant;
    BananaPocDemoSceneKind kind;
    RuntimeTerrainStaticMeshRegionId primary_region_id;
    RuntimeTerrainStaticMeshRegionId secondary_region_id;
    RuntimeTerrainBananaLineRouteId route_id;
    const char *anchor_county_id;
    const char *scene_key;
    const char *asset_pack_id;
    const char *gameplay_theme_id;
    int gameplay_placement_count;
    unsigned int bootstrap_signature;
    BananaPocDemoSceneValidationStatus validation_status;
} BananaPocCoherentWorldProfile;

int banana_poc_demo_scene_catalog_count(void);
const BananaPocDemoSceneCatalogEntry *banana_poc_demo_scene_catalog_at_index(int index);
int banana_poc_demo_scene_catalog_index_for_browser_variant(int browser_variant);
const BananaPocDemoSceneCatalogEntry *banana_poc_demo_scene_catalog_for_browser_variant(int browser_variant);
int banana_poc_demo_scene_catalog_capture_context(int browser_variant,
                                                  BananaPocDemoSceneCaptureContext *out_context);
int banana_poc_demo_scene_catalog_clamp_index(int requested_index);
int banana_poc_demo_scene_catalog_first_enabled_index(void);
int banana_poc_demo_scene_catalog_is_launchable_index(int index);
BananaPocDemoSceneValidationStatus banana_poc_demo_scene_catalog_validate_index(int index);
BananaPocDemoSceneValidationStatus banana_poc_demo_scene_catalog_validate_gameplay_for_variant(int browser_variant);
const char *banana_poc_demo_scene_catalog_validation_message(BananaPocDemoSceneValidationStatus status);
const char *banana_poc_demo_scene_catalog_display_name_for_variant(int browser_variant);
const char *banana_poc_demo_scene_catalog_gameplay_theme_for_variant(int browser_variant);
int banana_poc_demo_scene_catalog_war_policy_for_variant(int browser_variant,
                                                         float *out_radius,
                                                         int *out_reinforcements_per_tick,
                                                         int *out_population_cap);
int banana_poc_demo_scene_catalog_build_coherent_profile(int browser_variant,
                                                         BananaPocCoherentWorldProfile *out_profile);
int banana_poc_demo_scene_catalog_coherent_profile_consistent(const BananaPocCoherentWorldProfile *profile);
int banana_poc_demo_scene_catalog_coherent_transition_connected(int from_browser_variant,
                                                                int to_browser_variant);
unsigned int banana_poc_demo_scene_catalog_coherent_transition_signature(int from_browser_variant,
                                                                         int to_browser_variant);
const char *banana_poc_demo_scene_gameplay_team_label(BananaPocGameplayTeam team);
int banana_poc_demo_scene_gameplay_teams_are_enemies(BananaPocGameplayTeam first,
                                                     BananaPocGameplayTeam second);
const char *banana_poc_demo_scene_gameplay_model_id_for_key(const char *model_key);
int banana_poc_demo_scene_gameplay_model_override(const char *model_key, const char *model_id);
void banana_poc_demo_scene_gameplay_model_clear_overrides(void);
int banana_poc_demo_scene_vector_profile_for_model_id(const char *model_id,
                                                      float *out_radius_scale,
                                                      float *out_length_scale,
                                                      float *out_curve_scale,
                                                      float *out_tip_taper,
                                                      int *out_quality);
unsigned int banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(int browser_variant);
int banana_poc_demo_scene_gameplay_placement_count_for_variant(int browser_variant);
const BananaPocDemoSceneGameplayPlacement *banana_poc_demo_scene_gameplay_placement_at(int browser_variant,
                                                                                        int placement_index);

#ifdef __cplusplus
}
#endif

#endif
