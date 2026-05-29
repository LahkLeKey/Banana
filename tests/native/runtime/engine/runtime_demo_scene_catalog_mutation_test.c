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

static const BananaPocDemoSceneGameplayPlacement *find_role(int browser_variant,
                                                             BananaPocGameplayPlacementRole role)
{
    int count = banana_poc_demo_scene_gameplay_placement_count_for_variant(browser_variant);
    int index = 0;

    for (index = 0; index < count; index++)
    {
        const BananaPocDemoSceneGameplayPlacement *placement =
            banana_poc_demo_scene_gameplay_placement_at(browser_variant, index);
        if (placement && placement->role == role)
            return placement;
    }

    return NULL;
}

int main(void)
{
    const BananaPocDemoSceneCatalogEntry *variant0_entry =
        banana_poc_demo_scene_catalog_for_browser_variant(0);
    const BananaPocDemoSceneCatalogEntry *variant2_entry =
        banana_poc_demo_scene_catalog_for_browser_variant(2);
    const BananaPocDemoSceneGameplayPlacement *variant0_before = NULL;
    const BananaPocDemoSceneGameplayPlacement *variant0_after = NULL;
    const BananaPocDemoSceneGameplayPlacement *variant2_before = NULL;
    const BananaPocDemoSceneGameplayPlacement *variant2_after = NULL;
    const BananaPocDemoSceneGameplayPlacement *variant2_restored = NULL;

    banana_poc_demo_scene_gameplay_model_clear_overrides();

    variant0_before = find_role(0, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK);
    variant2_before = find_role(2, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK);

    if (!expect_true("baseline variant 0 landmark exists", variant0_before != NULL))
        return 1;

    if (!expect_true("baseline variant 2 landmark exists", variant2_before != NULL))
        return 1;

    if (!expect_true("baseline variant 2 landmark uses urban station model",
                     strcmp(variant2_before->model_id, "gameplay/urban-industrial/rail-hub-v1") == 0))
    {
        return 1;
    }

    if (!expect_true("new gameplay model key can be registered",
                     banana_poc_demo_scene_gameplay_model_override(
                         "urban-station-landmark",
                         "gameplay/urban-industrial/signal-gantry-v1")))
    {
        return 1;
    }

    variant2_after = find_role(2, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK);
    variant0_after = find_role(0, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK);

    if (!expect_true("variant 2 landmark switched via central model catalog mutation",
                     variant2_after != NULL &&
                         strcmp(variant2_after->model_id, "gameplay/urban-industrial/signal-gantry-v1") == 0))
    {
        return 1;
    }

    if (!expect_true("unrelated scene bootstrap metadata remains stable",
                     variant0_entry != NULL && variant2_entry != NULL &&
                         strcmp(variant0_entry->scene_key, "continent-stem-territories") == 0 &&
                         strcmp(variant2_entry->scene_key, "banana-mainline-neo-musa") == 0))
    {
        return 1;
    }

    if (!expect_true("unrelated variant landmark stays unchanged",
                     variant0_after != NULL &&
                         strcmp(variant0_after->model_id, variant0_before->model_id) == 0))
    {
        return 1;
    }

    if (!expect_true("mutated variant still validates",
                     banana_poc_demo_scene_catalog_validate_index(
                         banana_poc_demo_scene_catalog_index_for_browser_variant(2)) ==
                         BANANA_POC_DEMO_SCENE_VALIDATION_OK))
    {
        return 1;
    }

    banana_poc_demo_scene_gameplay_model_clear_overrides();
    variant2_restored = find_role(2, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK);

    if (!expect_true("clearing overrides restores baseline model",
                     variant2_restored != NULL &&
                         strcmp(variant2_restored->model_id, "gameplay/urban-industrial/rail-hub-v1") == 0))
    {
        return 1;
    }

    printf("runtime_demo_scene_catalog_mutation_test: pass\n");
    return 0;
}
