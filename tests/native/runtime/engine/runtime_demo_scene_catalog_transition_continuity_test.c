#include "win32_dx12_poc/scene/demo_scene_catalog.h"

#include <stdio.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    unsigned int baseline_signature =
        banana_poc_demo_scene_catalog_coherent_transition_signature(2, 3);
    int iteration = 0;

    if (!expect_true("connected station->corridor transition is detected",
                     banana_poc_demo_scene_catalog_coherent_transition_connected(2, 3)))
    {
        return 1;
    }

    if (!expect_true("connected corridor->station transition is detected",
                     banana_poc_demo_scene_catalog_coherent_transition_connected(3, 2)))
    {
        return 1;
    }

    if (!expect_true("transition signature exists", baseline_signature != 0u))
        return 1;

    for (iteration = 0; iteration < 16; iteration++)
    {
        unsigned int current =
            banana_poc_demo_scene_catalog_coherent_transition_signature(2, 3);

        if (!expect_true("transition signature is deterministic", current == baseline_signature))
            return 1;
    }

    printf("runtime_demo_scene_catalog_transition_continuity_test: pass\n");
    return 0;
}
