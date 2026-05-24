#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include "mesh.h"

#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float clampf(float value, float lo, float hi)
{
    if (value < lo)
        return lo;
    if (value > hi)
        return hi;
    return value;
}

static Mesh *mesh_create_banana_profile(int arc_segments, int ring_segments, float radius_scale,
                                        float length_scale, float curve_scale, float tip_taper)
{
    radius_scale = clampf(radius_scale, 0.15f, 4.0f);
    length_scale = clampf(length_scale, 0.20f, 4.0f);
    curve_scale = clampf(curve_scale, 0.05f, 3.0f);
    tip_taper = clampf(tip_taper, 0.0f, 1.0f);

    const int vertex_count = (arc_segments + 1) * (ring_segments + 1);
    const int index_count = arc_segments * ring_segments * 6;

    float *verts = (float *)malloc((size_t)vertex_count * 8 * sizeof(float));
    unsigned int *idx = (unsigned int *)malloc((size_t)index_count * sizeof(unsigned int));
    if (!verts || !idx)
    {
        free(verts);
        free(idx);
        return NULL;
    }

    int vi = 0;
    for (int i = 0; i <= arc_segments; i++)
    {
        float t = (float)i / (float)arc_segments;
        float angle = (-1.05f + 2.10f * t) * curve_scale;

        float center_x = sinf(angle) * (2.0f * length_scale);
        float center_y = ((1.0f - cosf(angle)) * 0.9f - 0.9f) * length_scale;
        float center_z = 0.0f;

        float tangent_x = cosf(angle) * (2.0f * length_scale);
        float tangent_y = sinf(angle) * (0.9f * length_scale);
        float tangent_z = 0.0f;
        float tangent_len = sqrtf(tangent_x * tangent_x + tangent_y * tangent_y + tangent_z * tangent_z);
        if (tangent_len < 1e-5f)
            tangent_len = 1.0f;
        tangent_x /= tangent_len;
        tangent_y /= tangent_len;
        tangent_z /= tangent_len;

        float binormal_x = tangent_y;
        float binormal_y = -tangent_x;
        float binormal_z = 0.0f;

        float normal_x = 0.0f;
        float normal_y = 0.0f;
        float normal_z = 1.0f;

        float center_weight = 1.0f - fabsf(2.0f * t - 1.0f);
        float taper_strength = 1.0f - tip_taper * (1.0f - center_weight);
        float thickness = (0.18f + 0.30f * center_weight) * taper_strength * radius_scale;

        for (int j = 0; j <= ring_segments; j++)
        {
            float u = (float)j / (float)ring_segments;
            float phi = u * 2.0f * (float)M_PI;
            float cos_phi = cosf(phi);
            float sin_phi = sinf(phi);

            float local_radius_n = thickness * 0.60f;
            float local_radius_b = thickness;

            float offset_x = normal_x * cos_phi * local_radius_n + binormal_x * sin_phi * local_radius_b;
            float offset_y = normal_y * cos_phi * local_radius_n + binormal_y * sin_phi * local_radius_b;
            float offset_z = normal_z * cos_phi * local_radius_n + binormal_z * sin_phi * local_radius_b;

            float nx = offset_x;
            float ny = offset_y;
            float nz = offset_z;
            float n_len = sqrtf(nx * nx + ny * ny + nz * nz);
            if (n_len < 1e-5f)
                n_len = 1.0f;
            nx /= n_len;
            ny /= n_len;
            nz /= n_len;

            verts[vi++] = center_x + offset_x;
            verts[vi++] = center_y + offset_y;
            verts[vi++] = center_z + offset_z;
            verts[vi++] = nx;
            verts[vi++] = ny;
            verts[vi++] = nz;
            verts[vi++] = t;
            verts[vi++] = u;
        }
    }

    int ii = 0;
    for (int i = 0; i < arc_segments; i++)
    {
        for (int j = 0; j < ring_segments; j++)
        {
            unsigned int a = (unsigned int)(i * (ring_segments + 1) + j);
            unsigned int b = (unsigned int)((i + 1) * (ring_segments + 1) + j);
            unsigned int c = a + 1u;
            unsigned int d = b + 1u;

            idx[ii++] = a;
            idx[ii++] = b;
            idx[ii++] = c;
            idx[ii++] = b;
            idx[ii++] = d;
            idx[ii++] = c;
        }
    }

    Mesh *m = mesh_create(verts, vertex_count, idx, index_count);
    free(verts);
    free(idx);
    return m;
}

Mesh *mesh_create_banana(void)
{
    return mesh_create_banana_profile(12, 8, 1.0f, 1.0f, 1.0f, 0.35f);
}

Mesh *mesh_create_banana_lod(int quality)
{
    if (quality <= 0)
        return mesh_create_banana_profile(8, 6, 1.0f, 1.0f, 1.0f, 0.35f);
    if (quality == 1)
        return mesh_create_banana_profile(12, 8, 1.0f, 1.0f, 1.0f, 0.35f);
    if (quality == 2)
        return mesh_create_banana_profile(18, 12, 1.0f, 1.0f, 1.0f, 0.35f);
    return mesh_create_banana_profile(24, 16, 1.0f, 1.0f, 1.0f, 0.35f);
}

Mesh *mesh_create_banana_vector(float radius_scale, float length_scale, float curve_scale,
                                float tip_taper, int quality)
{
    int q = quality;
    if (q < 0)
        q = 0;
    if (q > 3)
        q = 3;

    int arc_segments = 8;
    int ring_segments = 6;
    if (q == 1)
    {
        arc_segments = 12;
        ring_segments = 8;
    }
    else if (q == 2)
    {
        arc_segments = 18;
        ring_segments = 12;
    }
    else if (q == 3)
    {
        arc_segments = 24;
        ring_segments = 16;
    }

    return mesh_create_banana_profile(arc_segments, ring_segments, radius_scale, length_scale,
                                      curve_scale, tip_taper);
}

Mesh *mesh_create_banana_block_like(int quality)
{
    return mesh_create_banana_vector(1.25f, 0.70f, 0.15f, 0.05f, quality);
}

Mesh *mesh_create_banana_orb_like(int quality)
{
    return mesh_create_banana_vector(1.35f, 0.60f, 0.55f, 0.20f, quality);
}

Mesh *mesh_create_peanut_butter_pickup_asset(int quality)
{
    int q = quality;
    if (q < 1)
        q = 1;
    if (q > 3)
        q = 3;

    /* Short, thick jar-like banana derivative so pickup reads distinctly in-scene. */
    return mesh_create_banana_vector(1.55f, 0.58f, 0.12f, 0.02f, q);
}
