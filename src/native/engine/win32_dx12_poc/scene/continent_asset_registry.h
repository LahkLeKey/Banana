#ifndef BANANA_ENGINE_WIN32_DX12_POC_CONTINENT_ASSET_REGISTRY_H
#define BANANA_ENGINE_WIN32_DX12_POC_CONTINENT_ASSET_REGISTRY_H

#include "../../runtime/terrain/static_mesh/terrain_static_mesh_domain.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct BananaPocContinentAssetPack
{
    RuntimeTerrainStaticMeshRegionId region_id;
    const char *region_slug;
    const char *asset_pack_id;
    const char *gameplay_theme_id;
    const char *reference_model_id;
    const char *landmark_model_id;
    const char *traversal_model_id;
    const char *fallback_tag;
    int legacy_placeholder;
} BananaPocContinentAssetPack;

int banana_poc_continent_asset_pack_count(void);
const BananaPocContinentAssetPack *banana_poc_continent_asset_pack_at_index(int index);
const BananaPocContinentAssetPack *banana_poc_continent_asset_pack_for_region(RuntimeTerrainStaticMeshRegionId region_id);
int banana_poc_continent_asset_pack_is_registered(const char *asset_pack_id);
int banana_poc_continent_asset_pack_has_gameplay_models(const BananaPocContinentAssetPack *pack);

#ifdef __cplusplus
}
#endif

#endif
