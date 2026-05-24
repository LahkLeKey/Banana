#include "window.h"
#include <stdio.h>
#include <stdlib.h>

/* ══════════════════════════════════════════════════════════════════════════
 * WASM / Emscripten path — WebGL 2 canvas context
 * ══════════════════════════════════════════════════════════════════════════ */
#ifdef __EMSCRIPTEN__

#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>

struct Window
{
    int width;
    int height;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl;
    int should_close;
    double mouse_x;
    double mouse_y;
    int right_click_pending;
};

static EM_BOOL window_mouse_move_callback(int event_type, const EmscriptenMouseEvent *event,
                                          void *user_data)
{
    (void)event_type;
    Window *w = (Window *)user_data;
    if (!w)
        return EM_FALSE;
    w->mouse_x = event->canvasX;
    w->mouse_y = event->canvasY;
    return EM_FALSE;
}

static EM_BOOL window_mouse_down_callback(int event_type, const EmscriptenMouseEvent *event,
                                          void *user_data)
{
    (void)event_type;
    Window *w = (Window *)user_data;
    if (!w)
        return EM_FALSE;
    w->mouse_x = event->canvasX;
    w->mouse_y = event->canvasY;
    if (event->button == 2)
    {
        w->right_click_pending = 1;
        return EM_FALSE;
    }
    return EM_FALSE;
}

Window *window_create(int width, int height, const char *title)
{
    (void)title;
    Window *w = (Window *)calloc(1, sizeof(Window));
    w->width = width;
    w->height = height;
    w->mouse_x = width * 0.5;
    w->mouse_y = height * 0.5;

    emscripten_set_canvas_element_size("#canvas", width, height);

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2; /* WebGL 2 → OpenGL ES 3.0 */
    attrs.minorVersion = 0;
    attrs.antialias = 1;
    attrs.alpha = 0;

    w->gl = emscripten_webgl_create_context("#canvas", &attrs);
    if (w->gl <= 0)
    {
        fprintf(stderr, "[engine/window] WebGL2 context creation failed (%d)\n", (int)w->gl);
        free(w);
        return NULL;
    }
    emscripten_webgl_make_context_current(w->gl);
    emscripten_set_mousemove_callback("#canvas", w, 1, window_mouse_move_callback);
    fprintf(stdout, "[engine/window] WebGL2 context created %dx%d\n", width, height);
    return w;
}

int window_is_open(Window *w)
{
    return w && !w->should_close;
}
void window_poll_events(Window *w)
{
    (void)w; /* browser handles events */
}

int window_take_right_click(Window *w, float *x, float *y)
{
    if (!w || !w->right_click_pending)
        return 0;
    w->right_click_pending = 0;
    if (x)
        *x = (float)w->mouse_x;
    if (y)
        *y = (float)w->mouse_y;
    return 1;
}
void window_swap_buffers(Window *w)
{
    (void)w; /* WebGL auto-presents    */
}

void window_get_size(Window *w, int *width, int *height)
{
    double css_w = 0.0;
    double css_h = 0.0;
    double dpr = emscripten_get_device_pixel_ratio();
    if (dpr <= 0.0)
        dpr = 1.0;

    if (emscripten_get_element_css_size("#canvas", &css_w, &css_h) == EMSCRIPTEN_RESULT_SUCCESS)
    {
        int fb_w = (int)(css_w * dpr);
        int fb_h = (int)(css_h * dpr);
        if (fb_w > 0 && fb_h > 0 && (fb_w != w->width || fb_h != w->height))
        {
            emscripten_set_canvas_element_size("#canvas", fb_w, fb_h);
            w->width = fb_w;
            w->height = fb_h;
        }
    }

    if (width)
        *width = w->width;
    if (height)
        *height = w->height;
}

