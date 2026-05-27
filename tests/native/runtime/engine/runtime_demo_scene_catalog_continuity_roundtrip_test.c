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
    unsigned int baseline_forward =
        banana_poc_demo_scene_catalog_coherent_transition_signature(2, 3);
    unsigned int baseline_return =
        banana_poc_demo_scene_catalog_coherent_transition_signature(3, 2);
    unsigned int baseline_roundtrip = baseline_forward ^ baseline_return;
    int iteration = 0;

    if (!expect_true("forward transition is connected",
                     banana_poc_demo_scene_catalog_coherent_transition_connected(2, 3)))
    {
        return 1;
    }

    if (!expect_true("return transition is connected",
                     banana_poc_demo_scene_catalog_coherent_transition_connected(3, 2)))
    {
        return 1;
    }

    if (!expect_true("forward signature is non-zero", baseline_forward != 0u))
        return 1;

    if (!expect_true("return signature is non-zero", baseline_return != 0u))
        return 1;

    if (!expect_true("roundtrip signature is non-zero", baseline_roundtrip != 0u))
        return 1;

    for (iteration = 0; iteration < 10; iteration++)
    {
        unsigned int current_forward =
            banana_poc_demo_scene_catalog_coherent_transition_signature(2, 3);
        unsigned int current_return =
            banana_poc_demo_scene_catalog_coherent_transition_signature(3, 2);
        unsigned int current_roundtrip = current_forward ^ current_return;

        if (!expect_true("forward signature stays deterministic",
                         current_forward == baseline_forward))
            return 1;

        if (!expect_true("return signature stays deterministic",
                         current_return == baseline_return))
            return 1;

        if (!expect_true("roundtrip signature stays deterministic",
                         current_roundtrip == baseline_roundtrip))
            return 1;
    }

    printf("runtime_demo_scene_catalog_continuity_roundtrip_test: pass\n");
    return 0;
}
