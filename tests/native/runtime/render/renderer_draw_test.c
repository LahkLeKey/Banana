#include "render/renderer.h"
#include "render/renderer_internal.h"
#include "render/mesh.h"
#include "../support/test_support.h"

#include <stdint.h>
#include <string.h>

static int g_dx12_submit_calls = 0;
static int g_last_uses_texture = -1;
static const Mesh *g_last_mesh = NULL;
static float g_last_position[3] = {0.0f, 0.0f, 0.0f};
static float g_last_scale[3] = {0.0f, 0.0f, 0.0f};

void banana_dx12_runtime_submit_scene_draw(const Mesh *mesh,
                                           const float *position,
                                           const float *scale,
                                           int uses_texture,
                                           float color_r,
                                           float color_g,
                                           float color_b)
{
    (void)color_r;
    (void)color_g;
    (void)color_b;

    g_dx12_submit_calls += 1;
    g_last_mesh = mesh;
    g_last_uses_texture = uses_texture;
    g_last_position[0] = position[0];
    g_last_position[1] = position[1];
    g_last_position[2] = position[2];
    g_last_scale[0] = scale[0];
    g_last_scale[1] = scale[1];
    g_last_scale[2] = scale[2];
}

static void reset_renderer_probe(void)
{
    g_dx12_submit_calls = 0;
    g_last_uses_texture = -1;
    g_last_mesh = NULL;
    memset(g_last_position, 0, sizeof(g_last_position));
    memset(g_last_scale, 0, sizeof(g_last_scale));
}

static RendererDrawCommand make_command(Mesh *mesh, float use_texture)
{
    RendererDrawCommand command;

    memset(&command, 0, sizeof(command));
    command.mesh = mesh;
    command.position[0] = 3.0f;
    command.position[1] = 4.0f;
    command.position[2] = 5.0f;
    command.scale[0] = 1.0f;
    command.scale[1] = 2.0f;
    command.scale[2] = 3.0f;
    command.material.use_texture = use_texture;
    command.material.color[0] = 0.1f;
    command.material.color[1] = 0.2f;
    command.material.color[2] = 0.3f;

    return command;
}

static void test_renderer_draw_command_guards_and_submit(void **state)
{
    Renderer renderer;
    RendererDrawCommand command;
    Mesh *mesh = (Mesh *)(uintptr_t)0xBEEF;

    (void)state;

    memset(&renderer, 0, sizeof(renderer));
    command = make_command(mesh, 1.0f);

    reset_renderer_probe();

    renderer_draw_command(NULL, &command);
    renderer_draw_command(&renderer, NULL);

    command.mesh = NULL;
    renderer_draw_command(&renderer, &command);

    BANANA_TEST_ASSERT_INT_EQ(g_dx12_submit_calls, 0,
                              "renderer_draw_command must no-op on invalid inputs");

    command = make_command(mesh, 1.0f);
    renderer_draw_command(&renderer, &command);

    BANANA_TEST_ASSERT_INT_EQ(g_dx12_submit_calls, 1,
                              "renderer_draw_command must submit draws for valid commands");
    BANANA_TEST_ASSERT_TRUE(g_last_mesh == mesh,
                            "renderer_draw_command must forward mesh pointer");
    BANANA_TEST_ASSERT_INT_EQ(g_last_uses_texture,
                              1,
                              "renderer_draw_command must request textured draw when use_texture > 0.5");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_position[0], 3.0f, 0.0001f,
                                "renderer_draw_command must forward position");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_scale[2], 3.0f, 0.0001f,
                                "renderer_draw_command must forward scale");

    command = make_command(mesh, 0.0f);
    renderer_draw_command(&renderer, &command);

    BANANA_TEST_ASSERT_INT_EQ(g_dx12_submit_calls, 2,
                              "renderer_draw_command must submit repeated draws");
    BANANA_TEST_ASSERT_INT_EQ(g_last_uses_texture,
                              0,
                              "renderer_draw_command must disable textures when use_texture <= 0.5");
}

static void test_renderer_draw_mesh_guards_and_bridge(void **state)
{
    Renderer renderer;
    Mesh *mesh = (Mesh *)(uintptr_t)0xCAFE;
    float position[3] = {7.0f, 8.0f, 9.0f};
    float rotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float scale[3] = {2.0f, 2.0f, 2.0f};
    Material material;

    (void)state;

    memset(&renderer, 0, sizeof(renderer));
    memset(&material, 0, sizeof(material));
    material.use_texture = 1.0f;
    material.color[0] = 0.7f;
    material.color[1] = 0.2f;
    material.color[2] = 0.5f;

    reset_renderer_probe();

    renderer_draw_mesh(&renderer, NULL, position, rotation, scale, &material);
    renderer_draw_mesh(&renderer, mesh, NULL, rotation, scale, &material);
    renderer_draw_mesh(&renderer, mesh, position, NULL, scale, &material);
    renderer_draw_mesh(&renderer, mesh, position, rotation, NULL, &material);
    renderer_draw_mesh(&renderer, mesh, position, rotation, scale, NULL);

    BANANA_TEST_ASSERT_INT_EQ(g_dx12_submit_calls, 0,
                              "renderer_draw_mesh must no-op when required arguments are missing");

    renderer_draw_mesh(&renderer, mesh, position, rotation, scale, &material);

    BANANA_TEST_ASSERT_INT_EQ(g_dx12_submit_calls, 1,
                              "renderer_draw_mesh must submit via renderer_draw_command");
    BANANA_TEST_ASSERT_TRUE(g_last_mesh == mesh,
                            "renderer_draw_mesh must forward mesh to draw command");
    BANANA_TEST_ASSERT_FLOAT_EQ(g_last_position[1], 8.0f, 0.0001f,
                                "renderer_draw_mesh must forward position through command bridge");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_renderer_draw_command_guards_and_submit),
    BANANA_TEST_CASE(test_renderer_draw_mesh_guards_and_bridge)
)
