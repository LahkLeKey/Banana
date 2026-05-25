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
    int profile_count = runtime_terrain_static_mesh_profile_count();
    int pack_count = banana_poc_continent_asset_pack_count();
    int scene_count = banana_poc_demo_scene_catalog_count();
    int saw_continent_scene = 0;
    int saw_banana_line_scene = 0;
    int index = 0;

    if (!expect_true("continent asset pack count matches canonical region profile count",
                     pack_count == profile_count))
    {
        return 1;
    }

    for (index = 0; index < profile_count; index++)
    {
        const RuntimeTerrainStaticMeshProfile *profile =
            runtime_terrain_static_mesh_profile_at_index(index);
        const BananaPocContinentAssetPack *pack =
            banana_poc_continent_asset_pack_for_region(profile ? profile->region_id : RUNTIME_TERRAIN_STATIC_MESH_REGION_INVALID);

        if (!expect_true("profile exists", profile != NULL))
            return 1;

        if (!expect_true("every canonical region has an asset pack", pack != NULL))
            return 1;

        if (!expect_true("asset pack region slug matches canonical profile slug",
                         pack->region_slug != NULL && profile->region_slug != NULL &&
                             strcmp(pack->region_slug, profile->region_slug) == 0))
        {
            return 1;
        }
    }

    if (!expect_true("scene catalog has entries", scene_count > 0))
        return 1;

    if (!expect_true("scene catalog has enabled entry",
                     banana_poc_demo_scene_catalog_is_launchable_index(
                         banana_poc_demo_scene_catalog_first_enabled_index())))
    {
        return 1;
    }

    for (index = 0; index < scene_count; index++)
    {
        const BananaPocDemoSceneCatalogEntry *entry = banana_poc_demo_scene_catalog_at_index(index);
        BananaPocDemoSceneValidationStatus validation =
            banana_poc_demo_scene_catalog_validate_index(index);

        if (!expect_true("catalog entry exists", entry != NULL))
            return 1;

        if (entry->kind == BANANA_POC_DEMO_SCENE_KIND_CONTINENT)
            saw_continent_scene = 1;

        if (entry->kind == BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_STATION ||
            entry->kind == BANANA_POC_DEMO_SCENE_KIND_BANANA_LINE_CORRIDOR)
        {
            saw_banana_line_scene = 1;
        }

        if (entry->enabled)
        {
            unsigned int sig_a =
                banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(entry->browser_variant);
            unsigned int sig_b =
                banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(entry->browser_variant);
            const BananaPocContinentAssetPack *pack =
                banana_poc_continent_asset_pack_for_region(entry->primary_region_id);

            if (!expect_true("enabled entry validates cleanly",
                             validation == BANANA_POC_DEMO_SCENE_VALIDATION_OK))
            {
                return 1;
            }

            if (!expect_true("launchable entry has deterministic bootstrap signature", sig_a == sig_b && sig_a != 0u))
                return 1;

            if (!expect_true("launchable entry references registered asset pack",
                             banana_poc_continent_asset_pack_is_registered(entry->asset_pack_id)))
            {
                return 1;
            }

            if (!expect_true("launchable entry references non-legacy asset pack",
                             pack != NULL && pack->legacy_placeholder == 0))
            {
                return 1;
            }
        }
        else
        {
            if (!expect_true("disabled entries report disabled validation",
                             validation == BANANA_POC_DEMO_SCENE_VALIDATION_DISABLED))
            {
                return 1;
            }
        }
    }

    if (!expect_true("catalog includes at least one continent scene", saw_continent_scene))
        return 1;

    if (!expect_true("catalog includes at least one banana line scene", saw_banana_line_scene))
        return 1;

    printf("runtime_demo_scene_catalog_bootstrap_test: pass\n");
    return 0;
}
