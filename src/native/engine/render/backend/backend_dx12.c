#include "backend_dx12.h"
#include "dx12/backend_dx12_diagnostics.h"
#include "dx12/backend_dx12_projection_policy.h"

#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
#define COBJMACROS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stddef.h>
#include <windows.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>

#define BANANA_DX12_BACK_BUFFER_COUNT 2
#define BANANA_DX12_DEBUG_MAX_VERTICES 262144

typedef struct BananaDx12DebugVertex
{
    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
    float a;
} BananaDx12DebugVertex;

static const char *BANANA_DX12_PROBE_WINDOW_CLASS = "BananaDx12ProbeWindow";

static LRESULT CALLBACK banana_dx12_probe_window_proc(HWND hwnd,
                                                      UINT message,
                                                      WPARAM w_param,
                                                      LPARAM l_param)
{
    return DefWindowProcA(hwnd, message, w_param, l_param);
}

static int banana_dx12_create_probe_window(HINSTANCE instance,
                                           WNDCLASSA *window_class,
                                           HWND *window)
{
    ATOM class_atom = 0;
    HWND probe_window = NULL;

    ZeroMemory(window_class, sizeof(*window_class));
    window_class->style = CS_OWNDC;
    window_class->lpfnWndProc = banana_dx12_probe_window_proc;
    window_class->hInstance = instance;
    window_class->lpszClassName = BANANA_DX12_PROBE_WINDOW_CLASS;

    class_atom = RegisterClassA(window_class);
    if (class_atom == 0)
    {
        return 0;
    }

    probe_window = CreateWindowExA(0,
                                   BANANA_DX12_PROBE_WINDOW_CLASS,
                                   "Banana DX12 Probe",
                                   WS_OVERLAPPEDWINDOW,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   16,
                                   16,
                                   NULL,
                                   NULL,
                                   instance,
                                   NULL);
    if (!probe_window)
    {
        UnregisterClassA(BANANA_DX12_PROBE_WINDOW_CLASS, instance);
        return 0;
    }

    *window = probe_window;
    return 1;
}

typedef struct BananaDx12Runtime
{
    int active;
    int width;
    int height;
    const char *status;
    int owns_window;
    HWND window;
    IDXGIFactory4 *factory;
    IDXGIAdapter1 *adapter;
    ID3D12Device *device;
    ID3D12CommandQueue *command_queue;
    IDXGISwapChain3 *swap_chain;
    ID3D12DescriptorHeap *rtv_heap;
    ID3D12DescriptorHeap *dsv_heap;
    ID3D12Resource *render_targets[BANANA_DX12_BACK_BUFFER_COUNT];
    ID3D12Resource *depth_stencil;
    ID3D12CommandAllocator *command_allocator;
    ID3D12GraphicsCommandList *command_list;
    ID3D12Fence *fence;
    HANDLE fence_event;
    UINT rtv_descriptor_size;
    UINT frame_index;
    UINT64 fence_value;
    UINT present_interval;
    UINT64 frame_counter;
    UINT64 frames_presented;
    UINT64 scene_draw_calls_total;
    UINT scene_draw_calls_frame;
    HRESULT last_present_result;
    int command_list_recording;
    ID3D12RootSignature *scene_root_signature;
    ID3D12PipelineState *scene_pipeline_state;
    ID3D12Resource *scene_vertex_buffer;
    D3D12_VERTEX_BUFFER_VIEW scene_vertex_buffer_view;
    BananaDx12DebugVertex *scene_vertex_data;
    UINT scene_vertex_count;
    UINT scene_vertex_capacity;
    int scene_pipeline_ready;
    unsigned char *ui_overlay_rgba;
    size_t ui_overlay_capacity;
    int ui_overlay_width;
    int ui_overlay_height;
    int ui_overlay_dirty;
    unsigned int ui_overlay_quads_last;
    unsigned int ui_overlay_rows_last;
    unsigned int ui_overlay_vertices_last;
    float camera_eye[3];
    float camera_target[3];
    float camera_up[3];
    float camera_fov_degrees;
    float camera_aspect;
    int camera_valid;
} BananaDx12Runtime;

static BananaDx12Runtime s_dx12_runtime = {0};

static BananaDx12DiagnosticsSnapshot banana_dx12_runtime_diagnostics_snapshot(void)
{
    BananaDx12DiagnosticsSnapshot snapshot;

    snapshot.status = s_dx12_runtime.status;
    snapshot.active = s_dx12_runtime.active;
    snapshot.width = s_dx12_runtime.width;
    snapshot.height = s_dx12_runtime.height;
    snapshot.frame_counter = (unsigned long long)s_dx12_runtime.frame_counter;
    snapshot.frames_presented = (unsigned long long)s_dx12_runtime.frames_presented;
    snapshot.scene_draw_calls_frame = s_dx12_runtime.scene_draw_calls_frame;
    snapshot.scene_draw_calls_total = (unsigned long long)s_dx12_runtime.scene_draw_calls_total;
    snapshot.present_interval = s_dx12_runtime.present_interval;
    snapshot.last_present_result = (unsigned long)s_dx12_runtime.last_present_result;
    snapshot.ui_overlay_width = s_dx12_runtime.ui_overlay_width;
    snapshot.ui_overlay_height = s_dx12_runtime.ui_overlay_height;
    snapshot.ui_overlay_dirty = s_dx12_runtime.ui_overlay_dirty;
    snapshot.ui_overlay_rows_last = s_dx12_runtime.ui_overlay_rows_last;
    snapshot.ui_overlay_quads_last = s_dx12_runtime.ui_overlay_quads_last;
    snapshot.ui_overlay_vertices_last = s_dx12_runtime.ui_overlay_vertices_last;

    return snapshot;
}

static int banana_dx12_runtime_read_present_interval(void)
{
    const char *present_interval_env = getenv("BANANA_DX12_PRESENT_INTERVAL");
    const char *vsync_env = getenv("BANANA_DX12_VSYNC");

    if (present_interval_env && present_interval_env[0])
    {
        long value = strtol(present_interval_env, NULL, 10);
        return value <= 0 ? 0 : 1;
    }
    if (vsync_env && vsync_env[0])
    {
        long value = strtol(vsync_env, NULL, 10);
        return value == 0 ? 0 : 1;
    }

    return 1;
}

static void banana_dx12_runtime_update_telemetry(void)
{
    BananaDx12DiagnosticsSnapshot snapshot = banana_dx12_runtime_diagnostics_snapshot();
    (void)banana_dx12_diagnostics_runtime_telemetry(&snapshot);
}

static void banana_dx12_runtime_release_depth_target(void)
{
    if (s_dx12_runtime.depth_stencil)
    {
        ID3D12Resource_Release(s_dx12_runtime.depth_stencil);
        s_dx12_runtime.depth_stencil = NULL;
    }
}

static float banana_dx12_clampf(float value, float min_value, float max_value)
{
    if (value < min_value)
    {
        return min_value;
    }
    if (value > max_value)
    {
        return max_value;
    }
    return value;
}

static float banana_dx12_vec3_dot(float ax, float ay, float az,
                                  float bx, float by, float bz)
{
    return (ax * bx) + (ay * by) + (az * bz);
}

static void banana_dx12_vec3_cross(float ax, float ay, float az,
                                   float bx, float by, float bz,
                                   float *out_x, float *out_y, float *out_z)
{
    *out_x = (ay * bz) - (az * by);
    *out_y = (az * bx) - (ax * bz);
    *out_z = (ax * by) - (ay * bx);
}

static void banana_dx12_vec3_normalize(float *x, float *y, float *z)
{
    float length = sqrtf((*x * *x) + (*y * *y) + (*z * *z));
    if (length <= 0.00001f)
    {
        *x = 0.0f;
        *y = 1.0f;
        *z = 0.0f;
        return;
    }

    *x /= length;
    *y /= length;
    *z /= length;
}

static void banana_dx12_project_world_to_clip(float wx,
                                               float wy,
                                               float wz,
                                               float *out_x,
                                               float *out_y,
                                               float *out_z,
                                               float *out_view_depth)
{
    BananaDx12ProjectionCamera camera = {0};

    camera.eye[0] = s_dx12_runtime.camera_eye[0];
    camera.eye[1] = s_dx12_runtime.camera_eye[1];
    camera.eye[2] = s_dx12_runtime.camera_eye[2];
    camera.target[0] = s_dx12_runtime.camera_target[0];
    camera.target[1] = s_dx12_runtime.camera_target[1];
    camera.target[2] = s_dx12_runtime.camera_target[2];
    camera.up[0] = s_dx12_runtime.camera_up[0];
    camera.up[1] = s_dx12_runtime.camera_up[1];
    camera.up[2] = s_dx12_runtime.camera_up[2];
    camera.fov_degrees = s_dx12_runtime.camera_fov_degrees;
    camera.aspect = s_dx12_runtime.camera_aspect;
    camera.valid = s_dx12_runtime.camera_valid;

    banana_dx12_projection_world_to_clip(&camera,
                                         wx,
                                         wy,
                                         wz,
                                         out_x,
                                         out_y,
                                         out_z,
                                         out_view_depth);
}

