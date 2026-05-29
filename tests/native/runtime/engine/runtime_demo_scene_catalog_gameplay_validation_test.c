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
    if (!expect_true("enabled continent slice validates gameplay placements",
                     banana_poc_demo_scene_catalog_validate_gameplay_for_variant(0) ==
                         BANANA_POC_DEMO_SCENE_VALIDATION_OK))
    {
        return 1;
    }

    if (!expect_true("enabled corridor slice validates gameplay placements",
                     banana_poc_demo_scene_catalog_validate_gameplay_for_variant(3) ==
                         BANANA_POC_DEMO_SCENE_VALIDATION_OK))
    {
        return 1;
    }

    if (!expect_true("disabled lab slice exposes gameplay mismatch",
                     banana_poc_demo_scene_catalog_validate_gameplay_for_variant(4) ==
                         BANANA_POC_DEMO_SCENE_VALIDATION_GAMEPLAY_MODEL_MISMATCH))
    {
        return 1;
    }

    if (!expect_true("disabled preview slice exposes gameplay mismatch",
                     banana_poc_demo_scene_catalog_validate_gameplay_for_variant(5) ==
                         BANANA_POC_DEMO_SCENE_VALIDATION_GAMEPLAY_MODEL_MISMATCH))
    {
        return 1;
    }

    printf("runtime_demo_scene_catalog_gameplay_validation_test: pass\n");
    return 0;
}