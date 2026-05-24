#include "mesh.h"

#include <stdlib.h>
#include <string.h>

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
    float *vertices;
    int vertex_count;
    unsigned int *indices;
    int index_count;
};

Mesh *mesh_create(const float *vertices, int vertex_count, const unsigned int *indices,
                  int index_count)
{
    Mesh *m = (Mesh *)malloc(sizeof(Mesh));
    if (!m)
        return NULL;

    m->vertices = NULL;
    m->indices = NULL;
    m->vertex_count = vertex_count;

    m->index_count = index_count;

    if (vertex_count > 0 && vertices)
    {
        size_t vertex_bytes = (size_t)vertex_count * 8u * sizeof(float);
        m->vertices = (float *)malloc(vertex_bytes);
        if (!m->vertices)
        {
            free(m);
            return NULL;
        }
        memcpy(m->vertices, vertices, vertex_bytes);
    }

    if (index_count > 0 && indices)
    {
        size_t index_bytes = (size_t)index_count * sizeof(unsigned int);
        m->indices = (unsigned int *)malloc(index_bytes);
        if (!m->indices)
        {
            free(m->vertices);
            free(m);
            return NULL;
        }
        memcpy(m->indices, indices, index_bytes);
    }

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
    free(m->vertices);
    free(m->indices);
    free(m);
}

#else

struct Mesh
{
    float *vertices;
    int vertex_count;
    unsigned int *indices;
    int index_count;
};

Mesh *mesh_create(const float *v, int vc, const unsigned int *i, int ic)
{
    Mesh *m = (Mesh *)malloc(sizeof(Mesh));
    if (!m)
        return NULL;

    m->vertices = NULL;
    m->vertex_count = vc;
    m->indices = NULL;
    m->index_count = ic;

    if (vc > 0 && v)
    {
        size_t vertex_bytes = (size_t)vc * 8u * sizeof(float);
        m->vertices = (float *)malloc(vertex_bytes);
        if (!m->vertices)
        {
            free(m);
            return NULL;
        }
        memcpy(m->vertices, v, vertex_bytes);
    }

    if (ic > 0 && i)
    {
        size_t index_bytes = (size_t)ic * sizeof(unsigned int);
        m->indices = (unsigned int *)malloc(index_bytes);
        if (!m->indices)
        {
            free(m->vertices);
            free(m);
            return NULL;
        }
        memcpy(m->indices, i, index_bytes);
    }

    return m;
}

void mesh_draw(Mesh *m)
{
    (void)m;
}

void mesh_destroy(Mesh *m)
{
    if (!m)
        return;

    free(m->vertices);
    free(m->indices);
    free(m);
}

#endif

int mesh_get_vertex_count(const Mesh *m)
{
    return m ? m->vertex_count : 0;
}

const float *mesh_get_vertices(const Mesh *m)
{
    return m ? m->vertices : NULL;
}

int mesh_get_index_count(const Mesh *m)
{
    return m ? m->index_count : 0;
}

const unsigned int *mesh_get_indices(const Mesh *m)
{
    return m ? m->indices : NULL;
}