static void banana_dx12_runtime_release_scene_pipeline(void)
{
    if (s_dx12_runtime.scene_vertex_buffer)
    {
        ID3D12Resource_Unmap(s_dx12_runtime.scene_vertex_buffer, 0, NULL);
        ID3D12Resource_Release(s_dx12_runtime.scene_vertex_buffer);
        s_dx12_runtime.scene_vertex_buffer = NULL;
    }
    s_dx12_runtime.scene_vertex_data = NULL;
    s_dx12_runtime.scene_vertex_count = 0;
    s_dx12_runtime.scene_vertex_capacity = 0;

    if (s_dx12_runtime.scene_pipeline_state)
    {
        ID3D12PipelineState_Release(s_dx12_runtime.scene_pipeline_state);
        s_dx12_runtime.scene_pipeline_state = NULL;
    }
    if (s_dx12_runtime.scene_root_signature)
    {
        ID3D12RootSignature_Release(s_dx12_runtime.scene_root_signature);
        s_dx12_runtime.scene_root_signature = NULL;
    }

    ZeroMemory(&s_dx12_runtime.scene_vertex_buffer_view,
               sizeof(s_dx12_runtime.scene_vertex_buffer_view));
    s_dx12_runtime.scene_pipeline_ready = 0;
}

static int banana_dx12_runtime_push_quad(float center_x,
                                         float center_y,
                                         float half_width,
                                         float half_height,
                                         float z,
                                         float r,
                                         float g,
                                         float b,
                                         float a)
{
    BananaDx12DebugVertex *v = NULL;

    if (!s_dx12_runtime.scene_vertex_data)
    {
        return 0;
    }
    if ((s_dx12_runtime.scene_vertex_count + 6u) > s_dx12_runtime.scene_vertex_capacity)
    {
        return 0;
    }

    v = &s_dx12_runtime.scene_vertex_data[s_dx12_runtime.scene_vertex_count];

    v[0].x = center_x - half_width;
    v[0].y = center_y - half_height;
    v[0].z = z;
    v[0].r = r;
    v[0].g = g;
    v[0].b = b;
    v[0].a = a;

    v[1].x = center_x - half_width;
    v[1].y = center_y + half_height;
    v[1].z = z;
    v[1].r = r;
    v[1].g = g;
    v[1].b = b;
    v[1].a = a;

    v[2].x = center_x + half_width;
    v[2].y = center_y + half_height;
    v[2].z = z;
    v[2].r = r;
    v[2].g = g;
    v[2].b = b;
    v[2].a = a;

    v[3] = v[0];
    v[4] = v[2];

    v[5].x = center_x + half_width;
    v[5].y = center_y - half_height;
    v[5].z = z;
    v[5].r = r;
    v[5].g = g;
    v[5].b = b;
    v[5].a = a;

    s_dx12_runtime.scene_vertex_count += 6u;
    return 1;
}

static int banana_dx12_runtime_push_triangle(float x0,
                                             float y0,
                                             float z0,
                                             float x1,
                                             float y1,
                                             float z1,
                                             float x2,
                                             float y2,
                                             float z2,
                                             float r,
                                             float g,
                                             float b,
                                             float a)
{
    BananaDx12DebugVertex *v = NULL;
    if (!s_dx12_runtime.scene_vertex_data)
    {
        return 0;
    }
    if ((s_dx12_runtime.scene_vertex_count + 3u) > s_dx12_runtime.scene_vertex_capacity)
    {
        return 0;
    }

    v = &s_dx12_runtime.scene_vertex_data[s_dx12_runtime.scene_vertex_count];
    v[0].x = x0;
    v[0].y = y0;
    v[0].z = z0;
    v[0].r = r;
    v[0].g = g;
    v[0].b = b;
    v[0].a = a;

    v[1].x = x1;
    v[1].y = y1;
    v[1].z = z1;
    v[1].r = r;
    v[1].g = g;
    v[1].b = b;
    v[1].a = a;

    v[2].x = x2;
    v[2].y = y2;
    v[2].z = z2;
    v[2].r = r;
    v[2].g = g;
    v[2].b = b;
    v[2].a = a;

    s_dx12_runtime.scene_vertex_count += 3u;
    return 1;
}

static int banana_dx12_runtime_ensure_ui_overlay_capacity(size_t required)
{
    unsigned char *resized = NULL;

    if (required == 0)
        return 1;

    if (s_dx12_runtime.ui_overlay_capacity >= required && s_dx12_runtime.ui_overlay_rgba)
        return 1;

    resized = (unsigned char *)realloc(s_dx12_runtime.ui_overlay_rgba, required);
    if (!resized)
        return 0;

    s_dx12_runtime.ui_overlay_rgba = resized;
    s_dx12_runtime.ui_overlay_capacity = required;
    return 1;
}

static void banana_dx12_runtime_append_ui_overlay_vertices(void)
{
    int width = s_dx12_runtime.ui_overlay_width;
    int height = s_dx12_runtime.ui_overlay_height;
    int y = 0;
    unsigned int row_count = 0;
    unsigned int quad_count = 0;
    unsigned int vertices_before = s_dx12_runtime.scene_vertex_count;

    if (!s_dx12_runtime.ui_overlay_dirty ||
        !s_dx12_runtime.ui_overlay_rgba ||
        width <= 0 ||
        height <= 0)
    {
        s_dx12_runtime.ui_overlay_rows_last = 0;
        s_dx12_runtime.ui_overlay_quads_last = 0;
        s_dx12_runtime.ui_overlay_vertices_last = 0;
        return;
    }

    for (y = 0; y < height; y++)
    {
        int x = 0;
        int row_has_overlay = 0;
        while (x < width)
        {
            size_t pixel_index = ((size_t)y * (size_t)width + (size_t)x) * 4u;
            const unsigned char *p = &s_dx12_runtime.ui_overlay_rgba[pixel_index];
            unsigned char r = p[0];
            unsigned char g = p[1];
            unsigned char b = p[2];
            unsigned char a = p[3];
            int run_end = x + 1;
            float left = 0.0f;
            float right = 0.0f;
            float top = 0.0f;
            float bottom = 0.0f;
            float center_x = 0.0f;
            float center_y = 0.0f;
            float half_width = 0.0f;
            float half_height = 0.0f;

            if (a == 0)
            {
                x++;
                continue;
            }

            while (run_end < width)
            {
                size_t run_idx = ((size_t)y * (size_t)width + (size_t)run_end) * 4u;
                const unsigned char *rp = &s_dx12_runtime.ui_overlay_rgba[run_idx];
                if (rp[0] != r || rp[1] != g || rp[2] != b || rp[3] != a)
                    break;
                run_end++;
            }

            left = (((float)x / (float)width) * 2.0f) - 1.0f;
            right = (((float)run_end / (float)width) * 2.0f) - 1.0f;
            top = 1.0f - (((float)y / (float)height) * 2.0f);
            bottom = 1.0f - (((float)(y + 1) / (float)height) * 2.0f);

            center_x = (left + right) * 0.5f;
            center_y = (top + bottom) * 0.5f;
            half_width = (right - left) * 0.5f;
            half_height = (top - bottom) * 0.5f;

            if (!banana_dx12_runtime_push_quad(center_x,
                                               center_y,
                                               half_width,
                                               half_height,
                                               0.01f,
                                               (float)r / 255.0f,
                                               (float)g / 255.0f,
                                               (float)b / 255.0f,
                                               (float)a / 255.0f))
            {
                s_dx12_runtime.ui_overlay_rows_last = row_count;
                s_dx12_runtime.ui_overlay_quads_last = quad_count;
                s_dx12_runtime.ui_overlay_vertices_last = s_dx12_runtime.scene_vertex_count - vertices_before;
                s_dx12_runtime.ui_overlay_dirty = 0;
                return;
            }

            row_has_overlay = 1;
            quad_count++;

            run_end = run_end < (x + 1) ? (x + 1) : run_end;
            x = run_end;
        }

        if (row_has_overlay)
            row_count++;
    }

    s_dx12_runtime.ui_overlay_rows_last = row_count;
    s_dx12_runtime.ui_overlay_quads_last = quad_count;
    s_dx12_runtime.ui_overlay_vertices_last = s_dx12_runtime.scene_vertex_count - vertices_before;
    s_dx12_runtime.ui_overlay_dirty = 0;
}

