#include "render/camera.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

static void test_camera_matrices_are_valid(void **state)
{
    (void)state;
    Camera camera = camera_create(60.0f, 1.0f, 0.1f, 100.0f);
    float view[16] = {0};
    float proj[16] = {0};
    float vp[16] = {0};

    camera_look_at(&camera, 0.0f, 5.0f, 10.0f, 0.0f, 0.0f, 0.0f);
    camera_get_view(&camera, view);
    camera_get_projection(&camera, proj);
    camera_get_view_projection(&camera, vp);

    assert_float_equal(view[15], 1.0f, 0.0001f);
    assert_true(proj[10] < 0.0f);
    assert_true(vp[15] > 0.0f);
    assert_true(vp[0] > 0.0f);
    assert_true(vp[5] > 0.0f);
    assert_true(vp[10] < 0.0f);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_camera_matrices_are_valid),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#else
#include <stdio.h>
#include <string.h>

static int fail_if(int condition, const char *message)
{
    if (condition)
    {
        fprintf(stderr, "%s\n", message);
        return 1;
    }
    return 0;
}

static int test_camera_matrices_are_valid(void)
{
    Camera camera = camera_create(60.0f, 1.0f, 0.1f, 100.0f);
    float view[16] = {0};
    float proj[16] = {0};
    float vp[16] = {0};

    camera_look_at(&camera, 0.0f, 5.0f, 10.0f, 0.0f, 0.0f, 0.0f);
    camera_get_view(&camera, view);
    camera_get_projection(&camera, proj);
    camera_get_view_projection(&camera, vp);

    if (fail_if(view[15] != 1.0f, "camera_get_view must write a valid homogeneous matrix") ||
        fail_if(proj[10] >= 0.0f, "camera_get_projection must use the standard clip-space convention") ||
        fail_if(vp[15] <= 0.0f, "camera_get_view_projection must combine the two transforms into a non-trivial matrix"))
        return 1;

    return fail_if(vp[0] <= 0.0f || vp[5] <= 0.0f || vp[10] >= 0.0f,
                   "camera_get_view_projection must preserve the expected perspective and view orientation");
}

int main(void)
{
    return test_camera_matrices_are_valid();
}
#endif
