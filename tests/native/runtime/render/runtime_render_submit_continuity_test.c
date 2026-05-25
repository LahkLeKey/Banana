#include "runtime/render/submit/render_submit.h"

#include <math.h>
#include <stdio.h>

static int expect_near(const char *label, float actual, float expected, float tolerance)
{
    if (fabsf(actual - expected) <= tolerance)
        return 1;

    fprintf(stderr,
            "%s expected=%.9f actual=%.9f tolerance=%.9f\n",
            label,
            expected,
            actual,
            tolerance);
    return 0;
}

int main(void)
{
    RendererDrawCommand command = {0};

    command.position[0] = 1.23456789f;
    command.position[1] = -0.99812345f;
    command.position[2] = 5.00048820f;
    command.scale[0] = 1.11111110f;
    command.scale[1] = 0.99971120f;
    command.scale[2] = 1.25002130f;

    runtime_render_apply_viewport_chunk_continuity_transform(&command);

    if (!expect_near("position x snapped", command.position[0], 1.23437500f, 1e-7f))
        return 1;
    if (!expect_near("position y snapped", command.position[1], -0.99804688f, 1e-7f))
        return 1;
    if (!expect_near("position z snapped", command.position[2], 5.00097656f, 1e-7f))
        return 1;

    if (!expect_near("scale x snapped", command.scale[0], 1.11132813f, 1e-7f))
        return 1;
    if (!expect_near("scale y snapped", command.scale[1], 0.99951172f, 1e-7f))
        return 1;
    if (!expect_near("scale z snapped", command.scale[2], 1.25f, 1e-7f))
        return 1;

    return 0;
}
