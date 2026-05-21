#include "render/renderer.h"

#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    Renderer *renderer = renderer_create(8, 8);
    if (!expect_int("renderer created", renderer != NULL ? 1 : 0, 1))
        return 1;

    renderer_begin_frame(renderer);
    renderer_end_frame(renderer);

    const unsigned char *frame = renderer_get_frame_buffer(renderer);
    if (!expect_int("frame available", frame != NULL ? 1 : 0, 1))
        return 1;

    if (!expect_int("alpha set", frame[3] == 0xFF ? 1 : 0, 1))
        return 1;

    renderer_resize(renderer, 16, 16);
    renderer_begin_frame(renderer);
    renderer_end_frame(renderer);
    frame = renderer_get_frame_buffer(renderer);
    if (!expect_int("resized frame available", frame != NULL ? 1 : 0, 1))
        return 1;

    renderer_destroy(renderer);
    return 0;
}
