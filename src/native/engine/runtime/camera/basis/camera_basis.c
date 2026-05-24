#include "camera_basis.h"

static void vec3_set(float *out, float x, float y, float z)
{
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

int runtime_camera_compute_ground_basis(const float *camera_eye,
                                        const float *camera_target,
                                        int camera_valid,
                                        float *forward,
                                        float *right)
{
    if (!forward || !right)
        return 0;

    if (!camera_eye || !camera_target || !camera_valid)
        return 0;

    (void)camera_eye;
    (void)camera_target;

    vec3_set(forward, 0.f, 0.f, -1.f);
    vec3_set(right, 1.f, 0.f, 0.f);

    return 1;
}
