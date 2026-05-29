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
    int disabled_index = banana_poc_demo_scene_catalog_index_for_browser_variant(4);
    const BananaPocDemoSceneCatalogEntry *disabled_entry =
        banana_poc_demo_scene_catalog_for_browser_variant(4);
    BananaPocDemoSceneValidationStatus disabled_status =
        banana_poc_demo_scene_catalog_validate_index(disabled_index);

    if (!expect_true("disabled variant resolves to a catalog index", disabled_index >= 0))
        return 1;

    if (!expect_true("disabled variant entry exists", disabled_entry != NULL))
        return 1;

    if (!expect_true("disabled variant is marked disabled", disabled_entry->enabled == 0))
        return 1;

    if (!expect_true("disabled entry diagnostics status is scene-disabled",
                     disabled_status == BANANA_POC_DEMO_SCENE_VALIDATION_DISABLED))
    {
        return 1;
    }

    if (!expect_true("disabled entry diagnostics message is stable",
                     strcmp(banana_poc_demo_scene_catalog_validation_message(disabled_status),
                            "scene-disabled") == 0))
    {
        return 1;
    }

    if (!expect_true("disabled entry still reports gameplay mismatch in gameplay validator",
                     banana_poc_demo_scene_catalog_validate_gameplay_for_variant(4) ==
                         BANANA_POC_DEMO_SCENE_VALIDATION_GAMEPLAY_MODEL_MISMATCH))
    {
        return 1;
    }

    if (!expect_true("disabled variant has no deterministic gameplay placements",
                     banana_poc_demo_scene_gameplay_placement_count_for_variant(4) == 0))
    {
        return 1;
    }

    printf("runtime_demo_scene_catalog_disabled_diagnostics_test: pass\n");
    return 0;
}
