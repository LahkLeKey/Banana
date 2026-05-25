#include "runtime/terrain/terrain_abi.h"

#include <stdio.h>
#include <string.h>

typedef struct TerrainStubState
{
    int sample_calls;
    int set_calls;
    int dirty_calls;
    float last_sample_x;
    float last_sample_z;
    int last_set_x;
    int last_set_z;
    int last_set_elevation;
    int last_min_x;
    int last_min_z;
    int last_max_x;
    int last_max_z;
} TerrainStubState;

static TerrainStubState g_stub;

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s failed: got %d expected %d\n", label, actual, expected);
    return 0;
}

static int expect_float(const char *label, float actual, float expected)
{
    float delta = actual - expected;
    if (delta < 0.f)
        delta = -delta;

    if (delta <= 0.0001f)
        return 1;

    fprintf(stderr, "%s failed: got %.3f expected %.3f\n", label, actual, expected);
    return 0;
}

static float sample_height_stub(const EngineRuntimeState *state, float x, float z)
{
    (void)state;
    g_stub.sample_calls += 1;
    g_stub.last_sample_x = x;
    g_stub.last_sample_z = z;
    return 42.5f;
}

static int set_height_stub(EngineRuntimeState *state, int x, int z, int elevation)
{
    (void)state;
    g_stub.set_calls += 1;
    g_stub.last_set_x = x;
    g_stub.last_set_z = z;
    g_stub.last_set_elevation = elevation;
    return 7;
}

static void mark_region_dirty_stub(EngineRuntimeState *state,
                                   int min_x,
                                   int min_z,
                                   int max_x,
                                   int max_z)
{
    (void)state;
    g_stub.dirty_calls += 1;
    g_stub.last_min_x = min_x;
    g_stub.last_min_z = min_z;
    g_stub.last_max_x = max_x;
    g_stub.last_max_z = max_z;
}

int main(void)
{
    EngineRuntimeState state;
    RuntimeApplicationServicePorts ports;
    unsigned int retry_lineage_a = 0u;
    unsigned int retry_lineage_b = 0u;

    memset(&state, 0, sizeof(state));
    memset(&ports, 0, sizeof(ports));
    memset(&g_stub, 0, sizeof(g_stub));

    if (!expect_float("sample null ports", runtime_terrain_abi_sample_height(NULL, &state, 1.f, 2.f), 0.f))
        return 1;

    if (!expect_int("set null ports", runtime_terrain_abi_set_height(NULL, &state, 1, 2, 3), -1))
        return 1;

    runtime_terrain_abi_mark_region_dirty(NULL, &state, 1, 2, 3, 4);
    if (!expect_int("dirty null ports", g_stub.dirty_calls, 0))
        return 1;

    ports.terrain.sample_height = sample_height_stub;
    ports.terrain.set_height = set_height_stub;
    ports.terrain.mark_region_dirty = mark_region_dirty_stub;

    if (!expect_float("sample value", runtime_terrain_abi_sample_height(&ports, &state, 4.f, 9.f), 42.5f))
        return 1;
    if (!expect_int("sample calls", g_stub.sample_calls, 1))
        return 1;
    if (!expect_float("sample x", g_stub.last_sample_x, 4.f))
        return 1;
    if (!expect_float("sample z", g_stub.last_sample_z, 9.f))
        return 1;

    if (!expect_int("set value", runtime_terrain_abi_set_height(&ports, &state, 8, 6, 13), 7))
        return 1;
    if (!expect_int("set calls", g_stub.set_calls, 1))
        return 1;
    if (!expect_int("set x", g_stub.last_set_x, 8))
        return 1;
    if (!expect_int("set z", g_stub.last_set_z, 6))
        return 1;
    if (!expect_int("set elevation", g_stub.last_set_elevation, 13))
        return 1;

    runtime_terrain_abi_mark_region_dirty(&ports, &state, 1, 3, 5, 7);
    if (!expect_int("dirty calls", g_stub.dirty_calls, 1))
        return 1;
    if (!expect_int("dirty min x", g_stub.last_min_x, 1))
        return 1;
    if (!expect_int("dirty min z", g_stub.last_min_z, 3))
        return 1;
    if (!expect_int("dirty max x", g_stub.last_max_x, 5))
        return 1;
    if (!expect_int("dirty max z", g_stub.last_max_z, 7))
        return 1;

    retry_lineage_a = runtime_terrain_abi_retry_fingerprint_lineage(101u, 1u, -11);
    retry_lineage_b = runtime_terrain_abi_retry_fingerprint_lineage(101u, 1u, -11);
    if (!expect_int("retry lineage deterministic", retry_lineage_a == retry_lineage_b, 1))
        return 1;
    if (!expect_int("retry lineage non-zero", retry_lineage_a != 0u, 1))
        return 1;
    if (!expect_int("retry lineage differs by attempt",
                    runtime_terrain_abi_retry_fingerprint_lineage(101u, 2u, -11) != retry_lineage_a,
                    1))
        return 1;

    return 0;
}
