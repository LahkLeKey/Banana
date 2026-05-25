#include "runtime/terrain/terrain_static_mesh.h"
#include "runtime/terrain/terrain_generation.h"

#include <stdio.h>
#include <string.h>

#define TERRAIN_SIZE 32
#define TERRAIN_LAYERS 6

static const int k_sample_offsets[][2] = {
    {0, 0},
    {1, 0},
    {0, 1},
    {-1, 0},
    {0, -1},
    {2, -2},
};

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

static int verify_profile_parity(int profile_index, int local_chunk_x, int local_chunk_z)
{
    unsigned char first[TERRAIN_SIZE * TERRAIN_SIZE];
    unsigned char second[TERRAIN_SIZE * TERRAIN_SIZE];
    unsigned int first_fingerprint = 0u;
    unsigned int second_fingerprint = 0u;

    memset(first, 0, sizeof(first));
    memset(second, 0, sizeof(second));

    if (runtime_terrain_generate_static_mesh_heightfield(
            first,
            TERRAIN_SIZE,
            TERRAIN_LAYERS,
            profile_index,
            local_chunk_x,
            local_chunk_z,
            &first_fingerprint) != RUNTIME_TERRAIN_GENERATION_STATUS_OK)
        return 0;

    if (runtime_terrain_generate_static_mesh_heightfield(
            second,
            TERRAIN_SIZE,
            TERRAIN_LAYERS,
            profile_index,
            local_chunk_x,
            local_chunk_z,
            &second_fingerprint) != RUNTIME_TERRAIN_GENERATION_STATUS_OK)
        return 0;

    return memcmp(first, second, sizeof(first)) == 0 &&
        first_fingerprint == second_fingerprint;
}

static int verify_route_parity(RuntimeTerrainStaticMeshRegionId from_region_id, RuntimeTerrainStaticMeshRegionId to_region_id)
{
    int route_a[1024][2];
    int route_b[1024][2];
    int route_a_count = 0;
    int route_b_count = 0;

    memset(route_a, 0, sizeof(route_a));
    memset(route_b, 0, sizeof(route_b));

        if (!runtime_terrain_banana_line_route_between_regions(
            from_region_id,
            to_region_id,
            route_a,
            (int)(sizeof(route_a) / sizeof(route_a[0])),
            &route_a_count))
        return 0;

        if (!runtime_terrain_banana_line_route_between_regions(
            from_region_id,
            to_region_id,
            route_b,
            (int)(sizeof(route_b) / sizeof(route_b[0])),
            &route_b_count))
        return 0;

    return route_a_count > 0 && route_b_count > 0 &&
        route_a_count <= (int)(sizeof(route_a) / sizeof(route_a[0])) &&
        route_b_count <= (int)(sizeof(route_b) / sizeof(route_b[0])) &&
        route_a_count == route_b_count &&
        memcmp(route_a, route_b, sizeof(route_a)) == 0;
}

static int verify_named_route_parity(RuntimeTerrainBananaLineRouteId route_id)
{
    int route_a[1024][2];
    int route_b[1024][2];
    int route_a_count = 0;
    int route_b_count = 0;

    memset(route_a, 0, sizeof(route_a));
    memset(route_b, 0, sizeof(route_b));

        if (!runtime_terrain_banana_line_route_for_id(
            route_id,
            route_a,
            (int)(sizeof(route_a) / sizeof(route_a[0])),
            &route_a_count))
        return 0;

        if (!runtime_terrain_banana_line_route_for_id(
            route_id,
            route_b,
            (int)(sizeof(route_b) / sizeof(route_b[0])),
            &route_b_count))
        return 0;

    return route_a_count > 0 && route_b_count > 0 &&
        route_a_count <= (int)(sizeof(route_a) / sizeof(route_a[0])) &&
        route_b_count <= (int)(sizeof(route_b) / sizeof(route_b[0])) &&
        route_a_count == route_b_count &&
        memcmp(route_a, route_b, sizeof(route_a)) == 0;
}

