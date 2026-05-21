#include "mesh.h"

#include <stdlib.h>

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
    Mesh *m = (Mesh *)malloc(sizeof(Mesh));
    if (!m)
        return NULL;

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

#else

struct Mesh
{
    int index_count;
};

Mesh *mesh_create(const float *v, int vc, const unsigned int *i, int ic)
{
    (void)v;
    (void)vc;
    (void)i;

    Mesh *m = (Mesh *)malloc(sizeof(Mesh));
    if (!m)
        return NULL;

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
