#include "renderer.h"
#include "renderer_internal.h"
#include "backend_dx12.h"
#include "material.h"
#include "mesh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── GL platform detection ───────────────────────────────────────────────── */
#if defined(BANANA_ENGINE_HAS_GLFW)
#define BANANA_ENGINE_HAS_GL 1
#include <GL/gl.h>
#include "shader.h"

static const char *DEFAULT_VERT = "#version 330 core\n"
                                  "layout(location=0) in vec3 a_pos;\n"
                                  "layout(location=1) in vec3 a_normal;\n"
                                  "layout(location=2) in vec2 a_uv;\n"
                                  "uniform mat4 u_mvp;\n"
                                  "out vec3 v_normal;\n"
                                  "out vec2 v_uv;\n"
                                  "void main() {\n"
                                  "  v_normal = a_normal;\n"
                                  "  v_uv = a_uv;\n"
                                  "  gl_Position = u_mvp * vec4(a_pos, 1.0);\n"
                                  "}\n";

static const char *DEFAULT_FRAG =
    "#version 330 core\n"
    "in vec3 v_normal;\n"
    "in vec2 v_uv;\n"
    "uniform vec3 u_color;\n"
    "uniform float u_use_texture;\n"
    "uniform float u_uv_scale;\n"
    "uniform sampler2D u_tile_texture;\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "  vec3 base_color = u_color;\n"
    "  if (u_use_texture > 0.5) {\n"
    "    base_color *= texture(u_tile_texture, v_uv * u_uv_scale).rgb;\n"
    "  }\n"
    "  float diff = max(dot(normalize(v_normal), vec3(0.5,1,0.5)), 0.15);\n"
    "  frag_color = vec4(base_color * diff, 1.0);\n"
    "}\n";
#endif /* platform */

#ifdef BANANA_ENGINE_HAS_GL
/* ── Internal mat4 multiply (column-major, A × B → C) ───────────────────── */
void renderer_mat4_mul(const float *A, const float *B, float *C)
{
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
        {
            float s = 0.f;
            for (int k = 0; k < 4; k++)
                s += A[k * 4 + row] * B[col * 4 + k];
            C[col * 4 + row] = s;
        }
}

/* ── Translation matrix (column-major) ──────────────────────────────────── */
void renderer_translation(float x, float y, float z, float *m16)
{
    memset(m16, 0, 64);
    m16[0] = m16[5] = m16[10] = m16[15] = 1.f;
    m16[12] = x;
    m16[13] = y;
    m16[14] = z;
}

void renderer_scale(float x, float y, float z, float *m16)
{
    memset(m16, 0, 64);
    m16[0] = x;
    m16[5] = y;
    m16[10] = z;
    m16[15] = 1.f;
}
#endif

#ifdef BANANA_ENGINE_HAS_GL
static unsigned int renderer_create_fallback_tile_texture(void)
{
    unsigned int texture_id = 0;
    const unsigned char pixel[4] = {255u, 255u, 255u, 255u};

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture_id;
}
#endif


#ifdef BANANA_ENGINE_HAS_GL

