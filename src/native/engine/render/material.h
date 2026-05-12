#ifndef BANANA_ENGINE_MATERIAL_H
#define BANANA_ENGINE_MATERIAL_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* Forward declaration */
    struct Shader;

    typedef struct Material
    {
        float color[4];  /* RGBA, 0.0–1.0 */
        float roughness; /* 0.0 = smooth, 1.0 = rough */
        float metallic;  /* 0.0 = dielectric, 1.0 = metal */
        float use_texture;
        float uv_scale;
    } Material;

    /* Solid color material. */
    Material material_solid(float r, float g, float b, float a);

    /* Apply material uniforms to the currently bound shader. */
    void material_apply(const Material *m, struct Shader *s);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_MATERIAL_H */