static int banana_dx12_runtime_push_mesh(const Mesh *mesh,
                                         const float *position,
                                         const float *scale,
                                         int uses_texture,
                                         float color_r,
                                         float color_g,
                                         float color_b,
                                         float color_a)
{
    const float *vertices = NULL;
    const unsigned int *indices = NULL;
    int vertex_count = 0;
    int index_count = 0;
    int i = 0;
    float light_x = 0.38f;
    float light_y = 0.84f;
    float light_z = -0.39f;

    banana_dx12_vec3_normalize(&light_x, &light_y, &light_z);

    if (!mesh || !position || !scale)
    {
        return 0;
    }

    vertices = mesh_get_vertices(mesh);
    indices = mesh_get_indices(mesh);
    vertex_count = mesh_get_vertex_count(mesh);
    index_count = mesh_get_index_count(mesh);

    if (!vertices || !indices || vertex_count <= 0 || index_count < 3)
    {
        return 0;
    }

    for (i = 0; i + 2 < index_count; i += 3)
    {
        unsigned int i0 = indices[i + 0];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];
        const float *v0 = NULL;
        const float *v1 = NULL;
        const float *v2 = NULL;
        float wx0;
        float wy0;
        float wz0;
        float wx1;
        float wy1;
        float wz1;
        float wx2;
        float wy2;
        float wz2;
        float sx0;
        float sy0;
        float sz0;
        float sx1;
        float sy1;
        float sz1;
        float sx2;
        float sy2;
        float sz2;
        float depth0 = 0.0f;
        float depth1 = 0.0f;
        float depth2 = 0.0f;
        float nx = 0.0f;
        float ny = 1.0f;
        float nz = 0.0f;
        float ndotl = 0.55f;
        float avg_height = 0.0f;
        float avg_depth = 0.0f;
        float terrain_tint = 1.0f;
        float height_tint = 1.0f;
        float depth_fade = 1.0f;
        float shaded_r = color_r;
        float shaded_g = color_g;
        float shaded_b = color_b;

        if (i0 >= (unsigned int)vertex_count ||
            i1 >= (unsigned int)vertex_count ||
            i2 >= (unsigned int)vertex_count)
        {
            continue;
        }

        v0 = &vertices[i0 * 8u];
        v1 = &vertices[i1 * 8u];
        v2 = &vertices[i2 * 8u];

        wx0 = position[0] + (v0[0] * scale[0]);
        wy0 = position[1] + (v0[1] * scale[1]);
        wz0 = position[2] + (v0[2] * scale[2]);
        wx1 = position[0] + (v1[0] * scale[0]);
        wy1 = position[1] + (v1[1] * scale[1]);
        wz1 = position[2] + (v1[2] * scale[2]);
        wx2 = position[0] + (v2[0] * scale[0]);
        wy2 = position[1] + (v2[1] * scale[1]);
        wz2 = position[2] + (v2[2] * scale[2]);

        banana_dx12_project_world_to_clip(wx0, wy0, wz0, &sx0, &sy0, &sz0, &depth0);
        banana_dx12_project_world_to_clip(wx1, wy1, wz1, &sx1, &sy1, &sz1, &depth1);
        banana_dx12_project_world_to_clip(wx2, wy2, wz2, &sx2, &sy2, &sz2, &depth2);

        nx = (v0[3] + v1[3] + v2[3]) / 3.0f;
        ny = (v0[4] + v1[4] + v2[4]) / 3.0f;
        nz = (v0[5] + v1[5] + v2[5]) / 3.0f;
        banana_dx12_vec3_normalize(&nx, &ny, &nz);
        ndotl = banana_dx12_vec3_dot(nx, ny, nz, light_x, light_y, light_z);
        ndotl = banana_dx12_clampf((ndotl * 0.55f) + 0.45f, 0.25f, 1.0f);

        avg_height = (wy0 + wy1 + wy2) / 3.0f;
        avg_depth = (depth0 + depth1 + depth2) / 3.0f;
        height_tint = banana_dx12_clampf(0.82f + (avg_height * 0.06f), 0.55f, 1.18f);
        depth_fade = banana_dx12_clampf(1.10f - (avg_depth * 0.018f), 0.58f, 1.04f);

        if (uses_texture)
        {
            terrain_tint = banana_dx12_clampf(0.80f + ((ny + 1.0f) * 0.10f), 0.72f, 1.05f);
            shaded_r = color_r * 0.72f * terrain_tint;
            shaded_g = color_g * 1.05f * terrain_tint;
            shaded_b = color_b * 0.72f * terrain_tint;
        }
        else
        {
            terrain_tint = banana_dx12_clampf(0.95f + (ny * 0.08f), 0.80f, 1.12f);
            shaded_r = color_r * terrain_tint;
            shaded_g = color_g * terrain_tint;
            shaded_b = color_b * terrain_tint;
        }

        shaded_r = banana_dx12_clampf(shaded_r * ndotl * height_tint * depth_fade, 0.08f, 1.0f);
        shaded_g = banana_dx12_clampf(shaded_g * ndotl * height_tint * depth_fade, 0.08f, 1.0f);
        shaded_b = banana_dx12_clampf(shaded_b * ndotl * height_tint * depth_fade, 0.08f, 1.0f);

        if (!banana_dx12_runtime_push_triangle(sx0,
                                               sy0,
                                               sz0,
                                               sx1,
                                               sy1,
                                               sz1,
                                               sx2,
                                               sy2,
                                               sz2,
                                               shaded_r,
                                               shaded_g,
                                               shaded_b,
                                               color_a))
        {
            break;
        }
    }

    return 1;
}

