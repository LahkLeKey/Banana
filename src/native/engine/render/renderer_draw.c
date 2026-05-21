#include "renderer.h"
#include "renderer_internal.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"

#ifdef __EMSCRIPTEN__
#define BANANA_ENGINE_HAS_GL 1
#include <GLES3/gl3.h>
#elif defined(BANANA_ENGINE_HAS_GLFW)
#define BANANA_ENGINE_HAS_GL 1
#include <GL/gl.h>
#endif

void renderer_draw_mesh(Renderer *r, struct Mesh *mesh, const float *position,
                        const float *rotation, const float *scale, const struct Material *material)
{
    (void)rotation;
    (void)scale;
#ifdef BANANA_ENGINE_HAS_GL
    shader_bind(r->default_shader);

    float vp[16], T[16], S[16], TS[16], mvp[16];
    camera_get_view_projection(&r->camera, vp);
    renderer_translation(position[0], position[1], position[2], T);
    renderer_scale(scale[0], scale[1], scale[2], S);
    renderer_mat4_mul(T, S, TS);
    renderer_mat4_mul(vp, TS, mvp);

    shader_set_mat4(r->default_shader, "u_mvp", mvp);
    shader_set_int(r->default_shader, "u_tile_texture", 0);
    glActiveTexture(GL_TEXTURE0);
    if (material->use_texture > 0.5f && r->default_tile_texture)
        glBindTexture(GL_TEXTURE_2D, r->default_tile_texture);
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    material_apply(material, r->default_shader);
    mesh_draw(mesh);
#else
    (void)r;
    (void)mesh;
    (void)position;
    (void)material;
#endif
}
