/*
 * Phase 1 Engine Test Harness (T004)
 * Tests: physics simulation, AI state machine, navigation, world entities.
 * Rendering tests run in stub mode (no GPU required on CI).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "../../../src/native/engine/physics/body.h"
#include "../../../src/native/engine/physics/collider.h"
#include "../../../src/native/engine/physics/dynamics.h"
#include "../../../src/native/engine/physics/world.h"
#include "../../../src/native/engine/ai/state_machine.h"
#include "../../../src/native/engine/ai/navigation.h"
#include "../../../src/native/engine/render/camera.h"
#include "../../../src/native/engine/render/renderer.h"
#include "../../../src/native/engine/render/mesh.h"
#include "../../../src/native/engine/world/entity.h"
#include "../../../src/native/engine/world/world.h"
#include "../../../src/native/engine/world/signals.h"

/* ── Minimal test framework ───────────────────────────────────────────────── */
static int s_pass = 0, s_fail = 0;

#define TEST(name) \
    do { printf("  %-55s", name); } while(0)

#define PASS() \
    do { printf("PASS\n"); s_pass++; } while(0)

#define FAIL(msg) \
    do { printf("FAIL (%s)\n", msg); s_fail++; } while(0)

#define ASSERT_NEAR(a, b, tol) \
    do { if (fabsf((a)-(b)) > (tol)) { FAIL(#a " !~= " #b); goto done; } } while(0)

#define ASSERT_TRUE(expr) \
    do { if (!(expr)) { FAIL(#expr " is false"); goto done; } } while(0)

#define ASSERT_INT_EQ(a, b) \
    do { if ((a) != (b)) { char _m[64]; snprintf(_m,64,"%d != %d",(a),(b)); FAIL(_m); goto done; } } while(0)

/* ══════════════════════════════════════════════════════════════════════════ */
/* Physics tests                                                             */
/* ══════════════════════════════════════════════════════════════════════════ */

static void test_physics_body_create(void) {
    TEST("physics_body_create: position and mass are set correctly");
    PhysicsBody *b = physics_body_create(1, 10.f, 1.f, 2.f, 3.f, SHAPE_BOX);
    ASSERT_TRUE(b != NULL);
    ASSERT_NEAR(b->position[0], 1.f, 0.001f);
    ASSERT_NEAR(b->position[1], 2.f, 0.001f);
    ASSERT_NEAR(b->position[2], 3.f, 0.001f);
    ASSERT_NEAR(b->mass, 10.f, 0.001f);
    ASSERT_INT_EQ(b->is_static, 0);
    physics_body_destroy(b);
    PASS();
    return;
done:
    if (b) physics_body_destroy(b);
}

static void test_physics_static_body(void) {
    TEST("physics_body: mass=0 makes body static");
    PhysicsBody *b = physics_body_create(1, 0.f, 0, 0, 0, SHAPE_BOX);
    ASSERT_TRUE(b != NULL);
    ASSERT_INT_EQ(b->is_static, 1);
    physics_body_destroy(b);
    PASS();
    return;
done:
    if (b) physics_body_destroy(b);
}

static void test_dynamics_gravity(void) {
    TEST("dynamics_integrate: body falls under gravity");
    PhysicsBody *b = physics_body_create(1, 1.f, 0.f, 10.f, 0.f, SHAPE_BOX);
    ASSERT_TRUE(b != NULL);
    float start_y = b->position[1];
    dynamics_integrate(b, 1.f / 60.f);
    ASSERT_TRUE(b->position[1] < start_y); /* must fall */
    physics_body_destroy(b);
    PASS();
    return;
done:
    if (b) physics_body_destroy(b);
}

static void test_dynamics_static_no_move(void) {
    TEST("dynamics_integrate: static body does not move");
    PhysicsBody *b = physics_body_create(1, 0.f, 5.f, 5.f, 5.f, SHAPE_BOX);
    float px = b->position[0], py = b->position[1], pz = b->position[2];
    dynamics_integrate(b, 1.f / 60.f);
    ASSERT_NEAR(b->position[0], px, 0.001f);
    ASSERT_NEAR(b->position[1], py, 0.001f);
    ASSERT_NEAR(b->position[2], pz, 0.001f);
    physics_body_destroy(b);
    PASS();
    return;
done:
    if (b) physics_body_destroy(b);
}

static void test_physics_impulse(void) {
    TEST("physics_body_apply_impulse: velocity changes immediately");
    PhysicsBody *b = physics_body_create(1, 2.f, 0, 0, 0, SHAPE_BOX);
    physics_body_apply_impulse(b, 10.f, 0.f, 0.f);
    ASSERT_NEAR(b->velocity[0], 5.f, 0.01f); /* impulse / mass = 10/2 */
    physics_body_destroy(b);
    PASS();
    return;
done:
    if (b) physics_body_destroy(b);
}

static void test_box_box_collision(void) {
    TEST("collider_box_vs_box: overlapping boxes produce collision");
    PhysicsBody *a = physics_body_create(1, 1.f, 0.f, 0.f, 0.f, SHAPE_BOX);
    PhysicsBody *b = physics_body_create(2, 1.f, 0.8f, 0.f, 0.f, SHAPE_BOX);
    Collision c;
    int hit = collider_box_vs_box(a, b, &c);
    ASSERT_INT_EQ(hit, 1);
    ASSERT_TRUE(c.penetration > 0.f);
    physics_body_destroy(a);
    physics_body_destroy(b);
    PASS();
    return;
done:
    if (a) physics_body_destroy(a);
    if (b) physics_body_destroy(b);
}

static void test_box_box_no_collision(void) {
    TEST("collider_box_vs_box: non-overlapping boxes have no collision");
    PhysicsBody *a = physics_body_create(1, 1.f, 0.f, 0.f, 0.f, SHAPE_BOX);
    PhysicsBody *b = physics_body_create(2, 1.f, 5.f, 0.f, 0.f, SHAPE_BOX);
    Collision c;
    int hit = collider_box_vs_box(a, b, &c);
    ASSERT_INT_EQ(hit, 0);
    physics_body_destroy(a);
    physics_body_destroy(b);
    PASS();
    return;
done:
    if (a) physics_body_destroy(a);
    if (b) physics_body_destroy(b);
}

static void test_physics_world_determinism(void) {
    TEST("physics_world: 60-frame simulation is deterministic");
    /* Run twice from same initial state, compare final positions */
    float pos1[3], pos2[3];
    for (int run = 0; run < 2; run++) {
        PhysicsWorld *w = physics_world_create();
        PhysicsBody  *b = physics_world_add_body(w, 1.f, 0.f, 10.f, 0.f, SHAPE_BOX);
        /* Floor */
        PhysicsBody *floor = physics_world_add_body(w, 0.f, 0.f, -0.5f, 0.f, SHAPE_BOX);
        floor->shape.box.half_extents[0] = 50.f;
        floor->shape.box.half_extents[1] = 0.5f;
        floor->shape.box.half_extents[2] = 50.f;
        (void)b;

        for (int i = 0; i < 60; i++)
            physics_world_step_fixed(w);

        float *out = (run == 0) ? pos1 : pos2;
        out[0] = w->bodies[0]->position[0];
        out[1] = w->bodies[0]->position[1];
        out[2] = w->bodies[0]->position[2];
        physics_world_destroy(w);
    }
    ASSERT_NEAR(pos1[0], pos2[0], 0.0001f);
    ASSERT_NEAR(pos1[1], pos2[1], 0.0001f);
    ASSERT_NEAR(pos1[2], pos2[2], 0.0001f);
    PASS();
    return;
done:;
}

/* ══════════════════════════════════════════════════════════════════════════ */
/* State machine tests                                                       */
/* ══════════════════════════════════════════════════════════════════════════ */

typedef struct { int enter_count; int update_count; const char *last_trigger; } FSMCtx;

static void on_enter_idle(void *c)          { ((FSMCtx*)c)->enter_count++; }
static void on_update_idle(void *c, float d) { (void)d; ((FSMCtx*)c)->update_count++; }
static void on_exit_idle(void *c)            { (void)c; }
static void on_enter_patrol(void *c)         { ((FSMCtx*)c)->enter_count++; }

static void test_fsm_initial_state(void) {
    TEST("fsm: first added state becomes initial state");
    FSMCtx ctx = {0};
    StateMachine fsm;
    fsm_init(&fsm, &ctx);
    fsm_add_state(&fsm, "idle",   on_enter_idle, on_update_idle, on_exit_idle);
    fsm_add_state(&fsm, "patrol", on_enter_patrol, NULL, NULL);
    ASSERT_TRUE(strcmp(fsm_current_state_name(&fsm), "idle") == 0);
    ASSERT_INT_EQ(ctx.enter_count, 1); /* on_enter called once on init */
    PASS();
    return;
done:;
}

static void test_fsm_transition(void) {
    TEST("fsm_trigger: transitions to correct state on trigger");
    FSMCtx ctx = {0};
    StateMachine fsm;
    fsm_init(&fsm, &ctx);
    int s_idle   = fsm_add_state(&fsm, "idle",   on_enter_idle, on_update_idle, on_exit_idle);
    int s_patrol = fsm_add_state(&fsm, "patrol", on_enter_patrol, NULL, NULL);
    fsm_add_transition(&fsm, s_idle, s_patrol, "start_patrol");
    fsm_trigger(&fsm, "start_patrol");
    ASSERT_TRUE(strcmp(fsm_current_state_name(&fsm), "patrol") == 0);
    ASSERT_INT_EQ(ctx.enter_count, 2); /* idle + patrol enter */
    PASS();
    return;
done:;
}

static void test_fsm_update(void) {
    TEST("fsm_update: calls on_update for current state");
    FSMCtx ctx = {0};
    StateMachine fsm;
    fsm_init(&fsm, &ctx);
    fsm_add_state(&fsm, "idle", on_enter_idle, on_update_idle, on_exit_idle);
    fsm_update(&fsm, 0.016f);
    fsm_update(&fsm, 0.016f);
    ASSERT_INT_EQ(ctx.update_count, 2);
    PASS();
    return;
done:;
}

/* ══════════════════════════════════════════════════════════════════════════ */
/* Navigation tests                                                          */
/* ══════════════════════════════════════════════════════════════════════════ */

static void test_nav_find_path_simple(void) {
    TEST("nav_find_path: finds path on open grid");
    NavGrid *g = nav_grid_create(1.f, 0.f, 0.f);
    NavPath  path;
    int ok = nav_find_path(g, 0.f, 0.f, 5.f, 5.f, &path);
    ASSERT_INT_EQ(ok, 1);
    ASSERT_TRUE(path.count > 0);
    nav_grid_destroy(g);
    PASS();
    return;
done:
    nav_grid_destroy(g);
}

static void test_nav_find_path_blocked(void) {
    TEST("nav_find_path: returns 0 when fully blocked");
    NavGrid *g = nav_grid_create(1.f, 0.f, 0.f);
    /* Block everything except start column */
    for (int r = 0; r < NAV_GRID_H; r++)
        for (int c = 1; c < NAV_GRID_W; c++)
            nav_grid_set_blocked(g, c, r, 1);
    NavPath path;
    int ok = nav_find_path(g, 0.f, 0.f, 10.f, 10.f, &path);
    ASSERT_INT_EQ(ok, 0);
    nav_grid_destroy(g);
    PASS();
    return;
done:
    nav_grid_destroy(g);
}

static void test_nav_move_along_path(void) {
    TEST("nav_move_along_path: position advances toward waypoint");
    NavPath path;
    path.count = 1;
    path.current_index = 0;
    path.waypoints[0][0] = 10.f;
    path.waypoints[0][1] = 0.f;
    path.waypoints[0][2] = 0.f;

    float pos[3] = {0.f, 0.f, 0.f};
    nav_move_along_path(&path, pos, 5.f, 1.f);
    ASSERT_NEAR(pos[0], 5.f, 0.01f);
    PASS();
    return;
done:;
}

/* ══════════════════════════════════════════════════════════════════════════ */
/* Camera / math tests                                                       */
/* ══════════════════════════════════════════════════════════════════════════ */

static void test_camera_view_matrix(void) {
    TEST("camera_get_view: produces valid (non-zero) matrix");
    Camera c = camera_create(60.f, 1.f, 0.1f, 100.f);
    camera_look_at(&c, 0, 5, 10, 0, 0, 0);
    float m[16] = {0};
    camera_get_view(&c, m);
    /* Diagonal elements should not all be zero */
    ASSERT_TRUE(fabsf(m[0]) > 0.001f || fabsf(m[5]) > 0.001f);
    PASS();
    return;
done:;
}

static void test_camera_projection_matrix(void) {
    TEST("camera_get_projection: m[0] and m[5] non-zero");
    Camera c = camera_create(60.f, 16.f/9.f, 0.1f, 1000.f);
    float m[16] = {0};
    camera_get_projection(&c, m);
    ASSERT_TRUE(fabsf(m[0]) > 0.001f);
    ASSERT_TRUE(fabsf(m[5]) > 0.001f);
    PASS();
    return;
done:;
}

/* ══════════════════════════════════════════════════════════════════════════ */
/* Renderer stub tests                                                       */
/* ══════════════════════════════════════════════════════════════════════════ */

static void test_renderer_frame_buffer_non_null(void) {
    TEST("renderer_stub: frame buffer is non-NULL after end_frame");
    Renderer *r = renderer_create(8, 8);
    ASSERT_TRUE(r != NULL);
    renderer_begin_frame(r);
    renderer_end_frame(r);
    const unsigned char *fb = renderer_get_frame_buffer(r);
    ASSERT_TRUE(fb != NULL);
    renderer_destroy(r);
    PASS();
    return;
done:
    if (r) renderer_destroy(r);
}

static void test_renderer_frame_buffer_non_zero(void) {
    TEST("renderer_stub: frame buffer contains non-zero pixels");
    Renderer *r = renderer_create(8, 8);
    renderer_begin_frame(r);
    renderer_end_frame(r);
    const unsigned char *fb = renderer_get_frame_buffer(r);
    int has_nonzero = 0;
    for (int i = 0; i < 8*8*4; i++) if (fb[i] != 0) { has_nonzero = 1; break; }
    ASSERT_TRUE(has_nonzero);
    renderer_destroy(r);
    PASS();
    return;
done:
    if (r) renderer_destroy(r);
}

static void test_mesh_create_cube(void) {
    TEST("mesh_create_cube: returns non-NULL mesh");
    Mesh *m = mesh_create_cube();
    ASSERT_TRUE(m != NULL);
    mesh_destroy(m);
    PASS();
    return;
done:;
}

/* ══════════════════════════════════════════════════════════════════════════ */
/* World / entity tests                                                      */
/* ══════════════════════════════════════════════════════════════════════════ */

static void test_world_spawn_entity(void) {
    TEST("world_spawn_entity: returns non-zero EntityId");
    World *w = world_create();
    EntityId id = world_spawn_entity(w, ENTITY_TYPE_NPC, 1.f, 0.f, 1.f);
    ASSERT_TRUE(id != 0);
    Entity *e = world_get_entity(w, id);
    ASSERT_TRUE(e != NULL);
    ASSERT_NEAR(e->position[0], 1.f, 0.001f);
    world_destroy(w);
    PASS();
    return;
done:
    world_destroy(w);
}

static void test_world_despawn_entity(void) {
    TEST("world_despawn_entity: entity no longer retrievable");
    World *w = world_create();
    EntityId id = world_spawn_entity(w, ENTITY_TYPE_STATIC, 0, 0, 0);
    world_despawn_entity(w, id);
    Entity *e = world_get_entity(w, id);
    ASSERT_TRUE(e == NULL);
    world_destroy(w);
    PASS();
    return;
done:
    world_destroy(w);
}

static void test_world_query_nearby(void) {
    TEST("world_query_nearby: finds entities within radius");
    World *w = world_create();
    EntityId id1 = world_spawn_entity(w, ENTITY_TYPE_NPC, 1.f, 0, 1.f);
    EntityId id2 = world_spawn_entity(w, ENTITY_TYPE_NPC, 100.f, 0, 100.f);
    (void)id2;
    float center[3] = {0, 0, 0};
    EntityId found[8];
    int count = world_query_nearby(w, center, 5.f, found, 8);
    ASSERT_INT_EQ(count, 1);
    ASSERT_INT_EQ((int)found[0], (int)id1);
    world_destroy(w);
    PASS();
    return;
done:
    world_destroy(w);
}

static void test_signals_send_and_flush(void) {
    TEST("signals_send: queue count increases");
    World *w = world_create();
    world_spawn_entity(w, ENTITY_TYPE_NPC, 0, 0, 0);
    SignalQueue *q = signals_get_queue();
    int before = q->count;
    signals_send(1, "player_nearby", NULL);
    ASSERT_INT_EQ(q->count, before + 1);
    signals_flush(w);
    ASSERT_INT_EQ(q->count, 0);
    world_destroy(w);
    PASS();
    return;
done:
    world_destroy(w);
}

/* ══════════════════════════════════════════════════════════════════════════ */
/* Main                                                                      */
/* ══════════════════════════════════════════════════════════════════════════ */

int main(void) {
    printf("\n═══ Banana Engine Phase 1 Tests ═══\n\n");

    printf("── Physics ──\n");
    test_physics_body_create();
    test_physics_static_body();
    test_dynamics_gravity();
    test_dynamics_static_no_move();
    test_physics_impulse();
    test_box_box_collision();
    test_box_box_no_collision();
    test_physics_world_determinism();

    printf("\n── State Machine ──\n");
    test_fsm_initial_state();
    test_fsm_transition();
    test_fsm_update();

    printf("\n── Navigation ──\n");
    test_nav_find_path_simple();
    test_nav_find_path_blocked();
    test_nav_move_along_path();

    printf("\n── Camera / Math ──\n");
    test_camera_view_matrix();
    test_camera_projection_matrix();

    printf("\n── Renderer (stub mode) ──\n");
    test_renderer_frame_buffer_non_null();
    test_renderer_frame_buffer_non_zero();
    test_mesh_create_cube();

    printf("\n── World / Entities / Signals ──\n");
    test_world_spawn_entity();
    test_world_despawn_entity();
    test_world_query_nearby();
    test_signals_send_and_flush();

    printf("\n═══════════════════════════════════════════════════════\n");
    printf("Results: %d passed, %d failed\n", s_pass, s_fail);
    return s_fail > 0 ? 1 : 0;
}
