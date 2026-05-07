#ifndef BANANA_ENGINE_RENDERER_H
#define BANANA_ENGINE_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "camera.h"
#include "material.h"

typedef struct Renderer Renderer;

/* Forward declaration */
struct Mesh;

/* Initialize the renderer.  Must be called after a Window+GL context exists. */
Renderer *renderer_create(int width, int height);

/* Begin a new frame: clear color + depth buffers. */
void renderer_begin_frame(Renderer *r);

/* Submit a mesh for rendering.
 * position: float[3]
 * rotation: float[4] quaternion (x, y, z, w)
 * scale:    float[3] */
void renderer_draw_mesh(Renderer *r, struct Mesh *mesh,
                        const float *position, const float *rotation,
                        const float *scale, const struct Material *material);

/* Set the active camera. */
void renderer_set_camera(Renderer *r, const Camera *camera);

/* Finish frame: resolve to frame buffer, optionally read pixels back. */
void renderer_end_frame(Renderer *r);

/* Return pointer to the RGBA pixel data of the last completed frame.
 * Buffer size = width * height * 4 bytes.
 * Pointer is valid until the next renderer_end_frame call. */
const unsigned char *renderer_get_frame_buffer(Renderer *r);

/* Resize internal buffers (call on window resize). */
void renderer_resize(Renderer *r, int width, int height);

void renderer_destroy(Renderer *r);

/* ── WASM ABI exports (called from game loop) ─────────────────────────────── */
void engine_render_frame(void);
const unsigned char *engine_get_frame_buffer(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_RENDERER_H */
