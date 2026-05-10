#include "test_engine_domain_suite_framework.h"

#include "../../../src/native/engine/render/camera.h"
#include "../../../src/native/engine/render/mesh.h"
#include "../../../src/native/engine/render/renderer.h"

static void test_camera_view_matrix(DomainSuiteStats *stats)
{
    Camera c;
    float m[16] = {0};

    SUITE_TEST("camera_get_view: produces valid (non-zero) matrix");
    c = camera_create(60.f, 1.f, 0.1f, 100.f);
    camera_look_at(&c, 0, 5, 10, 0, 0, 0);
    camera_get_view(&c, m);
    SUITE_ASSERT_TRUE(stats, fabsf(m[0]) > 0.001f || fabsf(m[5]) > 0.001f);
    SUITE_PASS(stats);
done:
    return;
}

static void test_camera_projection_matrix(DomainSuiteStats *stats)
{
    Camera c;
    float m[16] = {0};

    SUITE_TEST("camera_get_projection: m[0] and m[5] non-zero");
    c = camera_create(60.f, 16.f / 9.f, 0.1f, 1000.f);
    camera_get_projection(&c, m);
    SUITE_ASSERT_TRUE(stats, fabsf(m[0]) > 0.001f);
    SUITE_ASSERT_TRUE(stats, fabsf(m[5]) > 0.001f);
    SUITE_PASS(stats);
done:
    return;
}

static void test_renderer_frame_buffer_non_null(DomainSuiteStats *stats)
{
    Renderer *r = NULL;
    const unsigned char *fb = NULL;

    SUITE_TEST("renderer_stub: frame buffer is non-NULL after end_frame");
    r = renderer_create(8, 8);
    SUITE_ASSERT_TRUE(stats, r != NULL);
    renderer_begin_frame(r);
    renderer_end_frame(r);
    fb = renderer_get_frame_buffer(r);
    SUITE_ASSERT_TRUE(stats, fb != NULL);
    SUITE_PASS(stats);
done:
    if (r)
        renderer_destroy(r);
}

static void test_renderer_frame_buffer_non_zero(DomainSuiteStats *stats)
{
    Renderer *r = NULL;
    const unsigned char *fb = NULL;
    int has_nonzero = 0;

    SUITE_TEST("renderer_stub: frame buffer contains non-zero pixels");
    r = renderer_create(8, 8);
    SUITE_ASSERT_TRUE(stats, r != NULL);
    renderer_begin_frame(r);
    renderer_end_frame(r);
    fb = renderer_get_frame_buffer(r);
    SUITE_ASSERT_TRUE(stats, fb != NULL);

    for (int i = 0; i < 8 * 8 * 4; i++)
    {
        if (fb[i] != 0)
        {
            has_nonzero = 1;
            break;
        }
    }

    SUITE_ASSERT_TRUE(stats, has_nonzero);
    SUITE_PASS(stats);
done:
    if (r)
        renderer_destroy(r);
}

static void test_mesh_create_banana(DomainSuiteStats *stats)
{
    Mesh *m = NULL;

    SUITE_TEST("mesh_create_banana: returns non-NULL mesh");
    m = mesh_create_banana();
    SUITE_ASSERT_TRUE(stats, m != NULL);
    SUITE_PASS(stats);
done:
    if (m)
        mesh_destroy(m);
}

int run_engine_render_suite(void)
{
    DomainSuiteStats stats = {0, 0};

    printf("-- Rendering Suite --\n");
    test_camera_view_matrix(&stats);
    test_camera_projection_matrix(&stats);
    test_renderer_frame_buffer_non_null(&stats);
    test_renderer_frame_buffer_non_zero(&stats);
    test_mesh_create_banana(&stats);

    printf("Rendering: %d passed, %d failed\n\n", stats.pass, stats.fail);
    return stats.fail;
}
