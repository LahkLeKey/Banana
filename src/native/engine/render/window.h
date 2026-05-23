#ifndef BANANA_ENGINE_WINDOW_H
#define BANANA_ENGINE_WINDOW_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct Window Window;

    /* Create a window and initialize an OpenGL 3.3 core context.
     * Returns NULL on failure (error logged to stderr). */
    Window *window_create(int width, int height, const char *title);

    /* Returns 1 while the window has not been closed. */
    int window_is_open(Window *w);

    /* Process pending OS events (keyboard, mouse, resize). */
    void window_poll_events(Window *w);

    /* Return 1 once when the right mouse button is pressed on the canvas. */
    int window_take_right_click(Window *w, float *x, float *y);

    /* Present the back buffer to the screen. */
    void window_swap_buffers(Window *w);

    /* Retrieve current framebuffer dimensions (may differ from creation size). */
    void window_get_size(Window *w, int *width, int *height);

    /* Retrieve current input-space dimensions (CSS pixels on Web/WASM). */
    void window_get_input_size(Window *w, int *width, int *height);

    /* Retrieve platform-native window handle (HWND/GLFWwindow/etc) when available. */
    void *window_get_native_handle(Window *w);

    /* Destroy the window and its OpenGL context. */
    void window_destroy(Window *w);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_WINDOW_H */
