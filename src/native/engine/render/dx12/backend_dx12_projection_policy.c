#include "backend_dx12_projection_policy.h"

#include <math.h>

static float banana_dx12_projection_clampf(float value, float min_value, float max_value)
{
    if (value < min_value)
        return min_value;
    if (value > max_value)
        return max_value;
    return value;
}

static float banana_dx12_projection_vec3_dot(float ax,
                                             float ay,
                                             float az,
                                             float bx,
                                             float by,
                                             float bz)
{
    return (ax * bx) + (ay * by) + (az * bz);
}

static void banana_dx12_projection_vec3_cross(float ax,
                                               float ay,
                                               float az,
                                               float bx,
                                               float by,
                                               float bz,
                                               float *out_x,
                                               float *out_y,
                                               float *out_z)
{
    *out_x = (ay * bz) - (az * by);
    *out_y = (az * bx) - (ax * bz);
    *out_z = (ax * by) - (ay * bx);
}

static void banana_dx12_projection_vec3_normalize(float *x, float *y, float *z)
{
    float length = sqrtf((*x * *x) + (*y * *y) + (*z * *z));
    if (length <= 0.00001f)
    {
        *x = 0.0f;
        *y = 1.0f;
        *z = 0.0f;
        return;
    }

    *x /= length;
    *y /= length;
    *z /= length;
}

void banana_dx12_projection_world_to_clip(const BananaDx12ProjectionCamera *camera,
                                          float wx,
                                          float wy,
                                          float wz,
                                          float *out_x,
                                          float *out_y,
                                          float *out_z,
                                          float *out_view_depth)
{
    float camera_x = 16.0f;
    float camera_y = 14.0f;
    float camera_z = -16.0f;
    float target_x = 0.0f;
    float target_y = 0.0f;
    float target_z = 0.0f;
    float world_up_x = 0.0f;
    float world_up_y = 1.0f;
    float world_up_z = 0.0f;
    float tan_half_fov = 0.404026f; /* tan(44deg/2) default */
    float aspect = 1.0f;
    float forward_x = 0.0f;
    float forward_y = 0.0f;
    float forward_z = 0.0f;
    float right_x = 0.0f;
    float right_y = 0.0f;
    float right_z = 0.0f;
    float up_x = 0.0f;
    float up_y = 1.0f;
    float up_z = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;
    float view_x = 0.0f;
    float view_y = 0.0f;
    float view_z = 0.0f;
    float clip_x = 0.0f;
    float clip_y = 0.0f;

    if (camera && camera->valid)
    {
        camera_x = camera->eye[0];
        camera_y = camera->eye[1];
        camera_z = camera->eye[2];
        target_x = camera->target[0];
        target_y = camera->target[1];
        target_z = camera->target[2];
        world_up_x = camera->up[0];
        world_up_y = camera->up[1];
        world_up_z = camera->up[2];

        if (camera->fov_degrees > 1.0f && camera->fov_degrees < 179.0f)
        {
            float half_rad = (camera->fov_degrees * 0.5f) *
                             (3.14159265358979323846f / 180.0f);
            tan_half_fov = tanf(half_rad);
            if (tan_half_fov < 0.05f)
                tan_half_fov = 0.05f;
            if (tan_half_fov > 8.0f)
                tan_half_fov = 8.0f;
        }

        aspect = camera->aspect;
        if (aspect < 0.25f)
            aspect = 0.25f;
        if (aspect > 4.0f)
            aspect = 4.0f;
    }

    forward_x = target_x - camera_x;
    forward_y = target_y - camera_y;
    forward_z = target_z - camera_z;
    banana_dx12_projection_vec3_normalize(&forward_x, &forward_y, &forward_z);

    banana_dx12_projection_vec3_cross(forward_x,
                                      forward_y,
                                      forward_z,
                                      world_up_x,
                                      world_up_y,
                                      world_up_z,
                                      &right_x,
                                      &right_y,
                                      &right_z);
    banana_dx12_projection_vec3_normalize(&right_x, &right_y, &right_z);

    banana_dx12_projection_vec3_cross(right_x,
                                      right_y,
                                      right_z,
                                      forward_x,
                                      forward_y,
                                      forward_z,
                                      &up_x,
                                      &up_y,
                                      &up_z);
    banana_dx12_projection_vec3_normalize(&up_x, &up_y, &up_z);

    dx = wx - camera_x;
    dy = wy - camera_y;
    dz = wz - camera_z;

    view_x = banana_dx12_projection_vec3_dot(dx, dy, dz, right_x, right_y, right_z);
    view_y = banana_dx12_projection_vec3_dot(dx, dy, dz, up_x, up_y, up_z);
    view_z = banana_dx12_projection_vec3_dot(dx, dy, dz, forward_x, forward_y, forward_z);
    if (view_z < 0.01f)
        view_z = 0.01f;

    clip_x = (view_x / (view_z * tan_half_fov * aspect));
    clip_y = (view_y / (view_z * tan_half_fov));

    if (out_x)
        *out_x = banana_dx12_projection_clampf(clip_x, -1.25f, 1.25f);
    if (out_y)
        *out_y = banana_dx12_projection_clampf(clip_y, -1.25f, 1.25f);
    if (out_z)
        *out_z = banana_dx12_projection_clampf(0.02f + (view_z * 0.0045f), 0.02f, 0.98f);
    if (out_view_depth)
        *out_view_depth = view_z;
}
