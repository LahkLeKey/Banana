#include "window.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef BANANA_ENGINE_HAS_GLFW
#include <GLFW/glfw3.h>

struct Window {
    GLFWwindow *glfw;
    int width;
    int height;
};

Window *window_create(int width, int height, const char *title) {
    if (!glfwInit()) {
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
    if (!glfw) {
        fprintf(stderr, "[engine/render] glfwCreateWindow failed\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(glfw);
    glfwSwapInterval(1);

    Window *w = malloc(sizeof(Window));
    w->glfw   = glfw;
    w->width  = width;
    w->height = height;
    return w;
}

int  window_is_open(Window *w)      { return !glfwWindowShouldClose(w->glfw); }
void window_poll_events(Window *w)  { (void)w; glfwPollEvents(); }
void window_swap_buffers(Window *w) { glfwSwapBuffers(w->glfw); }

void window_get_size(Window *w, int *width, int *height) {
    glfwGetFramebufferSize(w->glfw, width, height);
}

void window_destroy(Window *w) {
    if (!w) return;
    glfwDestroyWindow(w->glfw);
    glfwTerminate();
    free(w);
}

#else  /* BANANA_ENGINE_STUB_RENDER */

struct Window { int width; int height; int open; };

Window *window_create(int width, int height, const char *title) {
    (void)title;
    Window *w = malloc(sizeof(Window));
    w->width = width; w->height = height; w->open = 1;
    return w;
}
int  window_is_open(Window *w)      { return w->open; }
void window_poll_events(Window *w)  { (void)w; }
void window_swap_buffers(Window *w) { (void)w; }
void window_get_size(Window *w, int *width, int *height) {
    *width = w->width; *height = w->height;
}
void window_destroy(Window *w)      { free(w); }

#endif /* BANANA_ENGINE_HAS_GLFW */
