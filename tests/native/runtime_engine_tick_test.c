#include "runtime/engine_tick.h"

#include <stdio.h>

static int s_update_calls = 0;
static int s_follow_calls = 0;
static int s_gameplay_calls = 0;
static int s_render_calls = 0;
static int s_update_order = 0;
static int s_gameplay_order = 0;
static int s_follow_order = 0;
static int s_render_order = 0;
static int s_order_step = 0;

static int rebuild_ok(void *context, int max_chunks)
{
    (void)context;
    (void)max_chunks;
    return 1;
}

static int rebuild_fail(void *context, int max_chunks)
{
    (void)context;
    (void)max_chunks;
    return 0;
}

static void update_cb(void *context, float dt)
{
    (void)context;
    (void)dt;
    s_update_calls += 1;
    s_order_step += 1;
    s_update_order = s_order_step;
}

static void follow_cb(void *context)
{
    (void)context;
    s_follow_calls += 1;
    s_order_step += 1;
    s_follow_order = s_order_step;
}

static void gameplay_cb(void *context)
{
    (void)context;
    s_gameplay_calls += 1;
    s_order_step += 1;
    s_gameplay_order = s_order_step;
}

static void render_cb(void *context)
{
    (void)context;
    s_render_calls += 1;
    s_order_step += 1;
    s_render_order = s_order_step;
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
                                                NULL,
                                                update_cb,
                                                follow_cb,
                                                NULL,
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
                                                NULL,
                                                update_cb,
                                                follow_cb,
                                                NULL,
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
                                                NULL,
                                                update_cb,
                                                follow_cb,
                                                NULL,
                                                gameplay_cb,
                                                render_cb),
                    0))
        return 1;

    if (!expect_int("update called", s_update_calls > 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("gameplay called", s_gameplay_calls > 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("follow called", s_follow_calls > 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("render called", s_render_calls > 0 ? 1 : 0, 1))
        return 1;
    if (!expect_int("update before gameplay", s_update_order < s_gameplay_order ? 1 : 0, 1))
        return 1;
    if (!expect_int("gameplay before follow", s_gameplay_order < s_follow_order ? 1 : 0, 1))
        return 1;
    if (!expect_int("follow before render", s_follow_order < s_render_order ? 1 : 0, 1))
        return 1;

    world_destroy(world);
    physics_world_destroy(physics);
    renderer_destroy(renderer);
    window_destroy(window);
    return 0;
}