static int banana_dx12_runtime_create_scene_pipeline(void)
{
    static const char *vertex_shader_source =
        "struct VSInput { float3 pos : POSITION; float4 col : COLOR; };\n"
        "struct PSInput { float4 pos : SV_POSITION; float4 col : COLOR; };\n"
        "PSInput main(VSInput input) {\n"
        "  PSInput output;\n"
        "  output.pos = float4(input.pos, 1.0);\n"
        "  output.col = input.col;\n"
        "  return output;\n"
        "}\n";
    static const char *pixel_shader_source =
        "struct PSInput { float4 pos : SV_POSITION; float4 col : COLOR; };\n"
        "float4 main(PSInput input) : SV_TARGET {\n"
        "  return input.col;\n"
        "}\n";

    ID3DBlob *vertex_shader_blob = NULL;
    ID3DBlob *pixel_shader_blob = NULL;
    ID3DBlob *root_signature_blob = NULL;
    ID3DBlob *error_blob = NULL;
    D3D12_ROOT_SIGNATURE_DESC root_signature_desc;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc;
    D3D12_INPUT_ELEMENT_DESC input_layout[2];
    D3D12_HEAP_PROPERTIES heap_props;
    D3D12_RESOURCE_DESC resource_desc;
    D3D12_RASTERIZER_DESC rasterizer_desc;
    D3D12_BLEND_DESC blend_desc;
    D3D12_DEPTH_STENCIL_DESC depth_stencil_desc;
    D3D12_SHADER_BYTECODE vs;
    D3D12_SHADER_BYTECODE ps;
    HRESULT result;

    if (!s_dx12_runtime.device)
    {
        return 0;
    }

    banana_dx12_runtime_release_scene_pipeline();

    result = D3DCompile(vertex_shader_source,
                        strlen(vertex_shader_source),
                        "banana_dx12_debug_vs",
                        NULL,
                        NULL,
                        "main",
                        "vs_5_0",
                        0,
                        0,
                        &vertex_shader_blob,
                        &error_blob);
    if (FAILED(result) || !vertex_shader_blob)
    {
        if (error_blob)
        {
            error_blob->lpVtbl->Release(error_blob);
        }
        return 0;
    }
    if (error_blob)
    {
        error_blob->lpVtbl->Release(error_blob);
        error_blob = NULL;
    }

    result = D3DCompile(pixel_shader_source,
                        strlen(pixel_shader_source),
                        "banana_dx12_debug_ps",
                        NULL,
                        NULL,
                        "main",
                        "ps_5_0",
                        0,
                        0,
                        &pixel_shader_blob,
                        &error_blob);
    if (FAILED(result) || !pixel_shader_blob)
    {
        if (error_blob)
        {
            error_blob->lpVtbl->Release(error_blob);
        }
        vertex_shader_blob->lpVtbl->Release(vertex_shader_blob);
        return 0;
    }
    if (error_blob)
    {
        error_blob->lpVtbl->Release(error_blob);
        error_blob = NULL;
    }

    ZeroMemory(&root_signature_desc, sizeof(root_signature_desc));
    root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    result = D3D12SerializeRootSignature(&root_signature_desc,
                                         D3D_ROOT_SIGNATURE_VERSION_1,
                                         &root_signature_blob,
                                         &error_blob);
    if (FAILED(result) || !root_signature_blob)
    {
        if (error_blob)
        {
            error_blob->lpVtbl->Release(error_blob);
        }
        vertex_shader_blob->lpVtbl->Release(vertex_shader_blob);
        pixel_shader_blob->lpVtbl->Release(pixel_shader_blob);
        return 0;
    }
    if (error_blob)
    {
        error_blob->lpVtbl->Release(error_blob);
        error_blob = NULL;
    }

    result = ID3D12Device_CreateRootSignature(s_dx12_runtime.device,
                                              0,
                                              root_signature_blob->lpVtbl->GetBufferPointer(root_signature_blob),
                                              root_signature_blob->lpVtbl->GetBufferSize(root_signature_blob),
                                              &IID_ID3D12RootSignature,
                                              (void **)&s_dx12_runtime.scene_root_signature);
    root_signature_blob->lpVtbl->Release(root_signature_blob);
    if (FAILED(result) || !s_dx12_runtime.scene_root_signature)
    {
        vertex_shader_blob->lpVtbl->Release(vertex_shader_blob);
        pixel_shader_blob->lpVtbl->Release(pixel_shader_blob);
        return 0;
    }

    ZeroMemory(input_layout, sizeof(input_layout));
    input_layout[0].SemanticName = "POSITION";
    input_layout[0].SemanticIndex = 0;
    input_layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    input_layout[0].InputSlot = 0;
    input_layout[0].AlignedByteOffset = 0;
    input_layout[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    input_layout[0].InstanceDataStepRate = 0;
    input_layout[1].SemanticName = "COLOR";
    input_layout[1].SemanticIndex = 0;
    input_layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    input_layout[1].InputSlot = 0;
    input_layout[1].AlignedByteOffset = 12;
    input_layout[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    input_layout[1].InstanceDataStepRate = 0;

    ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
    rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizer_desc.FrontCounterClockwise = FALSE;
    rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer_desc.DepthClipEnable = TRUE;
    rasterizer_desc.MultisampleEnable = FALSE;
    rasterizer_desc.AntialiasedLineEnable = FALSE;
    rasterizer_desc.ForcedSampleCount = 0;
    rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    ZeroMemory(&blend_desc, sizeof(blend_desc));
    blend_desc.AlphaToCoverageEnable = FALSE;
    blend_desc.IndependentBlendEnable = FALSE;
    blend_desc.RenderTarget[0].BlendEnable = TRUE;
    blend_desc.RenderTarget[0].LogicOpEnable = FALSE;
    blend_desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blend_desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
    depth_stencil_desc.DepthEnable = FALSE;
    depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    depth_stencil_desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    depth_stencil_desc.StencilEnable = FALSE;

    ZeroMemory(&pso_desc, sizeof(pso_desc));
    vs.pShaderBytecode = vertex_shader_blob->lpVtbl->GetBufferPointer(vertex_shader_blob);
    vs.BytecodeLength = vertex_shader_blob->lpVtbl->GetBufferSize(vertex_shader_blob);
    ps.pShaderBytecode = pixel_shader_blob->lpVtbl->GetBufferPointer(pixel_shader_blob);
    ps.BytecodeLength = pixel_shader_blob->lpVtbl->GetBufferSize(pixel_shader_blob);
    pso_desc.pRootSignature = s_dx12_runtime.scene_root_signature;
    pso_desc.VS = vs;
    pso_desc.PS = ps;
    pso_desc.BlendState = blend_desc;
    pso_desc.SampleMask = UINT_MAX;
    pso_desc.RasterizerState = rasterizer_desc;
    pso_desc.DepthStencilState = depth_stencil_desc;
    pso_desc.InputLayout.pInputElementDescs = input_layout;
    pso_desc.InputLayout.NumElements = 2;
    pso_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso_desc.NumRenderTargets = 1;
    pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso_desc.DSVFormat = DXGI_FORMAT_UNKNOWN;
    pso_desc.SampleDesc.Count = 1;
    pso_desc.SampleDesc.Quality = 0;

    result = ID3D12Device_CreateGraphicsPipelineState(s_dx12_runtime.device,
                                                      &pso_desc,
                                                      &IID_ID3D12PipelineState,
                                                      (void **)&s_dx12_runtime.scene_pipeline_state);
    vertex_shader_blob->lpVtbl->Release(vertex_shader_blob);
    pixel_shader_blob->lpVtbl->Release(pixel_shader_blob);
    if (FAILED(result) || !s_dx12_runtime.scene_pipeline_state)
    {
        banana_dx12_runtime_release_scene_pipeline();
        return 0;
    }

    ZeroMemory(&heap_props, sizeof(heap_props));
    heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_props.CreationNodeMask = 1;
    heap_props.VisibleNodeMask = 1;

    ZeroMemory(&resource_desc, sizeof(resource_desc));
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Alignment = 0;
    resource_desc.Width = (UINT64)(sizeof(BananaDx12DebugVertex) * BANANA_DX12_DEBUG_MAX_VERTICES);
    resource_desc.Height = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.MipLevels = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.SampleDesc.Quality = 0;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    result = ID3D12Device_CreateCommittedResource(s_dx12_runtime.device,
                                                  &heap_props,
                                                  D3D12_HEAP_FLAG_NONE,
                                                  &resource_desc,
                                                  D3D12_RESOURCE_STATE_GENERIC_READ,
                                                  NULL,
                                                  &IID_ID3D12Resource,
                                                  (void **)&s_dx12_runtime.scene_vertex_buffer);
    if (FAILED(result) || !s_dx12_runtime.scene_vertex_buffer)
    {
        banana_dx12_runtime_release_scene_pipeline();
        return 0;
    }

    result = ID3D12Resource_Map(s_dx12_runtime.scene_vertex_buffer,
                                0,
                                NULL,
                                (void **)&s_dx12_runtime.scene_vertex_data);
    if (FAILED(result) || !s_dx12_runtime.scene_vertex_data)
    {
        banana_dx12_runtime_release_scene_pipeline();
        return 0;
    }

    s_dx12_runtime.scene_vertex_buffer_view.BufferLocation =
        ID3D12Resource_GetGPUVirtualAddress(s_dx12_runtime.scene_vertex_buffer);
    s_dx12_runtime.scene_vertex_buffer_view.StrideInBytes = sizeof(BananaDx12DebugVertex);
    s_dx12_runtime.scene_vertex_buffer_view.SizeInBytes =
        (UINT)(sizeof(BananaDx12DebugVertex) * BANANA_DX12_DEBUG_MAX_VERTICES);
    s_dx12_runtime.scene_vertex_capacity = BANANA_DX12_DEBUG_MAX_VERTICES;
    s_dx12_runtime.scene_vertex_count = 0;
    s_dx12_runtime.scene_pipeline_ready = 1;
    return 1;
}

static void banana_dx12_runtime_release_swapchain_buffers(void)
{
    int i = 0;
    for (i = 0; i < BANANA_DX12_BACK_BUFFER_COUNT; i++)
    {
        if (s_dx12_runtime.render_targets[i])
        {
            ID3D12Resource_Release(s_dx12_runtime.render_targets[i]);
            s_dx12_runtime.render_targets[i] = NULL;
        }
    }
    banana_dx12_runtime_release_depth_target();
}

static void banana_dx12_runtime_wait_for_gpu(void)
{
    UINT64 fence_target = 0;
    if (!s_dx12_runtime.command_queue || !s_dx12_runtime.fence || !s_dx12_runtime.fence_event)
    {
        return;
    }

    s_dx12_runtime.fence_value++;
    fence_target = s_dx12_runtime.fence_value;
    if (FAILED(ID3D12CommandQueue_Signal(s_dx12_runtime.command_queue,
                                         (ID3D12Fence *)s_dx12_runtime.fence,
                                         fence_target)))
    {
        return;
    }

    if (ID3D12Fence_GetCompletedValue(s_dx12_runtime.fence) < fence_target)
    {
        if (SUCCEEDED(ID3D12Fence_SetEventOnCompletion(s_dx12_runtime.fence,
                                                       fence_target,
                                                       s_dx12_runtime.fence_event)))
        {
            WaitForSingleObject(s_dx12_runtime.fence_event, INFINITE);
        }
    }
}

static int banana_dx12_runtime_create_rtv_targets(void)
{
    UINT i = 0;
    D3D12_CPU_DESCRIPTOR_HANDLE handle;

    if (!s_dx12_runtime.swap_chain || !s_dx12_runtime.rtv_heap || !s_dx12_runtime.device)
    {
        return 0;
    }

    banana_dx12_runtime_release_swapchain_buffers();

    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(s_dx12_runtime.rtv_heap, &handle);
    for (i = 0; i < BANANA_DX12_BACK_BUFFER_COUNT; i++)
    {
        IDXGISwapChain3_GetBuffer(s_dx12_runtime.swap_chain,
                                  i,
                                  &IID_ID3D12Resource,
                                  (void **)&s_dx12_runtime.render_targets[i]);
        if (!s_dx12_runtime.render_targets[i])
        {
            banana_dx12_runtime_release_swapchain_buffers();
            return 0;
        }
        ID3D12Device_CreateRenderTargetView(s_dx12_runtime.device,
                                            s_dx12_runtime.render_targets[i],
                                            NULL,
                                            handle);
        handle.ptr += s_dx12_runtime.rtv_descriptor_size;
    }

    s_dx12_runtime.frame_index = IDXGISwapChain3_GetCurrentBackBufferIndex(s_dx12_runtime.swap_chain);
    return 1;
}

static int banana_dx12_runtime_create_depth_target(void)
{
    D3D12_RESOURCE_DESC depth_desc;
    D3D12_HEAP_PROPERTIES heap_props;
    D3D12_CLEAR_VALUE clear_value;
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle;
    HRESULT result;

    if (!s_dx12_runtime.device || !s_dx12_runtime.dsv_heap ||
        s_dx12_runtime.width <= 0 || s_dx12_runtime.height <= 0)
    {
        return 0;
    }

    banana_dx12_runtime_release_depth_target();

    ZeroMemory(&depth_desc, sizeof(depth_desc));
    depth_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depth_desc.Alignment = 0;
    depth_desc.Width = (UINT64)s_dx12_runtime.width;
    depth_desc.Height = (UINT)s_dx12_runtime.height;
    depth_desc.DepthOrArraySize = 1;
    depth_desc.MipLevels = 1;
    depth_desc.Format = DXGI_FORMAT_D32_FLOAT;
    depth_desc.SampleDesc.Count = 1;
    depth_desc.SampleDesc.Quality = 0;
    depth_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depth_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    ZeroMemory(&heap_props, sizeof(heap_props));
    heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_props.CreationNodeMask = 1;
    heap_props.VisibleNodeMask = 1;

    ZeroMemory(&clear_value, sizeof(clear_value));
    clear_value.Format = DXGI_FORMAT_D32_FLOAT;
    clear_value.DepthStencil.Depth = 1.0f;
    clear_value.DepthStencil.Stencil = 0;

    result = ID3D12Device_CreateCommittedResource(s_dx12_runtime.device,
                                                  &heap_props,
                                                  D3D12_HEAP_FLAG_NONE,
                                                  &depth_desc,
                                                  D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                                  &clear_value,
                                                  &IID_ID3D12Resource,
                                                  (void **)&s_dx12_runtime.depth_stencil);
    if (FAILED(result) || !s_dx12_runtime.depth_stencil)
    {
        return 0;
    }

    ZeroMemory(&dsv_desc, sizeof(dsv_desc));
    dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(s_dx12_runtime.dsv_heap,
                                                            &dsv_handle);
    ID3D12Device_CreateDepthStencilView(s_dx12_runtime.device,
                                        s_dx12_runtime.depth_stencil,
                                        &dsv_desc,
                                        dsv_handle);
    return 1;
}

static int banana_dx12_runtime_recreate_swap_chain(int width, int height)
{
    IDXGISwapChain1 *swap_chain1 = NULL;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    HRESULT result;

    if (!s_dx12_runtime.factory || !s_dx12_runtime.command_queue || !s_dx12_runtime.window)
        return 0;

    if (s_dx12_runtime.swap_chain)
    {
        IDXGISwapChain3_Release(s_dx12_runtime.swap_chain);
        s_dx12_runtime.swap_chain = NULL;
    }

    ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
    swap_chain_desc.Width = (UINT)width;
    swap_chain_desc.Height = (UINT)height;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = BANANA_DX12_BACK_BUFFER_COUNT;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    result = IDXGIFactory4_CreateSwapChainForHwnd(s_dx12_runtime.factory,
                                                  (IUnknown *)s_dx12_runtime.command_queue,
                                                  s_dx12_runtime.window,
                                                  &swap_chain_desc,
                                                  NULL,
                                                  NULL,
                                                  &swap_chain1);
    if (FAILED(result) || !swap_chain1)
        return 0;

    result = IDXGISwapChain1_QueryInterface(swap_chain1,
                                            &IID_IDXGISwapChain3,
                                            (void **)&s_dx12_runtime.swap_chain);
    IDXGISwapChain1_Release(swap_chain1);
    if (FAILED(result) || !s_dx12_runtime.swap_chain)
        return 0;

    IDXGIFactory4_MakeWindowAssociation(s_dx12_runtime.factory,
                                        s_dx12_runtime.window,
                                        DXGI_MWA_NO_ALT_ENTER);
    s_dx12_runtime.width = width;
    s_dx12_runtime.height = height;
    return 1;
}

static void banana_dx12_runtime_reset_state(void)
{
    ZeroMemory(&s_dx12_runtime, sizeof(s_dx12_runtime));
    s_dx12_runtime.status = "dx12-runtime-inactive";
    s_dx12_runtime.last_present_result = S_OK;
    s_dx12_runtime.present_interval = 1;
    banana_dx12_runtime_update_telemetry();
}

static void banana_dx12_runtime_release_all(void)
{
    banana_dx12_runtime_wait_for_gpu();
    banana_dx12_runtime_release_swapchain_buffers();

    if (s_dx12_runtime.fence_event)
    {
        CloseHandle(s_dx12_runtime.fence_event);
        s_dx12_runtime.fence_event = NULL;
    }
    if (s_dx12_runtime.fence)
    {
        ID3D12Fence_Release(s_dx12_runtime.fence);
        s_dx12_runtime.fence = NULL;
    }
    if (s_dx12_runtime.command_list)
    {
        ID3D12GraphicsCommandList_Release(s_dx12_runtime.command_list);
        s_dx12_runtime.command_list = NULL;
    }
    banana_dx12_runtime_release_scene_pipeline();
    if (s_dx12_runtime.command_allocator)
    {
        ID3D12CommandAllocator_Release(s_dx12_runtime.command_allocator);
        s_dx12_runtime.command_allocator = NULL;
    }
    if (s_dx12_runtime.rtv_heap)
    {
        ID3D12DescriptorHeap_Release(s_dx12_runtime.rtv_heap);
        s_dx12_runtime.rtv_heap = NULL;
    }
    if (s_dx12_runtime.dsv_heap)
    {
        ID3D12DescriptorHeap_Release(s_dx12_runtime.dsv_heap);
        s_dx12_runtime.dsv_heap = NULL;
    }
    if (s_dx12_runtime.swap_chain)
    {
        IDXGISwapChain3_Release(s_dx12_runtime.swap_chain);
        s_dx12_runtime.swap_chain = NULL;
    }
    if (s_dx12_runtime.command_queue)
    {
        ID3D12CommandQueue_Release(s_dx12_runtime.command_queue);
        s_dx12_runtime.command_queue = NULL;
    }
    if (s_dx12_runtime.device)
    {
        ID3D12Device_Release(s_dx12_runtime.device);
        s_dx12_runtime.device = NULL;
    }
    if (s_dx12_runtime.adapter)
    {
        IDXGIAdapter1_Release(s_dx12_runtime.adapter);
        s_dx12_runtime.adapter = NULL;
    }
    if (s_dx12_runtime.factory)
    {
        IDXGIFactory4_Release(s_dx12_runtime.factory);
        s_dx12_runtime.factory = NULL;
    }
    if (s_dx12_runtime.window && s_dx12_runtime.owns_window)
    {
        DestroyWindow(s_dx12_runtime.window);
        s_dx12_runtime.window = NULL;
    }
    s_dx12_runtime.window = NULL;
    s_dx12_runtime.owns_window = 0;

    s_dx12_runtime.active = 0;
    s_dx12_runtime.width = 0;
    s_dx12_runtime.height = 0;
    s_dx12_runtime.frame_index = 0;
    s_dx12_runtime.fence_value = 0;
    s_dx12_runtime.command_list_recording = 0;
    s_dx12_runtime.scene_draw_calls_frame = 0;
    s_dx12_runtime.scene_draw_calls_total = 0;
    s_dx12_runtime.frame_counter = 0;
    s_dx12_runtime.frames_presented = 0;
    s_dx12_runtime.last_present_result = S_OK;
    free(s_dx12_runtime.ui_overlay_rgba);
    s_dx12_runtime.ui_overlay_rgba = NULL;
    s_dx12_runtime.ui_overlay_capacity = 0;
    s_dx12_runtime.ui_overlay_width = 0;
    s_dx12_runtime.ui_overlay_height = 0;
    s_dx12_runtime.ui_overlay_dirty = 0;
    s_dx12_runtime.ui_overlay_rows_last = 0;
    s_dx12_runtime.ui_overlay_quads_last = 0;
    s_dx12_runtime.ui_overlay_vertices_last = 0;
    banana_dx12_runtime_update_telemetry();
}

static IDXGIAdapter1 *banana_dx12_find_hardware_adapter(IDXGIFactory4 *factory)
{
    UINT adapter_index = 0;
    IDXGIAdapter1 *adapter = NULL;

    while (IDXGIFactory4_EnumAdapters1(factory, adapter_index, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 description;
        HRESULT result = IDXGIAdapter1_GetDesc1(adapter, &description);
        if (FAILED(result))
        {
            IDXGIAdapter1_Release(adapter);
            adapter = NULL;
            adapter_index++;
            continue;
        }

        if ((description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
        {
            IDXGIAdapter1_Release(adapter);
            adapter = NULL;
            adapter_index++;
            continue;
        }

        return adapter;
    }

    return NULL;
}
#endif

static void banana_dx12_probe_once(void)
{
    if (banana_dx12_diagnostics_probe_state() != BANANA_DX12_DIAGNOSTICS_PROBE_UNKNOWN)
    {
        return;
    }

#if !defined(BANANA_ENGINE_RENDER_BACKEND_DX12)
    banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_UNAVAILABLE,
                                            "dx12-not-requested");
#elif !defined(_WIN32)
    banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_UNAVAILABLE,
                                            "dx12-non-windows-host");
#else
    HINSTANCE instance = GetModuleHandleA(NULL);
    WNDCLASSA window_class;
    HWND window = NULL;
    IDXGIFactory4 *factory = NULL;
    IDXGIAdapter1 *adapter = NULL;
    ID3D12Device *device = NULL;
    ID3D12CommandQueue *command_queue = NULL;
    IDXGISwapChain1 *swap_chain = NULL;
    D3D12_COMMAND_QUEUE_DESC queue_desc;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    HRESULT result;

    if (!instance)
    {
        banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_UNAVAILABLE,
                                                "dx12-instance-unavailable");
        return;
    }
    if (!banana_dx12_create_probe_window(instance, &window_class, &window))
    {
        banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_UNAVAILABLE,
                                                "dx12-window-create-failed");
        return;
    }

    result = CreateDXGIFactory2(0,
                                &IID_IDXGIFactory4,
                                (void **)&factory);
    if (FAILED(result) || !factory)
    {
        banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_UNAVAILABLE,
                                                "dx12-factory-create-failed");
        goto cleanup;
    }

    adapter = banana_dx12_find_hardware_adapter(factory);
    if (!adapter)
    {
        banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_UNAVAILABLE,
                                                "dx12-adapter-not-found");
        goto cleanup;
    }

    result = D3D12CreateDevice((IUnknown *)adapter,
                               D3D_FEATURE_LEVEL_11_0,
                               &IID_ID3D12Device,
                               (void **)&device);
    if (FAILED(result) || !device)
    {
        banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_UNAVAILABLE,
                                                "dx12-device-create-failed");
        goto cleanup;
    }

    ZeroMemory(&queue_desc, sizeof(queue_desc));
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queue_desc.NodeMask = 0;

    result = ID3D12Device_CreateCommandQueue(device,
                                             &queue_desc,
                                             &IID_ID3D12CommandQueue,
                                             (void **)&command_queue);
    if (FAILED(result) || !command_queue)
    {
        banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_UNAVAILABLE,
                                                "dx12-command-queue-create-failed");
        goto cleanup;
    }

    ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
    swap_chain_desc.Width = 16;
    swap_chain_desc.Height = 16;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.Stereo = FALSE;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swap_chain_desc.Flags = 0;

    result = IDXGIFactory4_CreateSwapChainForHwnd(factory,
                                                  (IUnknown *)command_queue,
                                                  window,
                                                  &swap_chain_desc,
                                                  NULL,
                                                  NULL,
                                                  &swap_chain);
    if (FAILED(result) || !swap_chain)
    {
        banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_UNAVAILABLE,
                                                "dx12-swap-chain-create-failed");
        goto cleanup;
    }

    IDXGIFactory4_MakeWindowAssociation(factory, window, DXGI_MWA_NO_ALT_ENTER);
    banana_dx12_diagnostics_set_probe_state(BANANA_DX12_DIAGNOSTICS_PROBE_AVAILABLE,
                                            "dx12-bootstrap-ready");

