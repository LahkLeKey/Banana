#include "runtime/engine_state.h"

#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    EngineRuntimeState state;

    state.window = (Window *)1;
    state.renderer = (Renderer *)1;
    state.physics = (PhysicsWorld *)1;
    state.world = (World *)1;
    state.entity_mesh = (Mesh *)1;
    state.pbj_pickup_mesh = (Mesh *)1;
    state.player_id = 77;
    state.engine_initialized = 1;
    state.terrain_initialized = 1;
    state.viewport_width = 1280;
    state.viewport_height = 720;
    state.camera_valid = 1;
    state.move_input_x = 0.5f;
    state.move_input_z = -0.5f;
    state.merchants_seeded = 1;
    state.pbj_pickup_collected = 1;
    state.controller_count = 12;

    runtime_engine_state_reset(&state);

    if (!expect_int("window reset", state.window == NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("renderer reset", state.renderer == NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("physics reset", state.physics == NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("world reset", state.world == NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("mesh reset", state.entity_mesh == NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("pickup mesh reset", state.pbj_pickup_mesh == NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("player reset", (int)state.player_id, 0))
        return 1;
    if (!expect_int("engine reset", state.engine_initialized, 0))
        return 1;
    if (!expect_int("terrain reset", state.terrain_initialized, 0))
        return 1;
    if (!expect_int("viewport width reset", state.viewport_width, 0))
        return 1;
    if (!expect_int("viewport height reset", state.viewport_height, 0))
        return 1;
    if (!expect_int("camera reset", state.camera_valid, 0))
        return 1;
    if (!expect_int("move x reset", state.move_input_x == 0.f ? 1 : 0, 1))
        return 1;
    if (!expect_int("move z reset", state.move_input_z == 0.f ? 1 : 0, 1))
        return 1;
    if (!expect_int("merchant flag reset", state.merchants_seeded, 0))
        return 1;
    if (!expect_int("pickup flag reset", state.pbj_pickup_collected, 0))
        return 1;
    if (!expect_int("controller count reset", state.controller_count, 0))
        return 1;

    return 0;
}
