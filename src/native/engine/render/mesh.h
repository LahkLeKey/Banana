#ifndef BANANA_ENGINE_MESH_H
#define BANANA_ENGINE_MESH_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct Mesh Mesh;

    /* Create a mesh from raw vertex and index data.
     * vertices: interleaved [x, y, z, nx, ny, nz, u, v] per vertex.
     * indices:  triangle index list. */
    Mesh *mesh_create(const float *vertices, int vertex_count, const unsigned int *indices,
                      int index_count);

    /* Banana-derived primitive generators.
     * No classical primitives are exposed from the engine mesh layer. */
    Mesh *mesh_create_banana(void);
    Mesh *mesh_create_banana_lod(int quality);

    /* Banana vector primitive: derive custom meshes from baseline banana shape.
     * radius_scale:  overall thickness multiplier (> 0)
     * length_scale:  axial length multiplier (> 0)
     * curve_scale:   curvature multiplier (> 0)
     * tip_taper:     0..1, higher = thinner tips
     * quality:       0..3, higher = more segments */
    Mesh *mesh_create_banana_vector(float radius_scale, float length_scale, float curve_scale,
                                    float tip_taper, int quality);

    /* Convenience adapters: these preserve banana-derived generation while
     * approximating familiar volume families for gameplay composition. */
    Mesh *mesh_create_banana_block_like(int quality);
    Mesh *mesh_create_banana_orb_like(int quality);

    /* Build a terrain mesh from a deterministic height map (WFC/CA-friendly).
     * heights: row-major [depth][width], each cell is an unsigned elevation value.
     * tile_size: horizontal spacing between cells.
     * height_scale: multiplier converting elevation values to world-space height. */
    Mesh *mesh_create_terrain_heightfield(const unsigned char *heights, int width, int depth,
                                          float tile_size, float height_scale);

    /* Issue a draw call (VAO bind + glDrawElements). Shader must be bound. */
    void mesh_draw(Mesh *m);

    void mesh_destroy(Mesh *m);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_MESH_H */
