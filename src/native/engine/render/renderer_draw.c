#include "renderer.h"
#include "renderer_internal.h"
#include "material.h"
#include "mesh.h"

#ifdef __EMSCRIPTEN__
#define BANANA_ENGINE_HAS_GL 1
#include "shader.h"
#include <GLES3/gl3.h>
#elif defined(BANANA_ENGINE_HAS_GLFW)
#define BANANA_ENGINE_HAS_GL 1
#include "shader.h"
#include <GL/gl.h>
#elif defined(BANANA_ENGINE_RENDER_BACKEND_DX12)
#include "backend_dx12.h"
#endif

void renderer_draw_command(Renderer *r, const RendererDrawCommand *command)
{
    if (!r || !command || !command->mesh)
        return;

#ifdef BANANA_ENGINE_HAS_GL
    shader_bind(r->default_shader);

    float vp[16], T[16], S[16], TS[16], mvp[16];
    camera_get_view_projection(&r->camera, vp);
    renderer_translation(command->position[0], command->position[1], command->position[2], T);
    renderer_scale(command->scale[0], command->scale[1], command->scale[2], S);
    renderer_mat4_mul(T, S, TS);
    renderer_mat4_mul(vp, TS, mvp);

    shader_set_mat4(r->default_shader, "u_mvp", mvp);
    shader_set_int(r->default_shader, "u_tile_texture", 0);
    glActiveTexture(GL_TEXTURE0);
    if (command->material.use_texture > 0.5f && r->default_tile_texture)
        glBindTexture(GL_TEXTURE_2D, r->default_tile_texture);
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    material_apply(&command->material, r->default_shader);
    mesh_draw(command->mesh);
#else
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12)
    banana_dx12_runtime_submit_scene_draw(command->position,
                                          command->scale,
                                          command->material.use_texture > 0.5f ? 1 : 0);
#else
    (void)command;
#endif
#endif
}

void renderer_draw_mesh(Renderer *r,
                        struct Mesh *mesh,
                        const float *position,
                        const float *rotation,
                        const float *scale,
                        const struct Material *material)
{
    RendererDrawCommand command;

    if (!mesh || !position || !rotation || !scale || !material)
        return;

    command.mesh = mesh;
    command.position[0] = position[0];
    command.position[1] = position[1];
    command.position[2] = position[2];
    command.rotation[0] = rotation[0];
    command.rotation[1] = rotation[1];
    command.rotation[2] = rotation[2];
    command.rotation[3] = rotation[3];
    command.scale[0] = scale[0];
    command.scale[1] = scale[1];
    command.scale[2] = scale[2];
    command.material = *material;

    renderer_draw_command(r, &command);
}
