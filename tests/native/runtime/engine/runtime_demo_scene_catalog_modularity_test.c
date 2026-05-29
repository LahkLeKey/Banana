#include "win32_dx12_poc/scene/demo_scene_catalog.h"

#include <stdio.h>
#include <string.h>

typedef struct RuntimeDemoSceneBaselineEntry
{
    int browser_variant;
    int enabled;
    BananaPocDemoSceneKind kind;
    const char *scene_key;
} RuntimeDemoSceneBaselineEntry;

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

int main(void)
{
    static const RuntimeDemoSceneBaselineEntry k_baseline[] = {
        {0, 1, BANANA_POC_DEMO_SCENE_KIND_CONTINENT, "continent-stem-territories"},
        {1, 1, BANANA_POC_DEMO_SCENE_KIND_CONTINENT, "continent-north-crown"},
        {2, 1, BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_STATION, "banana-mainline-neo-musa"},
        {3, 1, BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_CORRIDOR, "banana-mainline-metro-crescent"},
        {4, 0, BANANA_POC_DEMO_SCENE_KIND_LAB, "iron-spine-freight-lab"},
        {5, 0, BANANA_POC_DEMO_SCENE_KIND_CONTINENT, "frost-peel-preview"},
        {6, 1, BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_STATION, "banana-war-sentience-showcase"},
    };

    int scene_count = banana_poc_demo_scene_catalog_count();
    int baseline_count = (int)(sizeof(k_baseline) / sizeof(k_baseline[0]));
    int index = 0;
    int other = 0;

    if (!expect_true("catalog has baseline entries", scene_count >= baseline_count))
        return 1;

    /* Baseline entries are index-stable so new slices can be appended without rewriting old ones. */
    for (index = 0; index < baseline_count; index++)
    {
        const BananaPocDemoSceneCatalogEntry *entry =
            banana_poc_demo_scene_catalog_at_index(index);

        if (!expect_true("baseline entry exists", entry != NULL))
            return 1;

        if (!expect_true("baseline scene key matches",
                         entry->scene_key != NULL &&
                             strcmp(entry->scene_key, k_baseline[index].scene_key) == 0))
        {
            return 1;
        }

        if (!expect_true("baseline browser variant matches",
                         entry->browser_variant == k_baseline[index].browser_variant))
        {
            return 1;
        }

        if (!expect_true("baseline enabled flag matches",
                         entry->enabled == k_baseline[index].enabled))
        {
            return 1;
        }

        if (!expect_true("baseline kind matches", entry->kind == k_baseline[index].kind))
            return 1;

        if (!expect_true("variant resolves back to baseline index",
                         banana_poc_demo_scene_catalog_index_for_browser_variant(
                             entry->browser_variant) == index))
        {
            return 1;
        }
    }

    for (index = 0; index < scene_count; index++)
    {
        const BananaPocDemoSceneCatalogEntry *entry =
            banana_poc_demo_scene_catalog_at_index(index);

        if (!expect_true("catalog entry exists", entry != NULL))
            return 1;

        if (!expect_true("catalog scene key is present",
                         entry->scene_key != NULL && entry->scene_key[0] != '\0'))
        {
            return 1;
        }

        if (!expect_true("catalog variant has deterministic signature",
                         banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(
                             entry->browser_variant) != 0u))
        {
            return 1;
        }

        if (!expect_true("catalog variant index round-trips",
                         banana_poc_demo_scene_catalog_index_for_browser_variant(
                             entry->browser_variant) == index))
        {
            return 1;
        }

        for (other = index + 1; other < scene_count; other++)
        {
            const BananaPocDemoSceneCatalogEntry *other_entry =
                banana_poc_demo_scene_catalog_at_index(other);

            if (!expect_true("other catalog entry exists", other_entry != NULL))
                return 1;

            if (!expect_true("browser variants are unique",
                             entry->browser_variant != other_entry->browser_variant))
            {
                return 1;
            }
        }
    }

    printf("runtime_demo_scene_catalog_modularity_test: pass\n");
    return 0;
}