cleanup:
    if (swap_chain)
        IDXGISwapChain1_Release(swap_chain);
    if (command_queue)
        ID3D12CommandQueue_Release(command_queue);
    if (device)
        ID3D12Device_Release(device);
    if (adapter)
        IDXGIAdapter1_Release(adapter);
    if (factory)
        IDXGIFactory4_Release(factory);
    DestroyWindow(window);
    UnregisterClassA(BANANA_DX12_PROBE_WINDOW_CLASS, instance);
#endif
}

int banana_dx12_backend_is_available(void)
{
    const char *force_unavailable = getenv("BANANA_DX12_FORCE_UNAVAILABLE");

    if (force_unavailable &&
        (strcmp(force_unavailable, "1") == 0 ||
         strcmp(force_unavailable, "true") == 0 ||
         strcmp(force_unavailable, "TRUE") == 0 ||
         strcmp(force_unavailable, "yes") == 0 ||
         strcmp(force_unavailable, "YES") == 0))
    {
        return 0;
    }

    banana_dx12_probe_once();
    return banana_dx12_diagnostics_probe_state() == BANANA_DX12_DIAGNOSTICS_PROBE_AVAILABLE;
}

const char *banana_dx12_backend_status(void)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    if (s_dx12_runtime.active)
    {
        return s_dx12_runtime.status;
    }
