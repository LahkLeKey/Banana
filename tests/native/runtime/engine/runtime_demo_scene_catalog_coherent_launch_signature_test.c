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
    BananaPocCoherentWorldProfile profile_a;
    BananaPocCoherentWorldProfile profile_b;

    if (!expect_true("variant 2 coherent profile builds",
                     banana_poc_demo_scene_catalog_build_coherent_profile(2, &profile_a)))
    {
        return 1;
    }

    if (!expect_true("variant 2 coherent profile rebuilds deterministically",
                     banana_poc_demo_scene_catalog_build_coherent_profile(2, &profile_b)))
    {
        return 1;
    }

    if (!expect_true("profile remains valid", profile_a.validation_status == BANANA_POC_DEMO_SCENE_VALIDATION_OK))
        return 1;

    if (!expect_true("profile remains coherent", banana_poc_demo_scene_catalog_coherent_profile_consistent(&profile_a)))
        return 1;

    if (!expect_true("scene key is stable",
                     profile_a.scene_key != NULL && profile_b.scene_key != NULL &&
                         strcmp(profile_a.scene_key, profile_b.scene_key) == 0))
    {
        return 1;
    }

    if (!expect_true("theme is stable",
                     profile_a.gameplay_theme_id != NULL && profile_b.gameplay_theme_id != NULL &&
                         strcmp(profile_a.gameplay_theme_id, profile_b.gameplay_theme_id) == 0))
    {
        return 1;
    }

    if (!expect_true("placement count is stable",
                     profile_a.gameplay_placement_count == profile_b.gameplay_placement_count))
    {
        return 1;
    }

    if (!expect_true("bootstrap signature is stable and non-zero",
                     profile_a.bootstrap_signature != 0u &&
                         profile_a.bootstrap_signature == profile_b.bootstrap_signature))
    {
        return 1;
    }

    printf("runtime_demo_scene_catalog_coherent_launch_signature_test: pass\n");
    return 0;
}
