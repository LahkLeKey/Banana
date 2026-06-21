#include "render/dx12/backend_dx12_projection_policy.h"

#include "../../support/test_support.h"

static void test_projection_with_default_camera(void **state)
{
    (void)state;
    float clip_x = 0.f, clip_y = 0.f, clip_z = 0.f, view_depth = 0.f;

    banana_dx12_projection_world_to_clip(NULL,
                                         0.f,
                                         0.f,
                                         0.f,
                                         &clip_x,
                                         &clip_y,
                                         &clip_z,
                                         &view_depth);

    BANANA_TEST_ASSERT_TRUE(clip_x >= -1.25f && clip_x <= 1.25f,
                            "default projection must clamp clip x into range");
    BANANA_TEST_ASSERT_TRUE(clip_y >= -1.25f && clip_y <= 1.25f,
                            "default projection must clamp clip y into range");
    BANANA_TEST_ASSERT_TRUE(clip_z >= 0.02f && clip_z <= 0.98f,
                            "default projection must clamp clip z into range");
    BANANA_TEST_ASSERT_TRUE(view_depth > 0.f,
                            "default projection must produce positive view depth");
}

static void test_projection_with_valid_camera_and_clamps(void **state)
{
    (void)state;
    BananaDx12ProjectionCamera camera = {0};
    float clip_x = 0.f, clip_y = 0.f, clip_z = 0.f, view_depth = 0.f;

    camera.eye[0] = 1.f;
    camera.eye[1] = 2.f;
    camera.eye[2] = 3.f;
    camera.target[0] = 1.f;
    camera.target[1] = 2.f;
    camera.target[2] = 3.f; /* degenerate forward vector triggers normalize fallback */
    camera.up[0] = 0.f;
    camera.up[1] = 0.f;
    camera.up[2] = 0.f;     /* degenerate up vector */
    camera.fov_degrees = 400.f; /* outside valid range falls back to default tan_half_fov */
    camera.aspect = 10.f;       /* clamped down to 4.0 */
    camera.valid = 1;

    banana_dx12_projection_world_to_clip(&camera,
                                         1000.f,
                                         1000.f,
                                         1000.f,
                                         &clip_x,
                                         &clip_y,
                                         &clip_z,
                                         &view_depth);

    BANANA_TEST_ASSERT_TRUE(clip_x >= -1.25f && clip_x <= 1.25f,
                            "valid camera path must clamp clip x into range");
    BANANA_TEST_ASSERT_TRUE(clip_y >= -1.25f && clip_y <= 1.25f,
                            "valid camera path must clamp clip y into range");
    BANANA_TEST_ASSERT_TRUE(clip_z >= 0.02f && clip_z <= 0.98f,
                            "valid camera path must clamp clip z into range");
    BANANA_TEST_ASSERT_TRUE(view_depth > 0.f,
                            "valid camera path must produce positive view depth");
}

static void test_projection_with_custom_camera_outputs(void **state)
{
    (void)state;
    BananaDx12ProjectionCamera camera = {0};
    float clip_x = 0.f, clip_y = 0.f, clip_z = 0.f, view_depth = 0.f;

    camera.eye[0] = 0.f;
    camera.eye[1] = 0.f;
    camera.eye[2] = -10.f;
    camera.target[0] = 0.f;
    camera.target[1] = 0.f;
    camera.target[2] = 0.f;
    camera.up[0] = 0.f;
    camera.up[1] = 1.f;
    camera.up[2] = 0.f;
    camera.fov_degrees = 60.f;
    camera.aspect = 1.5f;
    camera.valid = 1;

    banana_dx12_projection_world_to_clip(&camera,
                                         0.f,
                                         0.f,
                                         5.f,
                                         &clip_x,
                                         &clip_y,
                                         &clip_z,
                                         &view_depth);

    BANANA_TEST_ASSERT_FLOAT_EQ(clip_x, 0.f, 0.25f,
                                "centered point should project near clip-space center x");
    BANANA_TEST_ASSERT_FLOAT_EQ(clip_y, 0.f, 0.25f,
                                "centered point should project near clip-space center y");
    BANANA_TEST_ASSERT_TRUE(view_depth > 0.f,
                            "custom camera must produce positive depth");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_projection_with_default_camera),
    BANANA_TEST_CASE(test_projection_with_valid_camera_and_clamps),
    BANANA_TEST_CASE(test_projection_with_custom_camera_outputs)
)