static void setup_fbo(Renderer *r)
{
    glGenFramebuffers(1, &r->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, r->fbo);

    glGenTextures(1, &r->fbo_texture);
    glBindTexture(GL_TEXTURE_2D, r->fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, r->width, r->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, r->fbo_texture, 0);

    glGenRenderbuffers(1, &r->fbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, r->fbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, r->width, r->height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, r->fbo_depth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "[engine/renderer] FBO incomplete\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
#endif

Renderer *renderer_create(int width, int height)
{
    Renderer *r = calloc(1, sizeof(Renderer));
    r->width = width;
    r->height = height;
    r->frame_buffer = malloc((size_t)width * height * 4);
    r->camera = camera_create(60.f, (float)width / height, 0.1f, 1000.f);
    camera_look_at(&r->camera, 12, 18, 12, 0, 0, 0);

#ifdef BANANA_ENGINE_HAS_GL
    r->default_shader = shader_create(DEFAULT_VERT, DEFAULT_FRAG);
    r->default_tile_texture = renderer_create_fallback_tile_texture();
    /* Native: render to FBO so we can readback pixels */
    r->use_fbo = 1;
    setup_fbo(r);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
#else
    r->default_shader = NULL;
#endif
    return r;
}

void renderer_attach_native_window(Renderer *r, void *native_window)
{
#ifdef BANANA_ENGINE_RENDER_BACKEND_DX12
    if (!r)
        return;
    r->dx12_runtime_active = banana_dx12_runtime_init(native_window, r->width, r->height);
    if (!r->dx12_runtime_active)
    {
        fprintf(stderr,
                "[engine/renderer] dx12 runtime attach failed native_window=%p telemetry=%s\n",
                native_window,
                banana_dx12_runtime_telemetry());
    }
    else
    {
        fprintf(stdout,
                "[engine/renderer] dx12 runtime attach ok native_window=%p telemetry=%s\n",
                native_window,
                banana_dx12_runtime_telemetry());
    }
#else
    (void)r;
    (void)native_window;
#endif
}

void renderer_set_camera(Renderer *r, const Camera *cam)
{
    if (!r || !cam)
        return;

    r->camera = *cam;

#ifdef BANANA_ENGINE_RENDER_BACKEND_DX12
    if (r->dx12_runtime_active)
    {
        banana_dx12_runtime_set_camera(r->camera.position,
                                       r->camera.target,
                                       r->camera.up,
                                       r->camera.fov_degrees,
                                       r->camera.aspect);
    }
#endif
}

void renderer_begin_frame(Renderer *r)
{
#ifdef BANANA_ENGINE_HAS_GL
    glBindFramebuffer(GL_FRAMEBUFFER, r->use_fbo ? r->fbo : 0);
    glViewport(0, 0, r->width, r->height);
    glClearColor(0.12f, 0.24f, 0.42f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
#ifdef BANANA_ENGINE_RENDER_BACKEND_DX12
    if (r->dx12_runtime_active)
    {
        if (!banana_dx12_runtime_begin_frame(0.12f, 0.24f, 0.42f, 1.f))
        {
            r->dx12_runtime_active = 0;
        }
    }
#endif
#endif
}

void renderer_end_frame(Renderer *r)
{
#ifdef BANANA_ENGINE_HAS_GL
    if (r->use_fbo)
    {
        /* Native: readback pixels into frame_buffer, then blit to default FBO */
        glReadPixels(0, 0, r->width, r->height, GL_RGBA, GL_UNSIGNED_BYTE, r->frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
#else
/* DX12 runtime is optional in this non-GL build path; headless readback remains
 * available for tests and ABI consumers. */
#ifdef BANANA_ENGINE_RENDER_BACKEND_DX12
    if (r->dx12_runtime_active)
    {
        if (!banana_dx12_runtime_end_frame())
        {
            r->dx12_runtime_active = 0;
        }
    }
#endif
    /* Headless stub: fill with identifiable gradient for test assertions */
    int pixel_count = r->width * r->height;
    int i = 0;
#if defined(BANANA_ENGINE_HAS_OPENMP)
#pragma omp parallel for schedule(static)
#endif
    for (i = 0; i < pixel_count; i++)
    {
        r->frame_buffer[i * 4 + 0] = (unsigned char)(i % 256);
        r->frame_buffer[i * 4 + 1] = 0x42;
        r->frame_buffer[i * 4 + 2] = 0x00;
        r->frame_buffer[i * 4 + 3] = 0xFF;
    }
#endif
}

const unsigned char *renderer_get_frame_buffer(Renderer *r)
{
    return r->frame_buffer;
}

void renderer_resize(Renderer *r, int width, int height)
{
    r->width = width;
    r->height = height;
    r->camera.aspect = (float)width / height;
    r->frame_buffer = realloc(r->frame_buffer, (size_t)width * height * 4);
#ifdef BANANA_ENGINE_HAS_GL
    glViewport(0, 0, width, height);
#elif defined(BANANA_ENGINE_RENDER_BACKEND_DX12)
    if (r->dx12_runtime_active)
    {
        if (!banana_dx12_runtime_resize(width, height))
        {
            r->dx12_runtime_active = 0;
        }
    }
#endif
}

void renderer_destroy(Renderer *r)
{
    if (!r)
        return;
#ifdef BANANA_ENGINE_HAS_GL
    shader_destroy(r->default_shader);
#endif
    free(r->frame_buffer);
#ifdef BANANA_ENGINE_HAS_GLFW
    glDeleteFramebuffers(1, &r->fbo);
    glDeleteTextures(1, &r->fbo_texture);
    glDeleteRenderbuffers(1, &r->fbo_depth);
#endif
#ifdef BANANA_ENGINE_HAS_GL
    if (r->default_tile_texture)
        glDeleteTextures(1, &r->default_tile_texture);
#endif
#ifdef BANANA_ENGINE_RENDER_BACKEND_DX12
    if (r->dx12_runtime_active)
        banana_dx12_runtime_shutdown();
#endif
    free(r);
}

/* ── C ABI stubs — wired to global singleton in engine.c ────────────────── */
/* These are implemented in engine.c where the singleton lives.              */
