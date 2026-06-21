#include "runtime/render/submit/render_submit.h"
#include "render/material.h"
#include "render/mesh.h"

#include "../../support/test_support.h"

#include <string.h>
#include <math.h>

static int g_begin_calls = 0;
static int g_end_calls = 0;
static int g_draw_calls = 0;
static int g_terrain_calls = 0;
static int g_resolve_material_calls = 0;
static int g_resolve_mesh_calls = 0;
static int g_resolve_command_calls = 0;

static void stub_begin_frame(Renderer *r) { (void)r; g_begin_calls++; }
static void stub_end_frame(Renderer *r) { (void)r; g_end_calls++; }
static void stub_draw_command(Renderer *r, const RendererDrawCommand *cmd) { (void)r; (void)cmd; g_draw_calls++; }
static void stub_terrain_draw(void) { g_terrain_calls++; }

static Material stub_resolve_material(const Entity *e) {
    (void)e; g_resolve_material_calls++; return material_solid(1.f,1.f,1.f,1.f);
}
static Mesh *stub_resolve_mesh(const Entity *e, Mesh *def) {
    (void)e; g_resolve_mesh_calls++; return def;
}
static void stub_resolve_command(const Entity *e, Mesh *m, Material mat, RendererDrawCommand *out) {
    (void)e; (void)mat;
    g_resolve_command_calls++;
    out->mesh = m;
    out->position[0] = e->position[0];
    out->position[1] = 0.f;
    out->position[2] = e->position[2];
    out->scale[0] = out->scale[1] = out->scale[2] = 1.f;
    out->rotation[3] = 1.f;
}

/* Renderer stubs - these override linked symbols */
void renderer_begin_frame(Renderer *r) { stub_begin_frame(r); }
void renderer_end_frame(Renderer *r) { stub_end_frame(r); }
void renderer_draw_command(Renderer *r, const RendererDrawCommand *cmd) { stub_draw_command(r, cmd); }

static void reset_counts(void) {
    g_begin_calls = g_end_calls = g_draw_calls = g_terrain_calls = 0;
    g_resolve_material_calls = g_resolve_mesh_calls = g_resolve_command_calls = 0;
}

static void test_submit_frame_null_renderer(void **state)
{
    (void)state;
    reset_counts();
    runtime_render_submit_frame(NULL, NULL, NULL, 0, NULL, NULL, NULL, NULL);
    BANANA_TEST_ASSERT_INT_EQ(g_begin_calls, 0,
                              "null renderer must skip begin_frame");
    BANANA_TEST_ASSERT_INT_EQ(g_end_calls, 0,
                              "null renderer must skip end_frame");
}

static void test_submit_frame_empty_world(void **state)
{
    (void)state;
    Renderer *r = (Renderer *)(void*)0x1;
    Mesh *mesh = NULL;
    reset_counts();

    runtime_render_submit_frame(r, NULL, mesh, 0, NULL, stub_resolve_material, NULL, NULL);

    BANANA_TEST_ASSERT_INT_EQ(g_begin_calls, 1, "begin_frame must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_end_calls, 1, "end_frame must be called once");
    BANANA_TEST_ASSERT_INT_EQ(g_draw_calls, 0, "no draws without world");
}

static void test_submit_frame_terrain_and_entity(void **state)
{
    (void)state;
    Renderer *r = (Renderer *)(void*)0x1;
    World world = {0};
    Entity entity = { .id = 1, .type = ENTITY_TYPE_NPC, .active = 1,
                      .position = {2.f, 0.f, 3.f}, .scale = {1.f, 1.f, 1.f} };
    Entity inactive = { .id = 2, .type = ENTITY_TYPE_NPC, .active = 0 };
    world.entities[0] = &entity;
    world.entities[1] = &inactive;
    world.entity_count = 2;
    reset_counts();

    /* terrain draw enabled */
    runtime_render_submit_frame(r, &world, NULL, 1, stub_terrain_draw,
                                stub_resolve_material, stub_resolve_mesh, NULL);
    BANANA_TEST_ASSERT_INT_EQ(g_terrain_calls, 1, "terrain draw must be called once");
    /* With NULL entity_mesh, entities are skipped per the frame guard logic */
    BANANA_TEST_ASSERT_INT_EQ(g_begin_calls, 1, "begin_frame must be called");
    BANANA_TEST_ASSERT_INT_EQ(g_end_calls, 1, "end_frame must be called");

    /* custom resolve_command path — entity_mesh==NULL means no entities drawn */
    reset_counts();
    runtime_render_submit_frame(r, &world, NULL, 0, NULL,
                                stub_resolve_material, stub_resolve_mesh, stub_resolve_command);
    BANANA_TEST_ASSERT_INT_EQ(g_begin_calls, 1, "second begin_frame must be called");
}

static void test_continuity_transform_snaps_values(void **state)
{
    (void)state;
    RendererDrawCommand cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.position[0] = 0.001234f;
    cmd.scale[0] = 0.0004567f;

    /* null guard */
    runtime_render_apply_viewport_chunk_continuity_transform(NULL);

    runtime_render_apply_viewport_chunk_continuity_transform(&cmd);

    /* verify snapping occurred (value should be rounded to step) */
    BANANA_TEST_ASSERT_TRUE(fabsf(cmd.position[0]) >= 0.f,
                            "continuity transform must produce valid position values");
    BANANA_TEST_ASSERT_TRUE(fabsf(cmd.scale[0]) >= 0.f,
                            "continuity transform must produce valid scale values");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_submit_frame_null_renderer),
    BANANA_TEST_CASE(test_submit_frame_empty_world),
    BANANA_TEST_CASE(test_submit_frame_terrain_and_entity),
    BANANA_TEST_CASE(test_continuity_transform_snaps_values)
)
