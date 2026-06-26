#include "runtime/orchestration/tick/loop/runtime_tick_orchestration.h"
#include "runtime/support/test_support.h"

#include <stdint.h>

static int g_rebuild_calls = 0;
static EngineRuntimeState *g_last_state = NULL;
static int g_last_max_chunks = 0;

static int stub_rebuild_dirty_chunks(EngineRuntimeState *state, int max_chunks)
{
    g_rebuild_calls += 1;
    g_last_state = state;
    g_last_max_chunks = max_chunks;
    return 77;
}

static void test_rebuild_dirty_chunks_rejects_invalid_contexts(void **state)
{
    (void)state;
    RuntimeApplicationServicePorts ports = {0};
    RuntimeTickOrchestrationContext context = {0};

    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_orchestration_rebuild_dirty_chunks(NULL, 3),
                              0,
                              "null context must return zero rebuilt chunks");

    context.state = (EngineRuntimeState *)(uintptr_t)0x1000;
    context.service_ports = NULL;
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_orchestration_rebuild_dirty_chunks(&context, 3),
                              0,
                              "missing service ports must return zero rebuilt chunks");

    context.service_ports = &ports;
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_orchestration_rebuild_dirty_chunks(&context, 3),
                              0,
                              "missing terrain callback must return zero rebuilt chunks");

    context.state = NULL;
    ports.terrain.rebuild_dirty_chunks = stub_rebuild_dirty_chunks;
    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_orchestration_rebuild_dirty_chunks(&context, 3),
                              0,
                              "missing runtime state must return zero rebuilt chunks");
}

static void test_rebuild_dirty_chunks_invokes_terrain_port(void **state)
{
    (void)state;
    RuntimeApplicationServicePorts ports = {0};
    RuntimeTickOrchestrationContext context = {0};
    EngineRuntimeState *expected_state = (EngineRuntimeState *)(uintptr_t)0xABC0;

    g_rebuild_calls = 0;
    g_last_state = NULL;
    g_last_max_chunks = 0;

    ports.terrain.rebuild_dirty_chunks = stub_rebuild_dirty_chunks;
    context.state = expected_state;
    context.service_ports = &ports;

    BANANA_TEST_ASSERT_INT_EQ(runtime_tick_orchestration_rebuild_dirty_chunks(&context, 11),
                              77,
                              "valid terrain callback result must be returned");
    BANANA_TEST_ASSERT_INT_EQ(g_rebuild_calls,
                              1,
                              "terrain callback must be invoked exactly once");
    BANANA_TEST_ASSERT_TRUE(g_last_state == expected_state,
                            "terrain callback must receive the same runtime state pointer");
    BANANA_TEST_ASSERT_INT_EQ(g_last_max_chunks,
                              11,
                              "terrain callback must receive the requested max chunks");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_rebuild_dirty_chunks_rejects_invalid_contexts),
    BANANA_TEST_CASE(test_rebuild_dirty_chunks_invokes_terrain_port)
)
