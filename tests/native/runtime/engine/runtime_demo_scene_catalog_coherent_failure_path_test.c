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
    BananaPocCoherentWorldProfile baseline;
    BananaPocCoherentWorldProfile mutated;

    if (!expect_true("baseline coherent profile builds",
                     banana_poc_demo_scene_catalog_build_coherent_profile(2, &baseline)))
    {
        return 1;
    }

    if (!expect_true("baseline profile starts coherent",
                     banana_poc_demo_scene_catalog_coherent_profile_consistent(&baseline)))
    {
        return 1;
    }

    mutated = baseline;
    mutated.route_id = RUNTIME_TERRAIN_BANANA_LINE_ROUTE_INVALID;
    if (!expect_true("invalid route breaks coherence",
                     !banana_poc_demo_scene_catalog_coherent_profile_consistent(&mutated)))
    {
        return 1;
    }

    mutated = baseline;
    mutated.anchor_county_id = "invalid-anchor";
    if (!expect_true("invalid county anchor breaks coherence",
                     !banana_poc_demo_scene_catalog_coherent_profile_consistent(&mutated)))
    {
        return 1;
    }

    mutated = baseline;
    mutated.asset_pack_id = "continent/not-registered";
    if (!expect_true("unregistered asset pack breaks coherence",
                     !banana_poc_demo_scene_catalog_coherent_profile_consistent(&mutated)))
    {
        return 1;
    }

    mutated = baseline;
    mutated.browser_variant = 3;
    if (!expect_true("cross-variant identity tampering breaks coherence",
                     !banana_poc_demo_scene_catalog_coherent_profile_consistent(&mutated)))
    {
        return 1;
    }

    mutated = baseline;
    mutated.scene_key = "banana-mainline-metro-crescent";
    if (!expect_true("scene key tampering breaks coherence",
                     !banana_poc_demo_scene_catalog_coherent_profile_consistent(&mutated)))
    {
        return 1;
    }

    mutated = baseline;
    mutated.bootstrap_signature ^= 0x55u;
    if (!expect_true("bootstrap signature tampering breaks coherence",
                     !banana_poc_demo_scene_catalog_coherent_profile_consistent(&mutated)))
    {
        return 1;
    }

    if (!expect_true("unknown scene diagnostics are stable",
                     strcmp(banana_poc_demo_scene_catalog_validation_message(
                                BANANA_POC_DEMO_SCENE_VALIDATION_UNKNOWN_SCENE),
                            "unknown-scene") == 0))
    {
        return 1;
    }

    if (!expect_true("route mismatch diagnostics are stable",
                     strcmp(banana_poc_demo_scene_catalog_validation_message(
                                BANANA_POC_DEMO_SCENE_VALIDATION_ROUTE_MISMATCH),
                            "route-anchor-mismatch") == 0))
    {
        return 1;
    }

    if (!expect_true("anchor mismatch diagnostics are stable",
                     strcmp(banana_poc_demo_scene_catalog_validation_message(
                                BANANA_POC_DEMO_SCENE_VALIDATION_ANCHOR_MISMATCH),
                            "county-anchor-mismatch") == 0))
    {
        return 1;
    }

    printf("runtime_demo_scene_catalog_coherent_failure_path_test: pass\n");
    return 0;
}
