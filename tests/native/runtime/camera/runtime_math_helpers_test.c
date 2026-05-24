#include "runtime/camera/camera_basis.h"
#include "runtime/terrain/terrain_height.h"

#include <math.h>
#include <stdio.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    float eye[3] = {4.0f, 3.0f, 8.0f};
    float target[3] = {0.0f, 1.0f, 0.0f};
    float forward[3] = {0.0f, 0.0f, 0.0f};
    float right[3] = {0.0f, 0.0f, 0.0f};
    unsigned char terrain[9] = {
        0, 1, 2,
        1, 2, 3,
        2, 3, 4,
    };
    float h_center = 0.0f;
    float h_clamped = 0.0f;

    if (!expect_true("basis valid", runtime_camera_compute_ground_basis(eye, target, 1, forward, right) == 1))
        return 1;

    if (!expect_true("forward normalized", fabsf((forward[0] * forward[0] + forward[2] * forward[2]) - 1.0f) < 0.001f))
        return 1;
    if (!expect_true("right normalized", fabsf((right[0] * right[0] + right[2] * right[2]) - 1.0f) < 0.001f))
        return 1;
    if (!expect_true("orthogonal", fabsf((forward[0] * right[0]) + (forward[2] * right[2])) < 0.001f))
        return 1;

    if (!expect_true("basis invalid guard",
                     runtime_camera_compute_ground_basis(eye, target, 0, forward, right) == 0))
        return 1;

    h_center = runtime_terrain_sample_height(terrain, 3, 0.0f, 0.0f, 0.5f, -2.0f);
    h_clamped = runtime_terrain_sample_height(terrain, 3, 100.0f, 100.0f, 0.5f, -2.0f);

    if (!expect_true("center sample", fabsf(h_center - (-1.0f)) < 0.001f))
        return 1;
    if (!expect_true("clamped sample", fabsf(h_clamped - 0.0f) < 0.001f))
        return 1;

    return 0;
}
