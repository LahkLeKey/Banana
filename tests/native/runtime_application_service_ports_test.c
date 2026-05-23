#include "runtime/application_service_ports.h"
#include "runtime/engine_state.h"

#include <math.h>
#include <stdio.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "FAIL: %s\n", label);
    return 0;
}

int main(void)
{
    EngineRuntimeState state;
    const RuntimeApplicationServicePorts *ports = runtime_application_service_ports();

    runtime_engine_state_reset(&state);

    if (!expect_true("ports object", ports != NULL))
        return 1;
    if (!expect_true("terrain sample callback", ports->terrain.sample_height != NULL))
        return 1;
    if (!expect_true("terrain mutation callback", ports->terrain.set_height != NULL))
        return 1;
    if (!expect_true("player motion callback", ports->player.update_motion != NULL))
        return 1;
    if (!expect_true("merchant callback", ports->merchant.trade_buy != NULL))
        return 1;
    if (!expect_true("render callback", ports->render.submit_scene != NULL))
        return 1;

    if (!expect_true("terrain set height", ports->terrain.set_height(&state, 9, 7, 2) == 1))
        return 1;
    if (!expect_true("terrain grid value", state.terrain_height[7][9] == 2))
        return 1;

    ports->terrain.mark_region_dirty(&state, 0, 0, 15, 15);
    if (!expect_true("chunk dirty flag", state.terrain_chunks[0].dirty == 1))
        return 1;

    if (!expect_true("sample height finite",
                     isfinite(ports->terrain.sample_height(&state, 0.f, 0.f)) != 0))
        return 1;

    return 0;
}
