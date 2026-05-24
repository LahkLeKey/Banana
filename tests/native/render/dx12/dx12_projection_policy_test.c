#include "render/dx12/backend_dx12_projection_policy.h"

#include <math.h>
#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_float_in_range(const char *label, float value, float min_value, float max_value)
{
    if (value >= min_value && value <= max_value)
        return 1;

    fprintf(stderr,
            "%s expected range=[%.4f, %.4f] actual=%.4f\n",
            label,
            min_value,
            max_value,
            value);
    return 0;
}

static int expect_float_close(const char *label, float actual, float expected, float tolerance)
{
    float delta = fabsf(actual - expected);
    if (delta <= tolerance)
        return 1;

    fprintf(stderr,
            "%s expected=%.5f actual=%.5f delta=%.5f tolerance=%.5f\n",
            label,
            expected,
            actual,
            delta,
            tolerance);
    return 0;
}

int main(void)
{
    BananaDx12ProjectionCamera camera = {0};
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float depth = 0.0f;
    float centered_x = 0.0f;

    banana_dx12_projection_world_to_clip(NULL,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         &x,
                                         &y,
                                         &z,
                                         &depth);

    if (!expect_float_in_range("default clip x", x, -1.25f, 1.25f))
        return 1;
    if (!expect_float_in_range("default clip y", y, -1.25f, 1.25f))
        return 1;
    if (!expect_float_in_range("default clip z", z, 0.02f, 0.98f))
        return 1;
    if (!expect_float_in_range("default depth", depth, 0.01f, 10000.0f))
        return 1;

    camera.eye[0] = 0.0f;
    camera.eye[1] = 14.0f;
    camera.eye[2] = -16.0f;
    camera.target[0] = 0.0f;
    camera.target[1] = 0.0f;
    camera.target[2] = 0.0f;
    camera.up[0] = 0.0f;
    camera.up[1] = 1.0f;
    camera.up[2] = 0.0f;
    camera.fov_degrees = 52.0f;
    camera.aspect = 1280.0f / 720.0f;
    camera.valid = 1;

    banana_dx12_projection_world_to_clip(&camera,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         &x,
                                         &y,
                                         &z,
                                         &depth);

    if (!expect_float_close("origin center x", x, 0.0f, 0.10f))
        return 1;
    if (!expect_float_in_range("origin center y", y, -0.20f, 0.20f))
        return 1;

    camera.eye[0] = 10.0f;
    camera.target[0] = 10.0f;

    banana_dx12_projection_world_to_clip(&camera,
                                         10.0f,
                                         0.0f,
                                         0.0f,
                                         &centered_x,
                                         &y,
                                         &z,
                                         &depth);

    if (!expect_float_close("translated camera center x", centered_x, 0.0f, 0.10f))
        return 1;

    banana_dx12_projection_world_to_clip(&camera,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         &x,
                                         &y,
                                         &z,
                                         &depth);

    if (!expect_int("off-center point should not remain centered",
                    fabsf(x) > 0.10f ? 1 : 0,
                    1))
        return 1;

    return 0;
}
