#include "renderer.h"
#include "mesh.h"
#include "shader.h"
#include "material.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ── GL platform detection ───────────────────────────────────────────────── */
#ifdef __EMSCRIPTEN__
#  define BANANA_ENGINE_HAS_GL 1
#  include <GLES3/gl3.h>

static const char *DEFAULT_VERT =
    "#version 300 es\n"
    "in vec3 a_pos;\n"
    "in vec3 a_normal;\n"
    "in vec2 a_uv;\n"
    "uniform mat4 u_mvp;\n"
    "out vec3 v_normal;\n"
    "void main() {\n"
    "  v_normal = a_normal;\n"
    "  gl_Position = u_mvp * vec4(a_pos, 1.0);\n"
    "}\n";

static const char *DEFAULT_FRAG =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec3 v_normal;\n"
    "uniform vec3 u_color;\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "  float diff = max(dot(normalize(v_normal), vec3(0.5,1.0,0.5)), 0.15);\n"
    "  frag_color = vec4(u_color * diff, 1.0);\n"
    "}\n";

#elif defined(BANANA_ENGINE_HAS_GLFW)
#  define BANANA_ENGINE_HAS_GL 1
#  include <GL/gl.h>

static const char *DEFAULT_VERT =
    "#version 330 core\n"
    "layout(location=0) in vec3 a_pos;\n"
    "layout(location=1) in vec3 a_normal;\n"
    "layout(location=2) in vec2 a_uv;\n"
    "uniform mat4 u_mvp;\n"
    "out vec3 v_normal;\n"
    "void main() {\n"
    "  v_normal = a_normal;\n"
    "  gl_Position = u_mvp * vec4(a_pos, 1.0);\n"
    "}\n";

static const char *DEFAULT_FRAG =
    "#version 330 core\n"
    "in vec3 v_normal;\n"
    "uniform vec3 u_color;\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "  float diff = max(dot(normalize(v_normal), vec3(0.5,1,0.5)), 0.15);\n"
    "  frag_color = vec4(u_color * diff, 1.0);\n"
    "}\n";
#endif /* platform */

/* ── Internal mat4 multiply (column-major, A × B → C) ───────────────────── */
static void r_mat4_mul(const float *A, const float *B, float *C) {
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++) {
            float s = 0.f;
            for (int k = 0; k < 4; k++)
                s += A[k * 4 + row] * B[col * 4 + k];
            C[col * 4 + row] = s;
        }
}

/* ── Translation matrix (column-major) ──────────────────────────────────── */
static void r_translation(float x, float y, float z, float *m16) {
    memset(m16, 0, 64);
    m16[0] = m16[5] = m16[10] = m16[15] = 1.f;
    m16[12] = x; m16[13] = y; m16[14] = z;
}

struct Renderer {
    int width;
    int height;
    Shader  *default_shader;
    Camera   camera;
    unsigned char *frame_buffer; /* RGBA pixels, width*height*4 */
#ifdef BANANA_ENGINE_HAS_GL
    /* FBO used in native mode for pixel readback; unused in WASM (render to canvas) */
    unsigned int fbo;
    unsigned int fbo_texture;
    unsigned int fbo_depth;
    int          use_fbo; /* 1 = native readback, 0 = direct to canvas (WASM) */
#endif
};

#ifdef BANANA_ENGINE_HAS_GL

