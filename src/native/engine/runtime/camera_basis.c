#include "camera_basis.h"

#include <math.h>

static void vec3_set(float *out, float x, float y, float z)
{
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

static float vec3_len(const float *v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

static void vec3_normalize(float *v)
{
    float len = vec3_len(v);
    if (len <= 1e-6f)
        return;
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

static void vec3_cross(const float *a, const float *b, float *out)
{
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}

int runtime_camera_compute_ground_basis(const float *camera_eye,
                                        const float *camera_target,
                                        int camera_valid,
                                        float *forward,
                                        float *right)
{
    if (!camera_eye || !camera_target || !forward || !right || !camera_valid)
        return 0;

    vec3_set(forward,
             camera_target[0] - camera_eye[0],
             0.f,
             camera_target[2] - camera_eye[2]);
    if (vec3_len(forward) <= 1e-6f)
        vec3_set(forward, 0.f, 0.f, -1.f);
    vec3_normalize(forward);

    {
        float world_up[3] = {0.f, 1.f, 0.f};
        vec3_cross(forward, world_up, right);
    }
    if (vec3_len(right) <= 1e-6f)
        vec3_set(right, 1.f, 0.f, 0.f);
    vec3_normalize(right);

    return 1;
}
