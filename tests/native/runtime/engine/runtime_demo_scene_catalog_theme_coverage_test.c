#include "win32_dx12_poc/scene/demo_scene_catalog.h"

#include <stdio.h>
#include <string.h>

typedef struct ThemeExpectation
{
    int browser_variant;
    const char *theme_id;
    const char *model_prefix;
} ThemeExpectation;

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

static const char *model_for_role(int browser_variant, BananaPocGameplayPlacementRole role)
{
    int placement_count = banana_poc_demo_scene_gameplay_placement_count_for_variant(browser_variant);
    int index = 0;

    for (index = 0; index < placement_count; index++)
    {
        const BananaPocDemoSceneGameplayPlacement *placement =
            banana_poc_demo_scene_gameplay_placement_at(browser_variant, index);

        if (placement && placement->role == role)
            return placement->model_id;
    }

    return NULL;
}

static int variant_matches_theme_expectation(const ThemeExpectation *expected)
{
    int catalog_index = banana_poc_demo_scene_catalog_index_for_browser_variant(expected->browser_variant);
    const BananaPocDemoSceneCatalogEntry *entry =
        banana_poc_demo_scene_catalog_for_browser_variant(expected->browser_variant);
    const BananaPocContinentAssetPack *pack = entry
                                                  ? banana_poc_continent_asset_pack_for_region(entry->primary_region_id)
                                                  : NULL;
    const char *landmark_model = model_for_role(expected->browser_variant,
                                                BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK);
    const char *traversal_model = model_for_role(expected->browser_variant,
                                                 BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_TRAVERSAL);

    if (!expect_true("variant resolves to a catalog index", catalog_index >= 0))
        return 0;

    if (!expect_true("variant catalog entry exists", entry != NULL))
        return 0;

    if (!expect_true("variant catalog entry validates cleanly",
                     banana_poc_demo_scene_catalog_validate_index(catalog_index) ==
                         BANANA_POC_DEMO_SCENE_VALIDATION_OK))
    {
        return 0;
    }

    if (!expect_true("variant resolves an asset pack", pack != NULL))
        return 0;

    if (!expect_true("variant gameplay theme matches expected theme",
                     pack->gameplay_theme_id != NULL &&
                         strcmp(pack->gameplay_theme_id, expected->theme_id) == 0))
    {
        return 0;
    }

    if (!expect_true("variant landmark model is themed",
                     landmark_model != NULL &&
                         strncmp(landmark_model, expected->model_prefix, strlen(expected->model_prefix)) == 0))
    {
        return 0;
    }

    if (!expect_true("variant traversal model is themed",
                     traversal_model != NULL &&
                         strncmp(traversal_model, expected->model_prefix, strlen(expected->model_prefix)) == 0))
    {
        return 0;
    }

    return 1;
}

int main(void)
{
    static const ThemeExpectation k_expected_variants[] = {
        {0, "theme/tropical-coastal", "gameplay/tropical-coastal/"},
        {2, "theme/urban-industrial", "gameplay/urban-industrial/"},
        {1, "theme/rugged-wild", "gameplay/rugged-wild/"},
    };
    const char *landmark_tropical = NULL;
    const char *landmark_urban = NULL;
    const char *landmark_rugged = NULL;
    int index = 0;

    for (index = 0; index < (int)(sizeof(k_expected_variants) / sizeof(k_expected_variants[0])); index++)
    {
        if (!variant_matches_theme_expectation(&k_expected_variants[index]))
            return 1;
    }

    landmark_tropical = model_for_role(0, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK);
    landmark_urban = model_for_role(2, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK);
    landmark_rugged = model_for_role(1, BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK);

    if (!expect_true("tropical and urban landmark sets differ",
                     landmark_tropical != NULL && landmark_urban != NULL &&
                         strcmp(landmark_tropical, landmark_urban) != 0))
    {
        return 1;
    }

    if (!expect_true("urban and rugged landmark sets differ",
                     landmark_urban != NULL && landmark_rugged != NULL &&
                         strcmp(landmark_urban, landmark_rugged) != 0))
    {
        return 1;
    }

    if (!expect_true("rugged and tropical landmark sets differ",
                     landmark_rugged != NULL && landmark_tropical != NULL &&
                         strcmp(landmark_rugged, landmark_tropical) != 0))
    {
        return 1;
    }

    printf("runtime_demo_scene_catalog_theme_coverage_test: pass\n");
    return 0;
}
