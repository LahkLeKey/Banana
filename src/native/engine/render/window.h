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

    /* Present the back buffer to the screen. */
    void window_swap_buffers(Window *w);

    /* Retrieve current framebuffer dimensions (may differ from creation size). */
    void window_get_size(Window *w, int *width, int *height);

    /* Destroy the window and its OpenGL context. */
    void window_destroy(Window *w);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_WINDOW_H */
