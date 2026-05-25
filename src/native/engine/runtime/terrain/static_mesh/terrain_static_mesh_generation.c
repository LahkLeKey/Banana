#include "terrain_static_mesh_domain.h"

#include <string.h>

int runtime_terrain_static_mesh_generate_heightfield_by_region(unsigned char *height_grid,
                                                               int terrain_size,
                                                               int terrain_max_layers,
                                                               RuntimeTerrainStaticMeshRegionId region_id,
                                                               int local_chunk_x,
                                                               int local_chunk_z,
                                                               unsigned int *out_generation_input_fingerprint)
{
    const RuntimeTerrainStaticMeshProfile *profile =
        runtime_terrain_static_mesh_profile_for_region(region_id);

    if (!profile)
        return RUNTIME_TERRAIN_GENERATION_STATUS_INVALID_AREA_IDENTITY;

    return runtime_terrain_generate_unexplored_contract(
        height_grid,
        terrain_size,
        terrain_max_layers,
        RUNTIME_TERRAIN_STATIC_MESH_WORLD_ID,
        profile->banana_line_lane_id,
        profile->partition_epoch,
        profile->hub_chunk_x + local_chunk_x,
        profile->hub_chunk_z + local_chunk_z,
        RUNTIME_TERRAIN_GENERATION_CONTRACT_V1,
        0u,
        0,
        out_generation_input_fingerprint);
}

int runtime_terrain_static_mesh_generate_county_heightfield(unsigned char *height_grid,
                                                            int terrain_size,
                                                            int terrain_max_layers,
                                                            int county_index,
                                                            unsigned int *out_generation_input_fingerprint)
{
    const RuntimeTerrainStaticMeshCounty *county = runtime_terrain_static_mesh_county_at_index(county_index);

    if (!county)
        return RUNTIME_TERRAIN_GENERATION_STATUS_INVALID_AREA_IDENTITY;

    return runtime_terrain_static_mesh_generate_heightfield_by_region(height_grid,
                                                                      terrain_size,
                                                                      terrain_max_layers,
                                                                      county->region_id,
                                                                      county->local_chunk_x,
                                                                      county->local_chunk_z,
                                                                      out_generation_input_fingerprint);
}
