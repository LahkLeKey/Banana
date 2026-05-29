#include "win32_dx12_poc/scene/demo_scene_catalog.h"

#include <stdio.h>

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

static unsigned int variant_signature(int browser_variant)
{
    int placement_count = banana_poc_demo_scene_gameplay_placement_count_for_variant(browser_variant);
    unsigned int hash = banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(browser_variant);
    int index = 0;

    hash = hash_string(hash, banana_poc_demo_scene_catalog_gameplay_theme_for_variant(browser_variant));

    for (index = 0; index < placement_count; index++)
    {
        const BananaPocDemoSceneGameplayPlacement *placement =
            banana_poc_demo_scene_gameplay_placement_at(browser_variant, index);

        if (!placement)
            return 0u;

        hash ^= (unsigned int)(placement->role + 33);
        hash = hash_u32(hash);
        hash = hash_string(hash, placement->model_id);
        hash = hash_string(hash, placement->fallback_model_id);
        hash = hash_string(hash, placement->diagnostics_tag);
    }

    hash ^= (unsigned int)(placement_count + 911);
    hash ^= (unsigned int)(browser_variant + 1237);
    return hash_u32(hash);
}

int main(void)
{
    static const int k_variants[] = {0, 1, 2, 3, 6};
    unsigned int baseline[5] = {0u, 0u, 0u, 0u, 0u};
    int variant_count = (int)(sizeof(k_variants) / sizeof(k_variants[0]));
    int iteration = 0;
    int index = 0;

    for (index = 0; index < variant_count; index++)
    {
        baseline[index] = variant_signature(k_variants[index]);

        if (!expect_true("baseline signature exists", baseline[index] != 0u))
            return 1;
    }

    for (iteration = 0; iteration < 16; iteration++)
    {
        for (index = 0; index < variant_count; index++)
        {
            unsigned int current = variant_signature(k_variants[index]);

            if (!expect_true("variant signature remains deterministic", current == baseline[index]))
                return 1;
        }
    }

    if (!expect_true("tropical and rugged signatures differ", baseline[0] != baseline[1]))
        return 1;

    if (!expect_true("tropical and urban station signatures differ", baseline[0] != baseline[2]))
        return 1;

    if (!expect_true("urban station and urban corridor signatures differ", baseline[2] != baseline[3]))
        return 1;

    if (!expect_true("sentience showcase signature differs from urban station", baseline[4] != baseline[2]))
        return 1;

    printf("runtime_demo_scene_catalog_theme_determinism_test: pass\n");
    return 0;
}