void window_get_input_size(Window *w, int *width, int *height)
{
    double css_w = 0.0;
    double css_h = 0.0;
    if (emscripten_get_element_css_size("#canvas", &css_w, &css_h) == EMSCRIPTEN_RESULT_SUCCESS)
    {
        if (width)
            *width = (int)css_w;
        if (height)
            *height = (int)css_h;
        return;
    }

    if (width)
        *width = w->width;
    if (height)
        *height = w->height;
}

void *window_get_native_handle(Window *w)
{
    (void)w;
    return NULL;
}

void window_destroy(Window *w)
{
    if (!w)
        return;
    emscripten_webgl_destroy_context(w->gl);
    free(w);
}

/* ══════════════════════════════════════════════════════════════════════════
 * Native path — Win32 + DX12
 * ══════════════════════════════════════════════════════════════════════════ */
#elif defined(BANANA_ENGINE_RENDER_BACKEND_DX12) && defined(_WIN32)

#include <windows.h>

static const char *BANANA_DX12_WINDOW_CLASS = "BananaDx12RuntimeWindow";

struct Window
{
    HINSTANCE instance;
    HWND hwnd;
    int width;
    int height;
    int open;
    int right_click_pending;
    float right_click_x;
    float right_click_y;
};

static LRESULT CALLBACK window_dx12_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
    Window *window = (Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (message)
    {
    case WM_NCCREATE:
    {
        CREATESTRUCT *create = (CREATESTRUCT *)l_param;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)create->lpCreateParams);
        break;
    }
    case WM_SIZE:
        if (window)
        {
            RECT rect;
            if (GetClientRect(hwnd, &rect))
            {
                window->width = rect.right - rect.left;
                window->height = rect.bottom - rect.top;
            }
        }
        break;
    case WM_RBUTTONDOWN:
        if (window)
        {
            window->right_click_pending = 1;
            window->right_click_x = (float)(short)LOWORD(l_param);
            window->right_click_y = (float)(short)HIWORD(l_param);
        }
        return 0;
    case WM_SETCURSOR:
        /* Always show a normal pointer in gameplay/runtime windows. */
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        return TRUE;
    case WM_CLOSE:
        if (window)
            window->open = 0;
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        if (window)
            window->open = 0;
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    (void)w_param;
    return DefWindowProcA(hwnd, message, w_param, l_param);
}

