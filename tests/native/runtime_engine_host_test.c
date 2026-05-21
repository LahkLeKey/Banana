#include "runtime/engine_host.h"

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
    Window *window = (Window *)1;
    Renderer *renderer = renderer_create(64, 64);
    PhysicsWorld *physics = (PhysicsWorld *)1;
    World *world = (World *)1;
    Mesh *entity_mesh = (Mesh *)1;
    EntityId player_id = 99;
    int terrain_initialized = 1;
    int viewport_width = 640;
    int viewport_height = 360;
    int engine_initialized = 1;
    int camera_valid = 1;
    float move_x = 0.5f;
    float move_z = -0.5f;
    const unsigned char *frame = NULL;

    if (!expect_int("renderer created", renderer != NULL ? 1 : 0, 1))
        return 1;

    runtime_engine_host_render_frame(renderer);
    frame = runtime_engine_host_get_frame_buffer(renderer);
    if (!expect_int("frame buffer available", frame != NULL ? 1 : 0, 1))
        return 1;

    if (!expect_int("null frame buffer", runtime_engine_host_get_frame_buffer(NULL) == NULL ? 1 : 0, 1))
        return 1;

    runtime_engine_host_reset_state(&window,
                                    &renderer,
                                    &physics,
                                    &world,
                                    &entity_mesh,
                                    &player_id,
                                    &terrain_initialized,
                                    &viewport_width,
                                    &viewport_height,
                                    &engine_initialized,
                                    &camera_valid,
                                    &move_x,
                                    &move_z);

    if (!expect_int("window reset", window == NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("renderer reset", renderer == NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("physics reset", physics == NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("world reset", world == NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("mesh reset", entity_mesh == NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("player reset", (int)player_id, 0))
        return 1;
    if (!expect_int("terrain reset", terrain_initialized, 0))
        return 1;
    if (!expect_int("viewport width reset", viewport_width, 0))
        return 1;
    if (!expect_int("viewport height reset", viewport_height, 0))
        return 1;
    if (!expect_int("engine reset", engine_initialized, 0))
        return 1;
    if (!expect_int("camera reset", camera_valid, 0))
        return 1;
    if (!expect_int("move x reset", move_x == 0.f ? 1 : 0, 1))
        return 1;
    if (!expect_int("move z reset", move_z == 0.f ? 1 : 0, 1))
        return 1;

    return 0;
}
