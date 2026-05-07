#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include "mesh.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* M_PI fallback for platforms that don't define it */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Unified GL detection */
#ifdef __EMSCRIPTEN__
#  define BANANA_ENGINE_HAS_GL 1
#  include <GLES3/gl3.h>
#elif defined(BANANA_ENGINE_HAS_GLFW)
#  define BANANA_ENGINE_HAS_GL 1
#  include <GL/gl.h>
#endif

#ifdef BANANA_ENGINE_HAS_GL

struct Mesh {
    unsigned int vao, vbo, ebo;
    int index_count;
};

Mesh *mesh_create(const float *vertices, int vertex_count,
                  const unsigned int *indices, int index_count) {
    Mesh *m = malloc(sizeof(Mesh));
    m->index_count = index_count;

    glGenVertexArrays(1, &m->vao);
    glGenBuffers(1, &m->vbo);
    glGenBuffers(1, &m->ebo);

    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * 8 * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    /* layout 0: position (xyz), layout 1: normal (xyz), layout 2: uv (uv) */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, 0, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
    return m;
}

void mesh_draw(Mesh *m) {
    glBindVertexArray(m->vao);
    glDrawElements(GL_TRIANGLES, m->index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void mesh_destroy(Mesh *m) {
    if (!m) return;
    glDeleteVertexArrays(1, &m->vao);
    glDeleteBuffers(1, &m->vbo);
    glDeleteBuffers(1, &m->ebo);
    free(m);
}

#else /* headless stub — test builds */

struct Mesh { int index_count; };
Mesh *mesh_create(const float *v, int vc, const unsigned int *i, int ic) {
    (void)v; (void)vc; (void)i;
    Mesh *m = malloc(sizeof(Mesh));
    m->index_count = ic;
    return m;
}
void mesh_draw(Mesh *m) { (void)m; }
void mesh_destroy(Mesh *m) { free(m); }

#endif

/* ── Primitive generators ─────────────────────────────────────────────────── */

Mesh *mesh_create_cube(void) {
    /* 24 unique vertices (4 per face × 6 faces) with normals + UVs */
    static const float verts[] = {
        /* front (+Z)  pos              normal        uv   */
        -0.5f,-0.5f, 0.5f,   0,0,1,   0,0,
         0.5f,-0.5f, 0.5f,   0,0,1,   1,0,
         0.5f, 0.5f, 0.5f,   0,0,1,   1,1,
        -0.5f, 0.5f, 0.5f,   0,0,1,   0,1,
        /* back (-Z) */
         0.5f,-0.5f,-0.5f,   0,0,-1,  0,0,
        -0.5f,-0.5f,-0.5f,   0,0,-1,  1,0,
        -0.5f, 0.5f,-0.5f,   0,0,-1,  1,1,
         0.5f, 0.5f,-0.5f,   0,0,-1,  0,1,
        /* left (-X) */
        -0.5f,-0.5f,-0.5f,  -1,0,0,   0,0,
        -0.5f,-0.5f, 0.5f,  -1,0,0,   1,0,
        -0.5f, 0.5f, 0.5f,  -1,0,0,   1,1,
        -0.5f, 0.5f,-0.5f,  -1,0,0,   0,1,
        /* right (+X) */
         0.5f,-0.5f, 0.5f,   1,0,0,   0,0,
         0.5f,-0.5f,-0.5f,   1,0,0,   1,0,
         0.5f, 0.5f,-0.5f,   1,0,0,   1,1,
         0.5f, 0.5f, 0.5f,   1,0,0,   0,1,
        /* top (+Y) */
        -0.5f, 0.5f, 0.5f,   0,1,0,   0,0,
         0.5f, 0.5f, 0.5f,   0,1,0,   1,0,
         0.5f, 0.5f,-0.5f,   0,1,0,   1,1,
        -0.5f, 0.5f,-0.5f,   0,1,0,   0,1,
        /* bottom (-Y) */
        -0.5f,-0.5f,-0.5f,   0,-1,0,  0,0,
         0.5f,-0.5f,-0.5f,   0,-1,0,  1,0,
         0.5f,-0.5f, 0.5f,   0,-1,0,  1,1,
        -0.5f,-0.5f, 0.5f,   0,-1,0,  0,1,
    };
    static const unsigned int idx[] = {
         0, 1, 2,  2, 3, 0,    /* front  */
         4, 5, 6,  6, 7, 4,    /* back   */
         8, 9,10, 10,11, 8,    /* left   */
        12,13,14, 14,15,12,    /* right  */
        16,17,18, 18,19,16,    /* top    */
        20,21,22, 22,23,20,    /* bottom */
    };
    return mesh_create(verts, 24, idx, 36);
}

Mesh *mesh_create_sphere(int lat, int lon) {
    int vc = (lat + 1) * (lon + 1);
    int ic = lat * lon * 6;
    float *verts = malloc(vc * 8 * sizeof(float));
    unsigned int *idx = malloc(ic * sizeof(unsigned int));
    int vi = 0;
    for (int i = 0; i <= lat; i++) {
        float theta = (float)i / lat * (float)M_PI;
        for (int j = 0; j <= lon; j++) {
            float phi = (float)j / lon * 2.f * (float)M_PI;
            float nx = sinf(theta) * cosf(phi);
            float ny = cosf(theta);
            float nz = sinf(theta) * sinf(phi);
            verts[vi++] = nx * 0.5f; verts[vi++] = ny * 0.5f; verts[vi++] = nz * 0.5f;
            verts[vi++] = nx; verts[vi++] = ny; verts[vi++] = nz;
            verts[vi++] = (float)j / lon; verts[vi++] = (float)i / lat;
        }
    }
    int ii = 0;
    for (int i = 0; i < lat; i++) {
        for (int j = 0; j < lon; j++) {
            int a = i*(lon+1)+j, b=a+lon+1;
            idx[ii++]=a; idx[ii++]=b; idx[ii++]=a+1;
            idx[ii++]=b; idx[ii++]=b+1; idx[ii++]=a+1;
        }
    }
    Mesh *m = mesh_create(verts, vc, idx, ic);
    free(verts); free(idx);
    return m;
}

Mesh *mesh_create_plane(float w, float d) {
    float hw = w * 0.5f, hd = d * 0.5f;
    float verts[] = {
        -hw,0,-hd, 0,1,0, 0,0,
         hw,0,-hd, 0,1,0, 1,0,
         hw,0, hd, 0,1,0, 1,1,
        -hw,0, hd, 0,1,0, 0,1,
    };
    unsigned int idx[] = { 0,1,2, 2,3,0 };
    return mesh_create(verts, 4, idx, 6);
}
