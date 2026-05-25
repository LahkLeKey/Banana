#ifndef BANANA_ENGINE_RUNTIME_TERRAIN_STATIC_MESH_DOMAIN_H
#define BANANA_ENGINE_RUNTIME_TERRAIN_STATIC_MESH_DOMAIN_H

#include "../terrain_generation.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define RUNTIME_TERRAIN_STATIC_MESH_WORLD_ID "overworld-348200km2"
#define RUNTIME_TERRAIN_STATIC_MESH_GENERATOR_VERSION "county-static-v1"
#define RUNTIME_TERRAIN_STATIC_MESH_MAX_ROUTE_STOPS 8

typedef enum RuntimeTerrainStaticMeshRegionId
{
    RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID = -1,
    RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES = 0,
    RUNTIME_TERRAIN_STATIC_MESH_REGION_SUN_COAST = 1,
    RUNTIME_TERRAIN_STATIC_MESH_REGION_METRO_BANANA = 2,
    RUNTIME_TERRAIN_STATIC_MESH_REGION_CENTRAL_CRESCENT = 3,
    RUNTIME_TERRAIN_STATIC_MESH_REGION_IRON_SPINE = 4,
    RUNTIME_TERRAIN_STATIC_MESH_REGION_GOLDEN_HOOK = 5,
    RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN = 6,
    RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL = 7,
    RUNTIME_TERRAIN_STATIC_MESH_REGION_COUNT = 8
} RuntimeTerrainStaticMeshRegionId;

typedef enum RuntimeTerrainBananaLineRouteId
{
    RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID = -1,
    RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE = 0,
    RUNTIME_TERRAIN_BANANA_LINE_ROUTE_SPINE_FREIGHT = 1,
    RUNTIME_TERRAIN_BANANA_LINE_ROUTE_FROST_FERRY = 2,
    RUNTIME_TERRAIN_BANANA_LINE_ROUTE_COUNT = 3
} RuntimeTerrainBananaLineRouteId;

typedef struct RuntimeTerrainStaticMeshProfile
{
    RuntimeTerrainStaticMeshRegionId region_id;
    const char *region_slug;
    const char *county_anchor;
    const char *banana_line_lane_id;
    const char *climate_tag;
    const char *relief_tag;
    const char *hazard_tag;
    const char *economy_tag;
    const char *density_tag;
    const char *infrastructure_tag;
    int hub_chunk_x;
    int hub_chunk_z;
    int coverage_radius_chunks;
    unsigned int partition_epoch;
} RuntimeTerrainStaticMeshProfile;

typedef struct RuntimeTerrainStaticMeshCounty
{
    RuntimeTerrainStaticMeshRegionId region_id;
    const char *county_id;
    int local_chunk_x;
    int local_chunk_z;
} RuntimeTerrainStaticMeshCounty;

typedef struct RuntimeTerrainBananaLineCorridor
{
    RuntimeTerrainBananaLineRouteId route_id;
    const char *route_slug;
    RuntimeTerrainStaticMeshRegionId stop_region_ids[RUNTIME_TERRAIN_STATIC_MESH_MAX_ROUTE_STOPS];
    int stop_count;
} RuntimeTerrainBananaLineCorridor;

int runtime_terrain_static_mesh_profile_count(void);
int runtime_terrain_static_mesh_profile_index_for_region_id(RuntimeTerrainStaticMeshRegionId region_id);
const RuntimeTerrainStaticMeshProfile *runtime_terrain_static_mesh_profile_at_index(int profile_index);
const RuntimeTerrainStaticMeshProfile *runtime_terrain_static_mesh_profile_for_region(RuntimeTerrainStaticMeshRegionId region_id);
RuntimeTerrainStaticMeshRegionId runtime_terrain_static_mesh_region_id_from_slug(const char *region_slug);
const char *runtime_terrain_static_mesh_region_slug(RuntimeTerrainStaticMeshRegionId region_id);

int runtime_terrain_static_mesh_county_count(void);
const RuntimeTerrainStaticMeshCounty *runtime_terrain_static_mesh_county_at_index(int county_index);

int runtime_terrain_banana_line_corridor_count(void);
const RuntimeTerrainBananaLineCorridor *runtime_terrain_banana_line_corridor_at_index(int corridor_index);
const RuntimeTerrainBananaLineCorridor *runtime_terrain_banana_line_corridor_for_route(RuntimeTerrainBananaLineRouteId route_id);
RuntimeTerrainBananaLineRouteId runtime_terrain_banana_line_route_id_from_slug(const char *route_slug);
const char *runtime_terrain_banana_line_route_slug(RuntimeTerrainBananaLineRouteId route_id);

int runtime_terrain_static_mesh_generate_heightfield_by_region(unsigned char *height_grid,
                                                               int terrain_size,
                                                               int terrain_max_layers,
                                                               RuntimeTerrainStaticMeshRegionId region_id,
                                                               int local_chunk_x,
                                                               int local_chunk_z,
                                                               unsigned int *out_generation_input_fingerprint);

int runtime_terrain_static_mesh_generate_county_heightfield(unsigned char *height_grid,
                                                            int terrain_size,
                                                            int terrain_max_layers,
                                                            int county_index,
                                                            unsigned int *out_generation_input_fingerprint);

int runtime_terrain_banana_line_route_between_regions(RuntimeTerrainStaticMeshRegionId from_region_id,
                                                      RuntimeTerrainStaticMeshRegionId to_region_id,
                                                      int (*out_steps)[2],
                                                      int max_steps,
                                                      int *out_step_count);

int runtime_terrain_banana_line_route_for_id(RuntimeTerrainBananaLineRouteId route_id,
                                             int (*out_steps)[2],
                                             int max_steps,
                                             int *out_step_count);

#ifdef __cplusplus
}
#endif

#endif
