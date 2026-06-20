#include "runtime/engine/application/service/application_service_ports.h"
#include "runtime/engine/composition/engine_composition.h"
#include "runtime/engine/state/engine_state.h"

#include "../support/test_support.h"

int engine_init(int width, int height);

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

float runtime_terrain_abi_sample_height(const RuntimeApplicationServicePorts *ports,
                                        EngineRuntimeState *state,
                                        float x,
                                        float z)
{
    (void)ports;
    (void)state;
    (void)x;
    (void)z;
    return 0.0f;
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

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_engine_init_fails_when_service_ports_are_null)
)
