#include "render/camera.h"
#include "../../support/test_support.h"

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

    BANANA_TEST_ASSERT_FLOAT_EQ(view[15], 1.0f, 0.0001f,
                                "camera_get_view must write valid homogeneous matrix");
    BANANA_TEST_ASSERT_TRUE(proj[10] < 0.0f,
                            "camera_get_projection must follow clip-space convention");
    BANANA_TEST_ASSERT_TRUE(vp[15] > 0.0f,
                            "camera_get_view_projection must be non-trivial");
    BANANA_TEST_ASSERT_TRUE(vp[0] > 0.0f && vp[5] > 0.0f && vp[10] < 0.0f,
                            "camera_get_view_projection must preserve perspective orientation");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_camera_matrices_are_valid)
)
