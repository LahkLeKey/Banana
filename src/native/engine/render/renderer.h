#ifndef BANANA_ENGINE_RENDERER_H
#define BANANA_ENGINE_RENDERER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "camera.h"
#include "material.h"

    typedef struct Renderer Renderer;

    /* Forward declaration */
    struct Mesh;

    typedef struct RendererDrawCommand
    {
        struct Mesh *mesh;
        float position[3];
        float rotation[4];
        float scale[3];
        struct Material material;
    } RendererDrawCommand;

    /* Initialize the renderer.  Must be called after a Window+GL context exists. */
    Renderer *renderer_create(int width, int height);

    /* Begin a new frame: clear color + depth buffers. */
    void renderer_begin_frame(Renderer *r);

    /* Submit a mesh for rendering.
     * position: float[3]
     * rotation: float[4] quaternion (x, y, z, w)
     * scale:    float[3] */
    void renderer_draw_mesh(Renderer *r, struct Mesh *mesh, const float *position,
                            const float *rotation, const float *scale,
                            const struct Material *material);

    /* Submit a typed render command. This is the backend-agnostic draw seam. */
    void renderer_draw_command(Renderer *r, const RendererDrawCommand *command);

    /* Set the active camera. */
    void renderer_set_camera(Renderer *r, const Camera *camera);

    /* Finish frame: resolve the render pass. */
    void renderer_end_frame(Renderer *r);

    /* Resize internal buffers (call on window resize). */
    void renderer_resize(Renderer *r, int width, int height);

    /* Attach the native window handle used by backend-specific runtime paths. */
    void renderer_attach_native_window(Renderer *r, void *native_window);

    void renderer_destroy(Renderer *r);

    /* ── C ABI exports (called from game loop) ───────────────────────────────── */
    void engine_render_frame(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_RENDERER_H */