#endif
    banana_dx12_probe_once();
    return banana_dx12_diagnostics_probe_status();
}

const char *banana_dx12_runtime_telemetry(void)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    BananaDx12DiagnosticsSnapshot snapshot = banana_dx12_runtime_diagnostics_snapshot();
    return banana_dx12_diagnostics_runtime_telemetry(&snapshot);
#else
    return banana_dx12_diagnostics_runtime_unavailable_telemetry();
#endif
}

unsigned long long banana_dx12_runtime_frames_presented(void)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    return (unsigned long long)s_dx12_runtime.frames_presented;
#else
    return 0ull;
#endif
}

unsigned int banana_dx12_runtime_scene_draw_calls(void)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    return (unsigned int)s_dx12_runtime.scene_draw_calls_frame;
#else
    return 0u;
#endif
}

int banana_dx12_runtime_init(void *native_window, int width, int height)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    IDXGISwapChain1 *swap_chain1 = NULL;
    D3D12_COMMAND_QUEUE_DESC queue_desc;
    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc;
    D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    HRESULT result;

    if (s_dx12_runtime.active)
    {
        if (width == s_dx12_runtime.width && height == s_dx12_runtime.height)
        {
            return 1;
        }
        if (!banana_dx12_runtime_resize(width, height))
        {
            s_dx12_runtime.status = "dx12-runtime-resize-failed";
            return 0;
        }
        return 1;
    }

    banana_dx12_runtime_reset_state();
    if (!native_window)
    {
        s_dx12_runtime.status = "dx12-native-window-missing";
        return 0;
    }
    s_dx12_runtime.window = (HWND)native_window;
    s_dx12_runtime.owns_window = 0;
    s_dx12_runtime.present_interval = (UINT)banana_dx12_runtime_read_present_interval();

    result = CreateDXGIFactory2(0, &IID_IDXGIFactory4, (void **)&s_dx12_runtime.factory);
    if (FAILED(result) || !s_dx12_runtime.factory)
    {
        s_dx12_runtime.status = "dx12-factory-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    s_dx12_runtime.adapter = banana_dx12_find_hardware_adapter(s_dx12_runtime.factory);
    if (!s_dx12_runtime.adapter)
    {
        s_dx12_runtime.status = "dx12-adapter-not-found";
        banana_dx12_runtime_release_all();
        return 0;
    }

    result = D3D12CreateDevice((IUnknown *)s_dx12_runtime.adapter,
                               D3D_FEATURE_LEVEL_11_0,
                               &IID_ID3D12Device,
                               (void **)&s_dx12_runtime.device);
    if (FAILED(result) || !s_dx12_runtime.device)
    {
        s_dx12_runtime.status = "dx12-device-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    ZeroMemory(&queue_desc, sizeof(queue_desc));
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    result = ID3D12Device_CreateCommandQueue(s_dx12_runtime.device,
                                             &queue_desc,
                                             &IID_ID3D12CommandQueue,
                                             (void **)&s_dx12_runtime.command_queue);
    if (FAILED(result) || !s_dx12_runtime.command_queue)
    {
        s_dx12_runtime.status = "dx12-command-queue-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
    swap_chain_desc.Width = (UINT)width;
    swap_chain_desc.Height = (UINT)height;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = BANANA_DX12_BACK_BUFFER_COUNT;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    result = IDXGIFactory4_CreateSwapChainForHwnd(s_dx12_runtime.factory,
                                                  (IUnknown *)s_dx12_runtime.command_queue,
                                                  s_dx12_runtime.window,
                                                  &swap_chain_desc,
                                                  NULL,
                                                  NULL,
                                                  &swap_chain1);
    if (FAILED(result) || !swap_chain1)
    {
        s_dx12_runtime.status = "dx12-swap-chain-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    result = IDXGISwapChain1_QueryInterface(swap_chain1,
                                            &IID_IDXGISwapChain3,
                                            (void **)&s_dx12_runtime.swap_chain);
    IDXGISwapChain1_Release(swap_chain1);
    if (FAILED(result) || !s_dx12_runtime.swap_chain)
    {
        s_dx12_runtime.status = "dx12-swap-chain-query-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    IDXGIFactory4_MakeWindowAssociation(s_dx12_runtime.factory,
                                        s_dx12_runtime.window,
                                        DXGI_MWA_NO_ALT_ENTER);

    ZeroMemory(&rtv_heap_desc, sizeof(rtv_heap_desc));
    rtv_heap_desc.NumDescriptors = BANANA_DX12_BACK_BUFFER_COUNT;
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    result = ID3D12Device_CreateDescriptorHeap(s_dx12_runtime.device,
                                               &rtv_heap_desc,
                                               &IID_ID3D12DescriptorHeap,
                                               (void **)&s_dx12_runtime.rtv_heap);
    if (FAILED(result) || !s_dx12_runtime.rtv_heap)
    {
        s_dx12_runtime.status = "dx12-rtv-heap-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }
    s_dx12_runtime.rtv_descriptor_size = ID3D12Device_GetDescriptorHandleIncrementSize(
        s_dx12_runtime.device,
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    ZeroMemory(&dsv_heap_desc, sizeof(dsv_heap_desc));
    dsv_heap_desc.NumDescriptors = 1;
    dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    result = ID3D12Device_CreateDescriptorHeap(s_dx12_runtime.device,
                                               &dsv_heap_desc,
                                               &IID_ID3D12DescriptorHeap,
                                               (void **)&s_dx12_runtime.dsv_heap);
    if (FAILED(result) || !s_dx12_runtime.dsv_heap)
    {
        s_dx12_runtime.status = "dx12-dsv-heap-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    if (!banana_dx12_runtime_create_rtv_targets())
    {
        s_dx12_runtime.status = "dx12-render-target-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }
    if (!banana_dx12_runtime_create_depth_target())
    {
        s_dx12_runtime.status = "dx12-depth-target-unavailable";
    }

    result = ID3D12Device_CreateCommandAllocator(s_dx12_runtime.device,
                                                 D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                 &IID_ID3D12CommandAllocator,
                                                 (void **)&s_dx12_runtime.command_allocator);
    if (FAILED(result) || !s_dx12_runtime.command_allocator)
    {
        s_dx12_runtime.status = "dx12-command-allocator-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    result = ID3D12Device_CreateCommandList(s_dx12_runtime.device,
                                            0,
                                            D3D12_COMMAND_LIST_TYPE_DIRECT,
                                            s_dx12_runtime.command_allocator,
                                            NULL,
                                            &IID_ID3D12GraphicsCommandList,
                                            (void **)&s_dx12_runtime.command_list);
    if (FAILED(result) || !s_dx12_runtime.command_list)
    {
        s_dx12_runtime.status = "dx12-command-list-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }
    ID3D12GraphicsCommandList_Close(s_dx12_runtime.command_list);

    result = ID3D12Device_CreateFence(s_dx12_runtime.device,
                                      0,
                                      D3D12_FENCE_FLAG_NONE,
                                      &IID_ID3D12Fence,
                                      (void **)&s_dx12_runtime.fence);
    if (FAILED(result) || !s_dx12_runtime.fence)
    {
        s_dx12_runtime.status = "dx12-fence-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }
    s_dx12_runtime.fence_value = 0;
    s_dx12_runtime.fence_event = CreateEventA(NULL, FALSE, FALSE, NULL);
    if (!s_dx12_runtime.fence_event)
    {
        s_dx12_runtime.status = "dx12-fence-event-create-failed";
        banana_dx12_runtime_release_all();
        return 0;
    }

    s_dx12_runtime.width = width;
    s_dx12_runtime.height = height;

    if (!banana_dx12_runtime_create_scene_pipeline())
    {
        s_dx12_runtime.scene_pipeline_ready = 0;
        s_dx12_runtime.status = "dx12-runtime-ready-no-scene-pipeline";
    }
    else
    {
        s_dx12_runtime.status = "dx12-runtime-ready";
    }

    s_dx12_runtime.active = 1;
    banana_dx12_runtime_update_telemetry();
    return 1;
#else
    (void)native_window;
    (void)width;
    (void)height;
    return 0;
#endif
}

int banana_dx12_runtime_resize(int width, int height)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    HRESULT result;
    if (!s_dx12_runtime.active || !s_dx12_runtime.swap_chain)
    {
        return 0;
    }

    if (width <= 0 || height <= 0)
        return 1;

    banana_dx12_runtime_wait_for_gpu();
    banana_dx12_runtime_release_swapchain_buffers();

    result = IDXGISwapChain3_ResizeBuffers(s_dx12_runtime.swap_chain,
                                           BANANA_DX12_BACK_BUFFER_COUNT,
                                           (UINT)width,
                                           (UINT)height,
                                           DXGI_FORMAT_R8G8B8A8_UNORM,
                                           0);
    if (FAILED(result))
    {
        if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET)
        {
            if (banana_dx12_runtime_recreate_swap_chain(width, height) &&
                banana_dx12_runtime_create_rtv_targets() &&
                banana_dx12_runtime_create_depth_target())
            {
                s_dx12_runtime.status = "dx12-runtime-recovered";
                banana_dx12_runtime_update_telemetry();
                return 1;
            }
            s_dx12_runtime.status = "dx12-recreate-after-loss-failed";
            banana_dx12_runtime_update_telemetry();
            return 0;
        }
        s_dx12_runtime.status = "dx12-resize-buffers-failed";
        banana_dx12_runtime_update_telemetry();
        return 0;
    }
    s_dx12_runtime.width = width;
    s_dx12_runtime.height = height;

    if (!banana_dx12_runtime_create_rtv_targets())
    {
        s_dx12_runtime.status = "dx12-resize-targets-failed";
        banana_dx12_runtime_update_telemetry();
        return 0;
    }
    if (!banana_dx12_runtime_create_depth_target())
    {
        s_dx12_runtime.status = "dx12-depth-target-unavailable";
    }
    s_dx12_runtime.status = "dx12-runtime-ready";
    banana_dx12_runtime_update_telemetry();
    return 1;
#else
    (void)width;
    (void)height;
    return 0;
#endif
}

int banana_dx12_runtime_begin_frame(float clear_r, float clear_g, float clear_b, float clear_a)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    D3D12_RESOURCE_BARRIER transition_to_rtv;
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle;
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle;
    D3D12_VIEWPORT viewport;
    D3D12_RECT scissor_rect;
    float color[4] = {clear_r, clear_g, clear_b, clear_a};

    if (!s_dx12_runtime.active || !s_dx12_runtime.command_allocator || !s_dx12_runtime.command_list ||
        !s_dx12_runtime.render_targets[s_dx12_runtime.frame_index])
    {
        return 0;
    }

    if (FAILED(ID3D12CommandAllocator_Reset(s_dx12_runtime.command_allocator)))
    {
        s_dx12_runtime.status = "dx12-command-allocator-reset-failed";
        return 0;
    }
    if (FAILED(ID3D12GraphicsCommandList_Reset(s_dx12_runtime.command_list,
                                               s_dx12_runtime.command_allocator,
                                               NULL)))
    {
        s_dx12_runtime.status = "dx12-command-list-reset-failed";
        return 0;
    }

    ZeroMemory(&transition_to_rtv, sizeof(transition_to_rtv));
    transition_to_rtv.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    transition_to_rtv.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    transition_to_rtv.Transition.pResource = s_dx12_runtime.render_targets[s_dx12_runtime.frame_index];
    transition_to_rtv.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    transition_to_rtv.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    transition_to_rtv.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    ID3D12GraphicsCommandList_ResourceBarrier(s_dx12_runtime.command_list, 1, &transition_to_rtv);

    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(s_dx12_runtime.rtv_heap,
                                                            &rtv_handle);
    rtv_handle.ptr += ((SIZE_T)s_dx12_runtime.frame_index * s_dx12_runtime.rtv_descriptor_size);
    ZeroMemory(&dsv_handle, sizeof(dsv_handle));

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = (float)s_dx12_runtime.width;
    viewport.Height = (float)s_dx12_runtime.height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    scissor_rect.left = 0;
    scissor_rect.top = 0;
    scissor_rect.right = s_dx12_runtime.width;
    scissor_rect.bottom = s_dx12_runtime.height;

    ID3D12GraphicsCommandList_RSSetViewports(s_dx12_runtime.command_list, 1, &viewport);
    ID3D12GraphicsCommandList_RSSetScissorRects(s_dx12_runtime.command_list, 1, &scissor_rect);
    ID3D12GraphicsCommandList_OMSetRenderTargets(s_dx12_runtime.command_list,
                                                 1,
                                                 &rtv_handle,
                                                 FALSE,
                                                 NULL);

    ID3D12GraphicsCommandList_ClearRenderTargetView(s_dx12_runtime.command_list,
                                                    rtv_handle,
                                                    color,
                                                    0,
                                                    NULL);
    s_dx12_runtime.command_list_recording = 1;
    s_dx12_runtime.scene_draw_calls_frame = 0;
    s_dx12_runtime.scene_vertex_count = 0;
    s_dx12_runtime.frame_counter += 1;

    banana_dx12_runtime_push_quad(0.0f,
                                  0.0f,
                                  0.92f,
                                  0.92f,
                                  0.90f,
                                  0.34f,
                                  0.56f,
                                  0.22f,
                                  1.0f);

    s_dx12_runtime.status = "dx12-frame-recording";
    banana_dx12_runtime_update_telemetry();
    return 1;
#else
    (void)clear_r;
    (void)clear_g;
    (void)clear_b;
    (void)clear_a;
    return 0;
#endif
}

int banana_dx12_runtime_end_frame(void)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    D3D12_RESOURCE_BARRIER transition_to_present;
    ID3D12CommandList *command_lists[1];
    HRESULT result;
    if (!s_dx12_runtime.active || !s_dx12_runtime.swap_chain || !s_dx12_runtime.command_list_recording)
    {
        return 0;
    }

    banana_dx12_runtime_append_ui_overlay_vertices();

    if (s_dx12_runtime.scene_pipeline_ready && s_dx12_runtime.scene_vertex_count > 0)
    {
        ID3D12GraphicsCommandList_SetGraphicsRootSignature(s_dx12_runtime.command_list,
                                                           s_dx12_runtime.scene_root_signature);
        ID3D12GraphicsCommandList_SetPipelineState(s_dx12_runtime.command_list,
                                                   s_dx12_runtime.scene_pipeline_state);
        ID3D12GraphicsCommandList_IASetPrimitiveTopology(s_dx12_runtime.command_list,
                                                         D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ID3D12GraphicsCommandList_IASetVertexBuffers(s_dx12_runtime.command_list,
                                                     0,
                                                     1,
                                                     &s_dx12_runtime.scene_vertex_buffer_view);
        ID3D12GraphicsCommandList_DrawInstanced(s_dx12_runtime.command_list,
                                                s_dx12_runtime.scene_vertex_count,
                                                1,
                                                0,
                                                0);
    }

    ZeroMemory(&transition_to_present, sizeof(transition_to_present));
    transition_to_present.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    transition_to_present.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    transition_to_present.Transition.pResource = s_dx12_runtime.render_targets[s_dx12_runtime.frame_index];
    transition_to_present.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    transition_to_present.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    transition_to_present.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    ID3D12GraphicsCommandList_ResourceBarrier(s_dx12_runtime.command_list, 1, &transition_to_present);

    if (FAILED(ID3D12GraphicsCommandList_Close(s_dx12_runtime.command_list)))
    {
        s_dx12_runtime.status = "dx12-command-list-close-failed";
        s_dx12_runtime.command_list_recording = 0;
        banana_dx12_runtime_update_telemetry();
        return 0;
    }

    command_lists[0] = (ID3D12CommandList *)s_dx12_runtime.command_list;
    ID3D12CommandQueue_ExecuteCommandLists(s_dx12_runtime.command_queue, 1, command_lists);

    result = IDXGISwapChain3_Present(s_dx12_runtime.swap_chain, s_dx12_runtime.present_interval, 0);
    s_dx12_runtime.last_present_result = result;
    if (FAILED(result))
    {
        if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET)
        {
            if (banana_dx12_runtime_recreate_swap_chain(s_dx12_runtime.width, s_dx12_runtime.height) &&
                banana_dx12_runtime_create_rtv_targets() &&
                banana_dx12_runtime_create_depth_target())
            {
                s_dx12_runtime.command_list_recording = 0;
                s_dx12_runtime.status = "dx12-runtime-recovered";
                banana_dx12_runtime_update_telemetry();
                return 1;
            }
            s_dx12_runtime.status = "dx12-present-device-loss";
            s_dx12_runtime.command_list_recording = 0;
            banana_dx12_runtime_update_telemetry();
            return 0;
        }
        s_dx12_runtime.status = "dx12-present-failed";
        s_dx12_runtime.command_list_recording = 0;
        banana_dx12_runtime_update_telemetry();
        return 0;
    }

    banana_dx12_runtime_wait_for_gpu();
    s_dx12_runtime.frame_index = IDXGISwapChain3_GetCurrentBackBufferIndex(s_dx12_runtime.swap_chain);
    s_dx12_runtime.command_list_recording = 0;
    s_dx12_runtime.frames_presented += 1;
    s_dx12_runtime.status = "dx12-runtime-ready";
    banana_dx12_runtime_update_telemetry();
    return 1;
#else
    return 0;
#endif
}

void banana_dx12_runtime_set_ui_overlay(const unsigned char *rgba,
                                        int width,
                                        int height)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    size_t required = 0;

    if (!rgba || width <= 0 || height <= 0)
    {
        s_dx12_runtime.ui_overlay_width = 0;
        s_dx12_runtime.ui_overlay_height = 0;
        s_dx12_runtime.ui_overlay_dirty = 0;
        return;
    }

    required = (size_t)width * (size_t)height * 4u;
    if (!banana_dx12_runtime_ensure_ui_overlay_capacity(required))
        return;

    memcpy(s_dx12_runtime.ui_overlay_rgba, rgba, required);
    s_dx12_runtime.ui_overlay_width = width;
    s_dx12_runtime.ui_overlay_height = height;
    s_dx12_runtime.ui_overlay_dirty = 1;
    banana_dx12_runtime_update_telemetry();
#else
    (void)rgba;
    (void)width;
    (void)height;
#endif
}

void banana_dx12_runtime_set_camera(const float *eye,
                                    const float *target,
                                    const float *up,
                                    float fov_degrees,
                                    float aspect)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    if (!eye || !target || !up)
    {
        s_dx12_runtime.camera_valid = 0;
        return;
    }

    s_dx12_runtime.camera_eye[0] = eye[0];
    s_dx12_runtime.camera_eye[1] = eye[1];
    s_dx12_runtime.camera_eye[2] = eye[2];
    s_dx12_runtime.camera_target[0] = target[0];
    s_dx12_runtime.camera_target[1] = target[1];
    s_dx12_runtime.camera_target[2] = target[2];
    s_dx12_runtime.camera_up[0] = up[0];
    s_dx12_runtime.camera_up[1] = up[1];
    s_dx12_runtime.camera_up[2] = up[2];
    s_dx12_runtime.camera_fov_degrees = fov_degrees;
    s_dx12_runtime.camera_aspect = aspect;
    s_dx12_runtime.camera_valid = 1;
#else
    (void)eye;
    (void)target;
    (void)up;
    (void)fov_degrees;
    (void)aspect;
#endif
}

void banana_dx12_runtime_shutdown(void)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    banana_dx12_runtime_release_all();
#else
    banana_dx12_runtime_update_telemetry();
#endif
}

void banana_dx12_runtime_submit_scene_draw(const Mesh *mesh,
                                           const float *position,
                                           const float *scale,
                                           int uses_texture,
                                           float color_r,
                                           float color_g,
                                           float color_b)
{
#if defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)
    float ndc_x = 0.0f;
    float ndc_y = 0.0f;
    float ndc_z = 0.20f;
    float view_depth = 0.0f;
    float half_width = 0.030f;
    float half_height = 0.030f;
    float r = uses_texture ? 0.95f : 0.35f;
    float g = uses_texture ? 0.34f : 0.88f;
    float b = uses_texture ? 0.20f : 0.96f;

    if (!s_dx12_runtime.active || !s_dx12_runtime.command_list_recording)
        return;

    if (!position || !scale)
        return;

    if (!s_dx12_runtime.scene_pipeline_ready)
        return;

    r = banana_dx12_clampf(color_r, 0.15f, 1.0f);
    g = banana_dx12_clampf(color_g, 0.15f, 1.0f);
    b = banana_dx12_clampf(color_b, 0.15f, 1.0f);

    if (banana_dx12_runtime_push_mesh(mesh, position, scale, uses_texture, r, g, b, 1.0f))
    {
        s_dx12_runtime.scene_draw_calls_frame += 1;
        s_dx12_runtime.scene_draw_calls_total += 1;
        return;
    }

    banana_dx12_project_world_to_clip(position[0],
                                      position[1] + (scale[1] * 0.50f),
                                      position[2],
                                      &ndc_x,
                                      &ndc_y,
                                      &ndc_z,
                                      &view_depth);

    half_width = banana_dx12_clampf(scale[0] / (6.0f + (view_depth * 0.36f)), 0.014f, 0.080f);
    half_height = banana_dx12_clampf(scale[2] / (6.0f + (view_depth * 0.36f)), 0.014f, 0.080f);

    if (!banana_dx12_runtime_push_quad(ndc_x,
                                       ndc_y,
                                       half_width,
                                       half_height,
                                       ndc_z,
                                       r,
                                       g,
                                       b,
                                       1.0f))
    {
        return;
    }

    s_dx12_runtime.scene_draw_calls_frame += 1;
    s_dx12_runtime.scene_draw_calls_total += 1;
#else
    (void)mesh;
    (void)position;
    (void)scale;
    (void)uses_texture;
    (void)color_r;
    (void)color_g;
    (void)color_b;
#endif
}
