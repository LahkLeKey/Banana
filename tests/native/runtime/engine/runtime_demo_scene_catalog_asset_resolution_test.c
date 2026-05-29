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

static unsigned int hash_u32(unsigned int value)
{
    value ^= value >> 16;
    value *= 0x7feb352du;
    value ^= value >> 15;
    value *= 0x846ca68bu;
    value ^= value >> 16;
    return value;
}

static unsigned int hash_string(unsigned int seed, const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    unsigned int hash = seed;

    if (!cursor)
        return hash_u32(hash ^ 0x9e3779b9u);

    while (*cursor)
    {
        hash ^= (unsigned int)(*cursor++);
        hash *= 16777619u;
    }

    return hash_u32(hash);
}

static unsigned int placement_signature_for_variant(int browser_variant)
{
    int placement_count = banana_poc_demo_scene_gameplay_placement_count_for_variant(browser_variant);
    int index = 0;
    unsigned int hash = 2166136261u;

    for (index = 0; index < placement_count; index++)
    {
        const BananaPocDemoSceneGameplayPlacement *placement =
            banana_poc_demo_scene_gameplay_placement_at(browser_variant, index);

        if (!placement)
            return 0u;

        hash ^= (unsigned int)(placement->role + 1);
        hash = hash_u32(hash);
        hash = hash_string(hash, placement->model_id);
        hash = hash_string(hash, placement->fallback_model_id);
        hash = hash_string(hash, placement->diagnostics_tag);
        hash = hash_string(hash, (placement->x == 0.0f) ? "0" : "x");
        hash = hash_string(hash, (placement->y == 0.0f) ? "0" : "y");
        hash = hash_string(hash, (placement->z == 0.0f) ? "0" : "z");
    }

    hash ^= (unsigned int)(placement_count + 4099);
    return hash_u32(hash);
}

int main(void)
{
    int variant = 0;
    int placement_count = banana_poc_demo_scene_gameplay_placement_count_for_variant(variant);
    unsigned int sig_a = placement_signature_for_variant(variant);
    unsigned int sig_b = placement_signature_for_variant(variant);
    int index = 0;
    int saw_reference_banana = 0;
    int saw_reference_bean = 0;
    int saw_landmark = 0;
    int saw_traversal = 0;
    int saw_banana_team = 0;
    int saw_bean_team = 0;

    if (!expect_true("variant 0 has at least three placements", placement_count >= 3))
        return 1;

    if (!expect_true("variant 0 placement signature is deterministic", sig_a != 0u && sig_a == sig_b))
        return 1;

    for (index = 0; index < placement_count; index++)
    {
        const BananaPocDemoSceneGameplayPlacement *placement =
            banana_poc_demo_scene_gameplay_placement_at(variant, index);

        if (!expect_true("placement exists", placement != NULL))
            return 1;

        if (!expect_true("placement has model or fallback",
                         (placement->model_id && placement->model_id[0] != '\0') ||
                             (placement->fallback_model_id && placement->fallback_model_id[0] != '\0')))
        {
            return 1;
        }

        if (placement->model_id && strcmp(placement->model_id, "gameplay/reference/banana-basic-v1") == 0)
            saw_reference_banana = 1;

        if (placement->model_id && strcmp(placement->model_id, "gameplay/reference/banana-bean-green-v1") == 0)
            saw_reference_bean = 1;

        if (placement->team == BANANA_POC_GAMEPLAY_TEAM_BANANA)
            saw_banana_team = 1;

        if (placement->team == BANANA_POC_GAMEPLAY_TEAM_BEAN)
            saw_bean_team = 1;

        if (placement->role == BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_LANDMARK)
            saw_landmark = 1;

        if (placement->role == BANANA_POC_GAMEPLAY_PLACEMENT_ROLE_TRAVERSAL)
            saw_traversal = 1;
    }

    if (!expect_true("variant 0 includes banana reference model", saw_reference_banana))
        return 1;

    if (!expect_true("variant 0 includes green bean model", saw_reference_bean))
        return 1;

    if (!expect_true("variant 0 includes banana team placement", saw_banana_team))
        return 1;

    if (!expect_true("variant 0 includes bean team placement", saw_bean_team))
        return 1;

    if (!expect_true("banana and bean teams are enemies",
                     banana_poc_demo_scene_gameplay_teams_are_enemies(BANANA_POC_GAMEPLAY_TEAM_BANANA,
                                                                      BANANA_POC_GAMEPLAY_TEAM_BEAN)))
    {
        return 1;
    }

    if (!expect_true("enemy relation is symmetric",
                     banana_poc_demo_scene_gameplay_teams_are_enemies(BANANA_POC_GAMEPLAY_TEAM_BEAN,
                                                                      BANANA_POC_GAMEPLAY_TEAM_BANANA)))
    {
        return 1;
    }

    if (!expect_true("same team is not hostile",
                     !banana_poc_demo_scene_gameplay_teams_are_enemies(BANANA_POC_GAMEPLAY_TEAM_BANANA,
                                                                       BANANA_POC_GAMEPLAY_TEAM_BANANA)))
    {
        return 1;
    }

    if (!expect_true("variant 0 includes landmark role", saw_landmark))
        return 1;

    if (!expect_true("variant 0 includes traversal role", saw_traversal))
        return 1;

    {
        int showcase_variant = 6;
        int showcase_placement_count =
            banana_poc_demo_scene_gameplay_placement_count_for_variant(showcase_variant);
        int showcase_index = banana_poc_demo_scene_catalog_index_for_browser_variant(showcase_variant);
        int placement_index = 0;
        int saw_flank = 0;
        int saw_regroup = 0;
        int saw_envoy = 0;

        if (!expect_true("showcase variant exists in catalog", showcase_index >= 0))
            return 1;

        if (!expect_true("showcase variant is launchable",
                         banana_poc_demo_scene_catalog_validate_index(showcase_index) ==
                             BANANA_POC_DEMO_SCENE_VALIDATION_OK))
        {
            return 1;
        }

        if (!expect_true("showcase variant has sentience placements", showcase_placement_count >= 4))
            return 1;

        for (placement_index = 0; placement_index < showcase_placement_count; placement_index++)
        {
            const BananaPocDemoSceneGameplayPlacement *placement =
                banana_poc_demo_scene_gameplay_placement_at(showcase_variant, placement_index);
            const char *model_id = placement ? placement->model_id : NULL;

            if (!expect_true("showcase placement exists", placement != NULL))
                return 1;

            if (!model_id)
                continue;

            if (strstr(model_id, "flank") != NULL)
                saw_flank = 1;
            if (strstr(model_id, "regroup") != NULL)
                saw_regroup = 1;
            if (strstr(model_id, "envoy") != NULL)
                saw_envoy = 1;
        }

        if (!expect_true("showcase variant includes flank model", saw_flank))
            return 1;
        if (!expect_true("showcase variant includes regroup model", saw_regroup))
            return 1;
        if (!expect_true("showcase variant includes envoy model", saw_envoy))
            return 1;
    }

    printf("runtime_demo_scene_catalog_asset_resolution_test: pass\n");
    return 0;
}