static void setup_fbo(Renderer *r) {
    glGenFramebuffers(1,  &r->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, r->fbo);

    glGenTextures(1, &r->fbo_texture);
    glBindTexture(GL_TEXTURE_2D, r->fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, r->width, r->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, r->fbo_texture, 0);

    glGenRenderbuffers(1, &r->fbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, r->fbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, r->width, r->height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, r->fbo_depth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "[engine/renderer] FBO incomplete\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
#endif

Renderer *renderer_create(int width, int height) {
    Renderer *r = calloc(1, sizeof(Renderer));
    r->width  = width;
    r->height = height;
    r->frame_buffer = malloc((size_t)width * height * 4);
    r->camera = camera_create(60.f, (float)width / height, 0.1f, 1000.f);
    camera_look_at(&r->camera, 0, 5, 10, 0, 0, 0);

#ifdef BANANA_ENGINE_HAS_GL
    r->default_shader = shader_create(DEFAULT_VERT, DEFAULT_FRAG);
#  ifdef __EMSCRIPTEN__
    /* WASM: render direct to canvas (FBO 0); no readback needed */
    r->use_fbo = 0;
#  else
    /* Native: render to FBO so we can readback pixels */
    r->use_fbo = 1;
    setup_fbo(r);
#  endif
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
#else
    r->default_shader = shader_create(NULL, NULL);
#endif
    return r;
}

void renderer_set_camera(Renderer *r, const Camera *cam) {
    r->camera = *cam;
}

void renderer_begin_frame(Renderer *r) {
#ifdef BANANA_ENGINE_HAS_GL
    glBindFramebuffer(GL_FRAMEBUFFER, r->use_fbo ? r->fbo : 0);
    glClearColor(0.08f, 0.08f, 0.12f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
    (void)r;
#endif
}

void renderer_draw_mesh(Renderer *r, struct Mesh *mesh,
                         const float *position, const float *rotation,
                         const float *scale, const struct Material *material) {
    (void)rotation; /* quaternion → mat4: Phase 2 */
    (void)scale;    /* non-uniform scale: Phase 2 */
#ifdef BANANA_ENGINE_HAS_GL
    shader_bind(r->default_shader);

    /* MVP = ViewProjection × Translation(position) */
    float vp[16], T[16], mvp[16];
    camera_get_view_projection(&r->camera, vp);
    r_translation(position[0], position[1], position[2], T);
    r_mat4_mul(vp, T, mvp);

    shader_set_mat4(r->default_shader, "u_mvp", mvp);
    material_apply(material, r->default_shader);
    mesh_draw(mesh);
#else
    (void)r; (void)mesh; (void)position; (void)material;
#endif
}

void renderer_end_frame(Renderer *r) {
#ifdef BANANA_ENGINE_HAS_GL
    if (r->use_fbo) {
        /* Native: readback pixels into frame_buffer, then blit to default FBO */
        glReadPixels(0, 0, r->width, r->height,
                     GL_RGBA, GL_UNSIGNED_BYTE, r->frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    /* WASM: canvas already has the rendered output; nothing to do */
#else
    /* Headless stub: fill with identifiable gradient for test assertions */
    for (int i = 0; i < r->width * r->height; i++) {
        r->frame_buffer[i*4+0] = (unsigned char)(i % 256);
        r->frame_buffer[i*4+1] = 0x42;
        r->frame_buffer[i*4+2] = 0x00;
        r->frame_buffer[i*4+3] = 0xFF;
    }
#endif
}

const unsigned char *renderer_get_frame_buffer(Renderer *r) {
    return r->frame_buffer;
}

void renderer_resize(Renderer *r, int width, int height) {
    r->width  = width;
    r->height = height;
    r->camera.aspect = (float)width / height;
    r->frame_buffer  = realloc(r->frame_buffer, (size_t)width * height * 4);
#ifdef BANANA_ENGINE_HAS_GLFW
    glViewport(0, 0, width, height);
#endif
}

void renderer_destroy(Renderer *r) {
    if (!r) return;
    shader_destroy(r->default_shader);
    free(r->frame_buffer);
#ifdef BANANA_ENGINE_HAS_GLFW
    glDeleteFramebuffers(1, &r->fbo);
    glDeleteTextures(1, &r->fbo_texture);
    glDeleteRenderbuffers(1, &r->fbo_depth);
#endif
    free(r);
}

/* ── WASM ABI stubs — wired to global singleton in engine.c ─────────────── */
/* These are implemented in engine.c where the singleton lives.              */
