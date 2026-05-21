#include "runtime/engine_tick.h"

#include <stdio.h>

static int s_update_calls = 0;
static int s_follow_calls = 0;
static int s_gameplay_calls = 0;
static int s_render_calls = 0;

static int rebuild_ok(int max_chunks)
{
    (void)max_chunks;
    return 1;
}

static int rebuild_fail(int max_chunks)
{
    (void)max_chunks;
    return 0;
}

static void update_cb(float dt)
{
    (void)dt;
    s_update_calls += 1;
}

static void follow_cb(void)
{
    s_follow_calls += 1;
}

static void gameplay_cb(void)
{
    s_gameplay_calls += 1;
}

static void render_cb(void)
{
    s_render_calls += 1;
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    Window *window = window_create(320, 200, "tick-test");
    Renderer *renderer = renderer_create(320, 200);
    PhysicsWorld *physics = physics_world_create();
    World *world = world_create();
    int viewport_w = 320;
    int viewport_h = 200;

    if (!expect_int("window created", window != NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("renderer created", renderer != NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("physics created", physics != NULL ? 1 : 0, 1))
        return 1;
    if (!expect_int("world created", world != NULL ? 1 : 0, 1))
        return 1;

    if (!expect_int("null window fails",
                    runtime_engine_tick_execute(NULL,
                                                renderer,
                                                physics,
                                                world,
                                                &viewport_w,
                                                &viewport_h,
                                                rebuild_ok,
                                                NULL,
                                                0,
                                                1.0f / 60.0f,
                                                update_cb,
                                                follow_cb,
                                                NULL,
                                                render_cb),
                    -1))
        return 1;

    if (!expect_int("terrain rebuild fail",
                    runtime_engine_tick_execute(window,
                                                renderer,
                                                physics,
                                                world,
                                                &viewport_w,
                                                &viewport_h,
                                                rebuild_fail,
                                                NULL,
                                                0,
                                                1.0f / 60.0f,
                                                update_cb,
                                                follow_cb,
                                                NULL,
                                                render_cb),
                    -1))
        return 1;

    if (!expect_int("tick success",
                    runtime_engine_tick_execute(window,
                                                renderer,
                                                physics,
                                                world,
                                                &viewport_w,
                                                &viewport_h,
                                                rebuild_ok,
                                                NULL,
                                                0,
                                                1.0f / 60.0f,
                                                update_cb,
                                                follow_cb,
                                                NULL,
                                                render_cb),
                    0))
        return 1;

    if (!expect_int("update called", s_update_calls > 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("follow called", s_follow_calls > 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("render called", s_render_calls > 0 ? 1 : 0, 1))
        return 1;

    world_destroy(world);
    physics_world_destroy(physics);
    renderer_destroy(renderer);
    window_destroy(window);
    return 0;
}
