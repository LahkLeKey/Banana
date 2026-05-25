#include "terrain_static_mesh_domain.h"

#include <string.h>

static int runtime_terrain_static_mesh_append_segment(const RuntimeTerrainStaticMeshProfile *from,
                                                      const RuntimeTerrainStaticMeshProfile *to,
                                                      int (*out_steps)[2],
                                                      int max_steps,
                                                      int *in_out_step_count)
{
    int cursor_x = 0;
    int cursor_z = 0;
    int used = 0;

    if (!from || !to || !out_steps || !in_out_step_count || max_steps <= 0)
        return 0;

    used = *in_out_step_count;
    cursor_x = (used > 0) ? out_steps[used - 1][0] : from->hub_chunk_x;
    cursor_z = (used > 0) ? out_steps[used - 1][1] : from->hub_chunk_z;

    if (used == 0)
    {
        out_steps[used][0] = cursor_x;
        out_steps[used][1] = cursor_z;
        used++;
    }

    while (cursor_x != to->hub_chunk_x && used < max_steps)
    {
        cursor_x += (cursor_x < to->hub_chunk_x) ? 1 : -1;
        out_steps[used][0] = cursor_x;
        out_steps[used][1] = cursor_z;
        used++;
    }

    while (cursor_z != to->hub_chunk_z && used < max_steps)
    {
        cursor_z += (cursor_z < to->hub_chunk_z) ? 1 : -1;
        out_steps[used][0] = cursor_x;
        out_steps[used][1] = cursor_z;
        used++;
    }

    *in_out_step_count = used;

    return (cursor_x == to->hub_chunk_x && cursor_z == to->hub_chunk_z) ? 1 : 0;
}

int runtime_terrain_banana_line_route_between_regions(RuntimeTerrainStaticMeshRegionId from_region_id,
                                                      RuntimeTerrainStaticMeshRegionId to_region_id,
                                                      int (*out_steps)[2],
                                                      int max_steps,
                                                      int *out_step_count)
{
    const RuntimeTerrainStaticMeshProfile *from =
        runtime_terrain_static_mesh_profile_for_region(from_region_id);
    const RuntimeTerrainStaticMeshProfile *to =
        runtime_terrain_static_mesh_profile_for_region(to_region_id);
    int used = 0;

    if (!from || !to || !out_steps || max_steps <= 0 || !out_step_count)
        return 0;

    if (!runtime_terrain_static_mesh_append_segment(from, to, out_steps, max_steps, &used))
        return 0;

    *out_step_count = used;
    return 1;
}

int runtime_terrain_banana_line_route_for_id(RuntimeTerrainBananaLineRouteId route_id,
                                             int (*out_steps)[2],
                                             int max_steps,
                                             int *out_step_count)
{
    const RuntimeTerrainBananaLineCorridor *corridor =
        runtime_terrain_banana_line_corridor_for_route(route_id);
    int stop_index = 0;
    int used = 0;

    if (!corridor || !out_steps || max_steps <= 0 || !out_step_count)
        return 0;

    for (stop_index = 1; stop_index < corridor->stop_count; stop_index++)
    {
        const RuntimeTerrainStaticMeshProfile *from =
            runtime_terrain_static_mesh_profile_for_region(corridor->stop_region_ids[stop_index - 1]);
        const RuntimeTerrainStaticMeshProfile *to =
            runtime_terrain_static_mesh_profile_for_region(corridor->stop_region_ids[stop_index]);

        if (!from || !to)
            return 0;

        if (!runtime_terrain_static_mesh_append_segment(from, to, out_steps, max_steps, &used))
            return 0;
    }

    *out_step_count = used;
    return 1;
}