int main(void)
{
    unsigned char county_heightfield[TERRAIN_SIZE * TERRAIN_SIZE];
    int steps[1024][2];
    int step_count = 0;
    int named_step_count = 0;
    unsigned int county_fingerprint = 0u;
    const RuntimeTerrainStaticMeshProfile *stem_profile = NULL;
    const RuntimeTerrainStaticMeshProfile *frost_peel_profile = NULL;
    const RuntimeTerrainStaticMeshProfile *north_crown_profile = NULL;
    const RuntimeTerrainStaticMeshCounty *port_koba = NULL;
    const RuntimeTerrainBananaLineCorridor *mainline = NULL;
    memset(county_heightfield, 0, sizeof(county_heightfield));

    if (!expect_true("profile count equals canonical region count",
                     runtime_terrain_static_mesh_profile_count() == RUNTIME_TERRAIN_STATIC_MESH_REGION_COUNT))
        return 1;

    if (!expect_true("county count matches canonical worldbuilding baseline",
                     runtime_terrain_static_mesh_county_count() == 31))
        return 1;

    if (!expect_true("banana line corridor count is scaffolded",
                     runtime_terrain_banana_line_corridor_count() == 3))
        return 1;

    stem_profile = runtime_terrain_static_mesh_profile_for_region(RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES);
    frost_peel_profile = runtime_terrain_static_mesh_profile_for_region(RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL);
    north_crown_profile = runtime_terrain_static_mesh_profile_for_region(RUNTIME_TERRAIN_STATIC_MESH_REGION_NORTH_CROWN);
    port_koba = runtime_terrain_static_mesh_county_at_index(0);
    mainline = runtime_terrain_banana_line_corridor_at_index(0);
    if (!expect_true("stem profile exists", stem_profile != NULL))
        return 1;
    if (!expect_true("frost peel profile exists", frost_peel_profile != NULL))
        return 1;
    if (!expect_true("north crown profile exists", north_crown_profile != NULL))
        return 1;
    if (!expect_true("port koba county exists", port_koba != NULL))
        return 1;
    if (!expect_true("mainline corridor exists", mainline != NULL))
        return 1;

    for (int profile_index = 0; profile_index < runtime_terrain_static_mesh_profile_count(); profile_index++)
    {
        for (int offset_index = 0;
             offset_index < (int)(sizeof(k_sample_offsets) / sizeof(k_sample_offsets[0]));
             offset_index++)
        {
            const int local_chunk_x = k_sample_offsets[offset_index][0];
            const int local_chunk_z = k_sample_offsets[offset_index][1];
            const RuntimeTerrainStaticMeshProfile *profile = runtime_terrain_static_mesh_profile_at_index(profile_index);
            const char *profile_label = profile ? profile->region_slug : "unknown";
            char label[128];

            snprintf(label,
                     sizeof(label),
                     "%s parity at offset (%d,%d)",
                     profile_label,
                     local_chunk_x,
                     local_chunk_z);

            if (!expect_true(label,
                             verify_profile_parity(
                                 profile_index,
                                 local_chunk_x,
                                 local_chunk_z)))
                return 1;
        }
    }

    {
        unsigned char stem_a[TERRAIN_SIZE * TERRAIN_SIZE];
        unsigned char stem_b[TERRAIN_SIZE * TERRAIN_SIZE];
        unsigned char frost_peel[TERRAIN_SIZE * TERRAIN_SIZE];
        unsigned int fingerprint_stem_a = 0u;
        unsigned int fingerprint_stem_b = 0u;
        unsigned int fingerprint_frost_peel = 0u;

        memset(stem_a, 0, sizeof(stem_a));
        memset(stem_b, 0, sizeof(stem_b));
        memset(frost_peel, 0, sizeof(frost_peel));

        if (!expect_true("stem generation A",
                             runtime_terrain_generate_static_mesh_heightfield(
                                 stem_a,
                                 TERRAIN_SIZE,
                                 TERRAIN_LAYERS,
                                 RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES,
                                 0,
                                 0,
                                 &fingerprint_stem_a) == RUNTIME_TERRAIN_GENERATION_STATUS_OK))
            return 1;

        if (!expect_true("stem generation B",
                             runtime_terrain_generate_static_mesh_heightfield(
                                 stem_b,
                                 TERRAIN_SIZE,
                                 TERRAIN_LAYERS,
                                 RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES,
                                 0,
                                 0,
                                 &fingerprint_stem_b) == RUNTIME_TERRAIN_GENERATION_STATUS_OK))
            return 1;

        if (!expect_true("stem deterministic parity", memcmp(stem_a, stem_b, sizeof(stem_a)) == 0))
            return 1;
        if (!expect_true("stem fingerprint parity", fingerprint_stem_a == fingerprint_stem_b))
            return 1;

        if (!expect_true("frost peel generation",
                             runtime_terrain_generate_static_mesh_heightfield(
                                 frost_peel,
                                 TERRAIN_SIZE,
                                 TERRAIN_LAYERS,
                                 RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL,
                                 0,
                                 0,
                                 &fingerprint_frost_peel) == RUNTIME_TERRAIN_GENERATION_STATUS_OK))
            return 1;

        if (!expect_true("stem and frost peel differ", fingerprint_stem_a != fingerprint_frost_peel))
            return 1;
    }

    if (!expect_true("county generation works",
                     runtime_terrain_generate_static_mesh_county_heightfield(
                         county_heightfield,
                         TERRAIN_SIZE,
                         TERRAIN_LAYERS,
                         0,
                         &county_fingerprint) == RUNTIME_TERRAIN_GENERATION_STATUS_OK))
        return 1;

    if (!expect_true("county anchor id matches canon", strcmp(port_koba->county_id, "port-koba") == 0))
        return 1;

    {
        unsigned char stem_hub[TERRAIN_SIZE * TERRAIN_SIZE];
        unsigned int stem_hub_fingerprint = 0u;

        memset(stem_hub, 0, sizeof(stem_hub));

        if (!expect_true("stem hub generation for county anchor comparison",
                             runtime_terrain_generate_static_mesh_heightfield(
                                 stem_hub,
                                 TERRAIN_SIZE,
                                 TERRAIN_LAYERS,
                                 RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES,
                                 0,
                                 0,
                                 &stem_hub_fingerprint) == RUNTIME_TERRAIN_GENERATION_STATUS_OK))
            return 1;

        if (!expect_true("county generation shares anchor fingerprint with regional hub",
                         county_fingerprint == stem_hub_fingerprint))
            return 1;
    }

    for (int corridor_index = 0;
         corridor_index < runtime_terrain_banana_line_corridor_count();
         corridor_index++)
    {
        const RuntimeTerrainBananaLineCorridor *corridor =
            runtime_terrain_banana_line_corridor_at_index(corridor_index);

        if (!expect_true("banana line corridor exists for route parity", corridor != NULL))
            return 1;

        if (!expect_true("banana line corridor named route is deterministic",
                         verify_named_route_parity(corridor->route_id)))
            return 1;

        for (int stop_index = 1; stop_index < corridor->stop_count; stop_index++)
        {
            if (!expect_true("banana line adjacent corridor leg is deterministic",
                             verify_route_parity(
                                 corridor->stop_region_ids[stop_index - 1],
                                 corridor->stop_region_ids[stop_index])))
                return 1;
        }
    }

    if (!expect_true("banana line route exists",
                     runtime_terrain_banana_line_route_between_regions(
                         RUNTIME_TERRAIN_STATIC_MESH_REGION_STEM_TERRITORIES,
                         RUNTIME_TERRAIN_STATIC_MESH_REGION_FROST_PEEL,
                         steps,
                         (int)(sizeof(steps) / sizeof(steps[0])),
                         &step_count)))
        return 1;

    if (!expect_true("banana line route has steps", step_count > 1))
        return 1;

    if (!expect_true("named banana line route exists",
                     runtime_terrain_banana_line_route_for_id(
                         RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE,
                         steps,
                         (int)(sizeof(steps) / sizeof(steps[0])),
                         &named_step_count)))
        return 1;

    if (!expect_true("named banana line route has steps", named_step_count > 1))
        return 1;

    if (!expect_true("banana line starts at stem hub",
                     steps[0][0] == stem_profile->hub_chunk_x &&
                         steps[0][1] == stem_profile->hub_chunk_z))
        return 1;

    if (!expect_true("banana line ends at frost peel hub",
                     steps[step_count - 1][0] == frost_peel_profile->hub_chunk_x &&
                         steps[step_count - 1][1] == frost_peel_profile->hub_chunk_z))
        return 1;

    if (!expect_true("named banana line starts at stem hub",
                     steps[0][0] == stem_profile->hub_chunk_x &&
                         steps[0][1] == stem_profile->hub_chunk_z))
        return 1;

    if (!expect_true("named banana line ends at north crown hub",
                     steps[named_step_count - 1][0] == north_crown_profile->hub_chunk_x &&
                         steps[named_step_count - 1][1] == north_crown_profile->hub_chunk_z))
        return 1;

    if (!expect_true("stem county anchor matches canon", strcmp(stem_profile->county_anchor, "port-koba") == 0))
        return 1;

    if (!expect_true("frost peel climate tag matches canon", strcmp(frost_peel_profile->climate_tag, "polar-maritime") == 0))
        return 1;

    if (!expect_true("mainline corridor id matches canon", mainline->route_id == RUNTIME_TERRAIN_BANANA_LINE_ROUTE_MAINLINE))
        return 1;

    if (!expect_true("mainline corridor stop count matches passenger spine scaffold", mainline->stop_count == 6))
        return 1;

    return 0;
}
