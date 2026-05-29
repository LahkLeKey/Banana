#include "win32_dx12_poc/scene/demo_scene_catalog.h"

#include <stdio.h>
#include <string.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    BananaPocCoherentWorldProfile disabled_profile;

    if (!expect_true("disabled coherent profile builds",
                     banana_poc_demo_scene_catalog_build_coherent_profile(4, &disabled_profile)))
    {
        return 1;
    }

    if (!expect_true("disabled coherent profile is marked disabled",
                     disabled_profile.validation_status == BANANA_POC_DEMO_SCENE_VALIDATION_DISABLED))
    {
        return 1;
    }

    if (!expect_true("disabled coherent profile is not consistent",
                     !banana_poc_demo_scene_catalog_coherent_profile_consistent(&disabled_profile)))
    {
        return 1;
    }

    if (!expect_true("disabled diagnostics message is stable",
                     strcmp(banana_poc_demo_scene_catalog_validation_message(disabled_profile.validation_status),
                            "scene-disabled") == 0))
    {
        return 1;
    }

    printf("runtime_demo_scene_catalog_coherent_disabled_profile_test: pass\n");
    return 0;
}
