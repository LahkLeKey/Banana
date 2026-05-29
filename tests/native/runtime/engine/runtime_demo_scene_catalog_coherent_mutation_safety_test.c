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
    BananaPocCoherentWorldProfile before_profile;
    BananaPocCoherentWorldProfile after_profile;

    banana_poc_demo_scene_gameplay_model_clear_overrides();

    if (!expect_true("baseline coherent profile builds",
                     banana_poc_demo_scene_catalog_build_coherent_profile(2, &before_profile)))
    {
        return 1;
    }

    if (!expect_true("baseline profile is coherent",
                     banana_poc_demo_scene_catalog_coherent_profile_consistent(&before_profile)))
    {
        return 1;
    }

    if (!expect_true("model override can register new coherent world binding",
                     banana_poc_demo_scene_gameplay_model_override(
                         "urban-station-landmark",
                         "gameplay/urban-industrial/signal-gantry-v1")))
    {
        return 1;
    }

    if (!expect_true("profile rebuilds after mutation",
                     banana_poc_demo_scene_catalog_build_coherent_profile(2, &after_profile)))
    {
        return 1;
    }

    if (!expect_true("mutated profile remains coherent",
                     banana_poc_demo_scene_catalog_coherent_profile_consistent(&after_profile)))
    {
        return 1;
    }

    if (!expect_true("scene identity remains stable after model mutation",
                     before_profile.scene_key != NULL && after_profile.scene_key != NULL &&
                         strcmp(before_profile.scene_key, after_profile.scene_key) == 0))
    {
        return 1;
    }

    if (!expect_true("asset pack remains stable after model mutation",
                     before_profile.asset_pack_id != NULL && after_profile.asset_pack_id != NULL &&
                         strcmp(before_profile.asset_pack_id, after_profile.asset_pack_id) == 0))
    {
        return 1;
    }

    if (!expect_true("placement count remains stable after model mutation",
                     before_profile.gameplay_placement_count == after_profile.gameplay_placement_count))
    {
        return 1;
    }

    banana_poc_demo_scene_gameplay_model_clear_overrides();

    printf("runtime_demo_scene_catalog_coherent_mutation_safety_test: pass\n");
    return 0;
}
