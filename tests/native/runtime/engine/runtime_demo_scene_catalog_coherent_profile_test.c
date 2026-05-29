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
    static const int k_enabled_variants[] = {0, 1, 2, 3, 6};
    int index = 0;

    for (index = 0; index < (int)(sizeof(k_enabled_variants) / sizeof(k_enabled_variants[0])); index++)
    {
        BananaPocCoherentWorldProfile profile;
        int variant = k_enabled_variants[index];

        if (!expect_true("enabled variant builds coherent profile",
                         banana_poc_demo_scene_catalog_build_coherent_profile(variant, &profile)))
        {
            return 1;
        }

        if (!expect_true("enabled profile has ok validation status",
                         profile.validation_status == BANANA_POC_DEMO_SCENE_VALIDATION_OK))
        {
            return 1;
        }

        if (!expect_true("enabled profile is coherent", banana_poc_demo_scene_catalog_coherent_profile_consistent(&profile)))
            return 1;

        if (!expect_true("enabled profile has deterministic signature", profile.bootstrap_signature != 0u))
            return 1;

        if (!expect_true("enabled profile has gameplay placements", profile.gameplay_placement_count >= 2))
            return 1;
    }

    {
        BananaPocCoherentWorldProfile disabled_profile;

        if (!expect_true("disabled variant builds profile",
                         banana_poc_demo_scene_catalog_build_coherent_profile(4, &disabled_profile)))
        {
            return 1;
        }

        if (!expect_true("disabled profile tracks disabled status",
                         disabled_profile.validation_status == BANANA_POC_DEMO_SCENE_VALIDATION_DISABLED))
        {
            return 1;
        }

        if (!expect_true("disabled profile is not coherent",
                         !banana_poc_demo_scene_catalog_coherent_profile_consistent(&disabled_profile)))
        {
            return 1;
        }
    }

    printf("runtime_demo_scene_catalog_coherent_profile_test: pass\n");
    return 0;
}
