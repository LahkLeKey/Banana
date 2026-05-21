#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include "mesh.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* M_PI fallback for platforms that don't define it */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Unified GL detection */
#ifdef __EMSCRIPTEN__
#define BANANA_ENGINE_HAS_GL 1
#include <GLES3/gl3.h>
#elif defined(BANANA_ENGINE_HAS_GLFW)
#define BANANA_ENGINE_HAS_GL 1
#include <GL/gl.h>
#endif

#ifdef BANANA_ENGINE_HAS_GL

struct Mesh
{
    unsigned int vao, vbo, ebo;
    int index_count;
};

Mesh *mesh_create(const float *vertices, int vertex_count, const unsigned int *indices,
                  int index_count)
{
    Mesh *m = malloc(sizeof(Mesh));
    m->index_count = index_count;

    glGenVertexArrays(1, &m->vao);
    glGenBuffers(1, &m->vbo);
    glGenBuffers(1, &m->ebo);

    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * 8 * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices,
                 GL_STATIC_DRAW);

    /* layout 0: position (xyz), layout 1: normal (xyz), layout 2: uv (uv) */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, 0, 8 * sizeof(float), (void *)(6 * sizeof(float)));

    glBindVertexArray(0);
    return m;
}

void mesh_draw(Mesh *m)
{
    glBindVertexArray(m->vao);
    glDrawElements(GL_TRIANGLES, m->index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void mesh_destroy(Mesh *m)
{
    if (!m)
        return;
    glDeleteVertexArrays(1, &m->vao);
    glDeleteBuffers(1, &m->vbo);
    glDeleteBuffers(1, &m->ebo);
    free(m);
}

#else /* headless stub — test builds */

struct Mesh
{
    int index_count;
};
Mesh *mesh_create(const float *v, int vc, const unsigned int *i, int ic)
{
    (void)v;
    (void)vc;
    (void)i;
    Mesh *m = malloc(sizeof(Mesh));
    m->index_count = ic;
    return m;
}
void mesh_draw(Mesh *m)
{
    (void)m;
}
void mesh_destroy(Mesh *m)
{
    free(m);
}

#endif

/* ── Banana-derived primitive generators ─────────────────────────────────── */

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
    /* Mobile-first default profile. */
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
    /* Low curvature and low taper gives a banana-derived block-like volume. */
    return mesh_create_banana_vector(1.25f, 0.70f, 0.15f, 0.05f, quality);
}

Mesh *mesh_create_banana_orb_like(int quality)
{
    /* Symmetric, compact profile gives a banana-derived orb-like volume. */
    return mesh_create_banana_vector(1.35f, 0.60f, 0.55f, 0.20f, quality);
}

Mesh *mesh_create_terrain_heightfield(const unsigned char *heights, int width, int depth,
                                      float tile_size, float height_scale)
{
    if (!heights || width < 2 || depth < 2 || tile_size <= 0.0f)
        return NULL;

    const int vertex_count = width * depth;
    const int index_count = (width - 1) * (depth - 1) * 6;

    float *verts = (float *)malloc((size_t)vertex_count * 8 * sizeof(float));
    unsigned int *indices = (unsigned int *)malloc((size_t)index_count * sizeof(unsigned int));
    if (!verts || !indices)
    {
        free(verts);
        free(indices);
        return NULL;
    }

    const float x_origin = ((float)(width - 1) * tile_size) * 0.5f;
    const float z_origin = ((float)(depth - 1) * tile_size) * 0.5f;

    int vi = 0;
    for (int z = 0; z < depth; z++)
    {
        for (int x = 0; x < width; x++)
        {
            const int idx_center = z * width + x;
            const int xl = x > 0 ? x - 1 : x;
            const int xr = x + 1 < width ? x + 1 : x;
            const int zd = z > 0 ? z - 1 : z;
            const int zu = z + 1 < depth ? z + 1 : z;

            const float h = (float)heights[idx_center] * height_scale;
            const float hl = (float)heights[z * width + xl] * height_scale;
            const float hr = (float)heights[z * width + xr] * height_scale;
            const float hd = (float)heights[zd * width + x] * height_scale;
            const float hu = (float)heights[zu * width + x] * height_scale;

            float nx = hl - hr;
            float ny = tile_size * 2.0f;
            float nz = hd - hu;
            float len = sqrtf(nx * nx + ny * ny + nz * nz);
            if (len < 1e-5f)
                len = 1.0f;
            nx /= len;
            ny /= len;
            nz /= len;

            verts[vi++] = (float)x * tile_size - x_origin;
            verts[vi++] = h;
            verts[vi++] = (float)z * tile_size - z_origin;
            verts[vi++] = nx;
            verts[vi++] = ny;
            verts[vi++] = nz;
            verts[vi++] = (float)x / (float)(width - 1);
            verts[vi++] = (float)z / (float)(depth - 1);
        }
    }

    int ii = 0;
    for (int z = 0; z < depth - 1; z++)
    {
        for (int x = 0; x < width - 1; x++)
        {
            unsigned int a = (unsigned int)(z * width + x);
            unsigned int b = a + 1u;
            unsigned int c = (unsigned int)((z + 1) * width + x);
            unsigned int d = c + 1u;

            indices[ii++] = a;
            indices[ii++] = c;
            indices[ii++] = b;
            indices[ii++] = b;
            indices[ii++] = c;
            indices[ii++] = d;
        }
    }

    Mesh *m = mesh_create(verts, vertex_count, indices, index_count);
    free(verts);
    free(indices);
    return m;
}
