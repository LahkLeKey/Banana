#include "terrain_static_mesh.h"

int runtime_terrain_generate_static_mesh_heightfield(unsigned char *height_grid,
                                                     int terrain_size,
                                                     int terrain_max_layers,
                                                     int profile_index,
                                                     int local_chunk_x,
                                                     int local_chunk_z,
                                                     unsigned int *out_generation_input_fingerprint)
{
    return runtime_terrain_static_mesh_generate_heightfield_by_region(height_grid,
                                                                      terrain_size,
                                                                      terrain_max_layers,
                                                                      (RuntimeTerrainStaticMeshRegionId)profile_index,
                                                                      local_chunk_x,
                                                                      local_chunk_z,
                                                                      out_generation_input_fingerprint);
}

int runtime_terrain_generate_static_mesh_county_heightfield(unsigned char *height_grid,
                                                            int terrain_size,
                                                            int terrain_max_layers,
                                                            int county_index,
                                                            unsigned int *out_generation_input_fingerprint)
{
    return runtime_terrain_static_mesh_generate_county_heightfield(height_grid,
                                                                    terrain_size,
                                                                    terrain_max_layers,
                                                                    county_index,
                                                                    out_generation_input_fingerprint);
}

int runtime_terrain_banana_line_route(int from_profile_index,
                                      int to_profile_index,
                                      int (*out_steps)[2],
                                      int max_steps,
                                      int *out_step_count)
{
    return runtime_terrain_banana_line_route_between_regions((RuntimeTerrainStaticMeshRegionId)from_profile_index,
                                                             (RuntimeTerrainStaticMeshRegionId)to_profile_index,
                                                             out_steps,
                                                             max_steps,
                                                             out_step_count);
}

int runtime_terrain_banana_line_route_named(const char *route_id,
                                            int (*out_steps)[2],
                                            int max_steps,
                                            int *out_step_count)
{
    RuntimeTerrainBananaLineRouteId route = runtime_terrain_banana_line_route_id_from_slug(route_id);

    if (route == RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID)
        return 0;

    return runtime_terrain_banana_line_route_for_id(route, out_steps, max_steps, out_step_count);
}
