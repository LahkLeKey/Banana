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
    unsigned int reference =
        banana_poc_demo_scene_catalog_coherent_transition_signature(2, 3);
    int iteration = 0;

    if (!expect_true("reference transition signature exists", reference != 0u))
        return 1;

    for (iteration = 0; iteration < 10; iteration++)
    {
        unsigned int loop_signature =
            banana_poc_demo_scene_catalog_coherent_transition_signature(2, 3);

        if (!expect_true("connected transition signature does not drift",
                         loop_signature == reference))
        {
            return 1;
        }
    }

    if (!expect_true("unconnected transition is reported as disconnected",
                     !banana_poc_demo_scene_catalog_coherent_transition_connected(0, 1)))
    {
        return 1;
    }

    if (!expect_true("unconnected transition has zero signature",
                     banana_poc_demo_scene_catalog_coherent_transition_signature(0, 1) == 0u))
    {
        return 1;
    }

    printf("runtime_demo_scene_catalog_transition_drift_test: pass\n");
    return 0;
}
