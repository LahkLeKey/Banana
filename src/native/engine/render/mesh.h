#ifndef BANANA_ENGINE_MESH_H
#define BANANA_ENGINE_MESH_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Mesh Mesh;

/* Create a mesh from raw vertex and index data.
 * vertices: interleaved [x, y, z, nx, ny, nz, u, v] per vertex.
 * indices:  triangle index list. */
Mesh *mesh_create(const float *vertices, int vertex_count,
                  const unsigned int *indices, int index_count);

/* Built-in primitive generators. */
Mesh *mesh_create_cube(void);
Mesh *mesh_create_sphere(int lat_bands, int lon_bands);
Mesh *mesh_create_plane(float width, float depth);

/* Issue a draw call (VAO bind + glDrawElements). Shader must be bound. */
void mesh_draw(Mesh *m);

void mesh_destroy(Mesh *m);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_MESH_H */
