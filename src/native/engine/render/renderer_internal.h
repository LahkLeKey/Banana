#ifndef BANANA_ENGINE_RENDERER_INTERNAL_H
#define BANANA_ENGINE_RENDERER_INTERNAL_H

#include "renderer.h"

struct Shader;
typedef struct Shader Shader;

struct Renderer
{
    int width;
    int height;
    int dx12_runtime_active;
    Shader *default_shader;
    Camera camera;
    unsigned char *frame_buffer;
#if defined(BANANA_ENGINE_HAS_GL) || defined(BANANA_ENGINE_HAS_GLFW)
    unsigned int fbo;
    unsigned int fbo_texture;
    unsigned int fbo_depth;
    int use_fbo;
    unsigned int default_tile_texture;
#endif
};

void renderer_mat4_mul(const float *A, const float *B, float *C);
void renderer_translation(float x, float y, float z, float *m16);
void renderer_scale(float x, float y, float z, float *m16);

#endif
