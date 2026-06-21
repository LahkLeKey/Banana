#include "runtime/camera/basis/camera_basis.h"
#include "../../support/test_support.h"

static void test_camera_ground_basis(void **state)
{
    (void)state;
    float forward[3] = {0.0f, 0.0f, 0.0f};
    float right[3] = {0.0f, 0.0f, 0.0f};
    const float eye[3] = {3.0f, 8.0f, 12.0f};
    const float target[3] = {0.0f, 0.0f, 0.0f};

    BANANA_TEST_ASSERT_INT_EQ(runtime_camera_compute_ground_basis(eye, target, 0, forward, right), 0,
                              "camera basis must reject invalid camera state");
    BANANA_TEST_ASSERT_INT_EQ(runtime_camera_compute_ground_basis(NULL, target, 1, forward, right), 0,
                              "camera basis must reject null eye vectors");
    BANANA_TEST_ASSERT_INT_EQ(runtime_camera_compute_ground_basis(eye, target, 1, NULL, right), 0,
                              "camera basis must reject null forward vectors");

    BANANA_TEST_ASSERT_INT_EQ(runtime_camera_compute_ground_basis(eye, target, 1, forward, right), 1,
                              "camera basis must accept a valid camera state");
    BANANA_TEST_ASSERT_FLOAT_EQ(forward[0], 0.0f, 0.0001f,
                                "camera basis forward x must be deterministic");
    BANANA_TEST_ASSERT_FLOAT_EQ(forward[1], 0.0f, 0.0001f,
                                "camera basis forward y must be deterministic");
    BANANA_TEST_ASSERT_FLOAT_EQ(forward[2], -1.0f, 0.0001f,
                                "camera basis forward z must point toward -z");
    BANANA_TEST_ASSERT_FLOAT_EQ(right[0], 1.0f, 0.0001f,
                                "camera basis right x must point toward +x");
    BANANA_TEST_ASSERT_FLOAT_EQ(right[1], 0.0f, 0.0001f,
                                "camera basis right y must be zero");
    BANANA_TEST_ASSERT_FLOAT_EQ(right[2], 0.0f, 0.0001f,
                                "camera basis right z must be zero");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_camera_ground_basis)
)