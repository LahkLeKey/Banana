#include "runtime/engine/application/service/application_service_ports.h"
#include "runtime/engine/composition/engine_composition.h"
#include "runtime/engine/state/engine_state.h"
#include "runtime/terrain/terrain_abi.h"

#include "runtime/support/test_support.h"

int engine_init(int width, int height);

static int g_set_height_calls = 0;
static int g_mark_dirty_calls = 0;
static int g_last_set_height = 0;
static int g_last_mark_min_x = 0;
static int g_last_mark_max_z = 0;

static float stub_terrain_sample_height(const EngineRuntimeState *state, float x, float z)
{
    (void)state;
    return x + z;
}

static int stub_terrain_set_height(EngineRuntimeState *state, int x, int z, int elevation)
{
    (void)state;
    (void)x;
    (void)z;
    g_set_height_calls += 1;
    g_last_set_height = elevation;
    return elevation + 1;
}

static void stub_terrain_mark_region_dirty(EngineRuntimeState *state,
                                           int min_x,
                                           int min_z,
                                           int max_x,
                                           int max_z)
{
    (void)state;
    (void)max_x;
    g_mark_dirty_calls += 1;
    g_last_mark_min_x = min_x;
    g_last_mark_max_z = max_z;
}

const RuntimeApplicationServicePorts *runtime_application_service_ports(void)
{
    return NULL;
}

int runtime_engine_composition_init(EngineRuntimeState *state,
                                    int width,
                                    int height,
                                    RuntimeTerrainSampleHeightFn terrain_sample_height,
                                    RuntimeEngineAttachControllerFn attach_controller)
{
    (void)state;
    (void)width;
    (void)height;
    (void)terrain_sample_height;
    (void)attach_controller;
    return 0;
}

int runtime_engine_composition_tick(EngineRuntimeState *state,
                                    float dt,
                                    RuntimeTerrainSampleHeightFn terrain_sample_height)
{
    (void)state;
    (void)dt;
    (void)terrain_sample_height;
    return 0;
}

void runtime_engine_composition_shutdown(EngineRuntimeState *state)
{
    (void)state;
}

uint32_t engine_controller_attach(uint32_t entity_id, const char *type_name)
{
    (void)entity_id;
    (void)type_name;
    return 0u;
}

static void test_engine_init_fails_when_service_ports_are_null(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(engine_init(320, 180),
                              -1,
                              "engine_init must fail when runtime_application_service_ports returns null");
}

static void test_terrain_abi_helpers_cover_all_paths(void **state)
{
    (void)state;
    RuntimeApplicationServicePorts ports = {0};
    EngineRuntimeState runtime_state = {0};
    unsigned int zero_hash = 0u;

    g_set_height_calls = 0;
    g_mark_dirty_calls = 0;
    g_last_set_height = 0;
    g_last_mark_min_x = 0;
    g_last_mark_max_z = 0;

    BANANA_TEST_ASSERT_FLOAT_EQ(runtime_terrain_abi_sample_height(NULL, &runtime_state, 1.0f, 2.0f),
                                0.0f,
                                0.0001f,
                                "sample_height must return zero when ports are missing");

    ports.terrain.sample_height = stub_terrain_sample_height;
    BANANA_TEST_ASSERT_FLOAT_EQ(runtime_terrain_abi_sample_height(&ports, &runtime_state, 3.0f, 4.0f),
                                7.0f,
                                0.0001f,
                                "sample_height must forward to the terrain service callback");

    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_abi_set_height(NULL, &runtime_state, 1, 2, 7),
                              -1,
                              "set_height must reject missing ports");
    ports.terrain.set_height = stub_terrain_set_height;
    BANANA_TEST_ASSERT_INT_EQ(runtime_terrain_abi_set_height(&ports, &runtime_state, 1, 2, 7),
                              8,
                              "set_height must forward successful calls to the terrain port");
    BANANA_TEST_ASSERT_INT_EQ(g_set_height_calls, 1,
                              "set_height stub must be invoked once");
    BANANA_TEST_ASSERT_INT_EQ(g_last_set_height, 7,
                              "set_height stub must receive the elevation");

    runtime_terrain_abi_mark_region_dirty(NULL, &runtime_state, 1, 2, 3, 4);
    ports.terrain.mark_region_dirty = stub_terrain_mark_region_dirty;
    runtime_terrain_abi_mark_region_dirty(&ports, &runtime_state, 11, 12, 13, 14);
    BANANA_TEST_ASSERT_INT_EQ(g_mark_dirty_calls, 1,
                              "mark_region_dirty stub must be invoked once");
    BANANA_TEST_ASSERT_INT_EQ(g_last_mark_min_x, 11,
                              "mark_region_dirty stub must receive the minimum x coordinate");
    BANANA_TEST_ASSERT_INT_EQ(g_last_mark_max_z, 14,
                              "mark_region_dirty stub must receive the maximum z coordinate");

    BANANA_TEST_ASSERT_TRUE(runtime_terrain_abi_retry_fingerprint_lineage(0u, 0u, 292984781) == 1u,
                            "retry_fingerprint_lineage must fix up the zero-hash case to one");
    zero_hash = runtime_terrain_abi_retry_fingerprint_lineage(0u, 0u, 292984781);
    BANANA_TEST_ASSERT_TRUE(zero_hash != 0u,
                            "retry_fingerprint_lineage must never return zero");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_engine_init_fails_when_service_ports_are_null),
    BANANA_TEST_CASE(test_terrain_abi_helpers_cover_all_paths)
)
