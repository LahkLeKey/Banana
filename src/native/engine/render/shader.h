#ifndef BANANA_ENGINE_SHADER_H
#define BANANA_ENGINE_SHADER_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct Shader Shader;

    /* Compile a vertex + fragment shader pair from GLSL source strings.
     * Returns NULL on compile/link failure (error logged to stderr). */
    Shader *shader_create(const char *vert_src, const char *frag_src);

    /* Bind this shader program for subsequent draw calls. */
    void shader_bind(Shader *s);

    /* Set uniform values. Shader must be bound first. */
    void shader_set_float(Shader *s, const char *name, float value);
    void shader_set_vec3(Shader *s, const char *name, float x, float y, float z);
    void shader_set_mat4(Shader *s, const char *name, const float *m16);

    void shader_destroy(Shader *s);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_SHADER_H */