Window *window_create(int width, int height, const char *title)
{
    Window *w = NULL;
    WNDCLASSA window_class;
    RECT rect;

    HINSTANCE instance = GetModuleHandleA(NULL);
    if (!instance)
    {
        fprintf(stderr, "[engine/render] GetModuleHandleA failed\n");
        return NULL;
    }

    w = (Window *)calloc(1, sizeof(Window));
    if (!w)
    {
        return NULL;
    }
    w->instance = instance;
    w->width = width;
    w->height = height;
    w->open = 1;

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = window_dx12_proc;
    window_class.hInstance = instance;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = BANANA_DX12_WINDOW_CLASS;
    if (!RegisterClassA(&window_class) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
    {
        fprintf(stderr, "[engine/render] RegisterClassA failed\n");
        free(w);
        return NULL;
    }

    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    w->hwnd = CreateWindowExA(0,
                              BANANA_DX12_WINDOW_CLASS,
                              title ? title : "Banana Engine DX12",
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              rect.right - rect.left,
                              rect.bottom - rect.top,
                              NULL,
                              NULL,
                              instance,
                              w);
    if (!w->hwnd)
    {
        fprintf(stderr, "[engine/render] CreateWindowExA failed\n");
        UnregisterClassA(BANANA_DX12_WINDOW_CLASS, instance);
        free(w);
        return NULL;
    }

    ShowWindow(w->hwnd, SW_SHOW);
    UpdateWindow(w->hwnd);
    return w;
}

int window_is_open(Window *w)
{
    return w && w->open;
}

void window_poll_events(Window *w)
{
    MSG msg;
    if (!w)
        return;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            w->open = 0;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int window_take_right_click(Window *w, float *x, float *y)
{
    if (!w || !w->right_click_pending)
        return 0;

    w->right_click_pending = 0;
    if (x)
        *x = w->right_click_x;
    if (y)
        *y = w->right_click_y;
    return 1;
}

void window_swap_buffers(Window *w)
{
    (void)w;
}

void window_get_size(Window *w, int *width, int *height)
{
    RECT rect;
    if (w && w->hwnd && GetClientRect(w->hwnd, &rect))
    {
        w->width = rect.right - rect.left;
        w->height = rect.bottom - rect.top;
    }
    if (width)
        *width = w ? w->width : 0;
    if (height)
        *height = w ? w->height : 0;
}

void window_get_input_size(Window *w, int *width, int *height)
{
    window_get_size(w, width, height);
}

void *window_get_native_handle(Window *w)
{
    return w ? (void *)w->hwnd : NULL;
}

void window_destroy(Window *w)
{
    if (!w)
        return;
    if (w->hwnd)
        DestroyWindow(w->hwnd);
    UnregisterClassA(BANANA_DX12_WINDOW_CLASS, w->instance);
    free(w);
}

/* ══════════════════════════════════════════════════════════════════════════
 * Native path — GLFW + OpenGL 3.3 core
 * ══════════════════════════════════════════════════════════════════════════ */
#elif defined(BANANA_ENGINE_HAS_GLFW)

#include <GLFW/glfw3.h>

struct Window
{
    GLFWwindow *glfw;
    int width;
    int height;
};

Window *window_create(int width, int height, const char *title)
{
    if (!glfwInit())
    {
        fprintf(stderr, "[engine/render] glfwInit failed\n");
        return NULL;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
#endif
    GLFWwindow *glfw = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!glfw)
    {
        fprintf(stderr, "[engine/render] glfwCreateWindow failed\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(glfw);
    glfwSwapInterval(1);

    Window *w = malloc(sizeof(Window));
    w->glfw = glfw;
    w->width = width;
    w->height = height;
    return w;
}

int window_is_open(Window *w)
{
    return !glfwWindowShouldClose(w->glfw);
}
void window_poll_events(Window *w)
{
    (void)w;
    glfwPollEvents();
}
void window_swap_buffers(Window *w)
{
    glfwSwapBuffers(w->glfw);
}

void window_get_size(Window *w, int *width, int *height)
{
    glfwGetFramebufferSize(w->glfw, width, height);
}

void window_get_input_size(Window *w, int *width, int *height)
{
    if (width)
        *width = w->width;
    if (height)
        *height = w->height;
}

void *window_get_native_handle(Window *w)
{
    return w ? (void *)w->glfw : NULL;
}

void window_destroy(Window *w)
{
    if (!w)
        return;
    glfwDestroyWindow(w->glfw);
    glfwTerminate();
    free(w);
}

/* ══════════════════════════════════════════════════════════════════════════
 * Headless stub — used in test builds (no GL context)
 * ══════════════════════════════════════════════════════════════════════════ */
#else

struct Window
{
    int width;
    int height;
    int open;
};

Window *window_create(int width, int height, const char *title)
{
    (void)title;
    Window *w = malloc(sizeof(Window));
    w->width = width;
    w->height = height;
    w->open = 1;
    return w;
}
int window_is_open(Window *w)
{
    return w->open;
}
void window_poll_events(Window *w)
{
    (void)w;
}
void window_swap_buffers(Window *w)
{
    (void)w;
}
void window_get_size(Window *w, int *width, int *height)
{
    *width = w->width;
    *height = w->height;
}

void window_get_input_size(Window *w, int *width, int *height)
{
    if (width)
        *width = w->width;
    if (height)
        *height = w->height;
}

void *window_get_native_handle(Window *w)
{
    (void)w;
    return NULL;
}
void window_destroy(Window *w)
{
    free(w);
}

#endif /* platform selector */
