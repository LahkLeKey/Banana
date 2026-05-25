#ifndef BANANA_ENGINE_RUNTIME_TERRAIN_STATIC_MESH_H
#define BANANA_ENGINE_RUNTIME_TERRAIN_STATIC_MESH_H

#include "static_mesh/terrain_static_mesh_domain.h"

#ifdef __cplusplus
extern "C"
{
#endif

int runtime_terrain_generate_static_mesh_heightfield(unsigned char *height_grid,
                                                     int terrain_size,
                                                     int terrain_max_layers,
                                                     int profile_index,
                                                     int local_chunk_x,
                                                     int local_chunk_z,
                                                     unsigned int *out_generation_input_fingerprint);

int runtime_terrain_generate_static_mesh_county_heightfield(unsigned char *height_grid,
                                                            int terrain_size,
                                                            int terrain_max_layers,
                                                            int county_index,
                                                            unsigned int *out_generation_input_fingerprint);

int runtime_terrain_banana_line_route(int from_profile_index,
                                      int to_profile_index,
                                      int (*out_steps)[2],
                                      int max_steps,
                                      int *out_step_count);

int runtime_terrain_banana_line_route_named(const char *route_id,
                                            int (*out_steps)[2],
                                            int max_steps,
                                            int *out_step_count);

#ifdef __cplusplus
}
#endif

#endif
