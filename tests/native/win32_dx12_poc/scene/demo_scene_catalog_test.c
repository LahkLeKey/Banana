#include "win32_dx12_poc/scene/demo_scene_catalog.h"

#include "../../runtime/support/test_support.h"

static void test_catalog_index_and_lookup_paths(void **state)
{
    (void)state;
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_count() > 0,
                            "catalog count must be positive");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_at_index(-1) == NULL,
                            "negative catalog index must return NULL");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_at_index(999) == NULL,
                            "out-of-range catalog index must return NULL");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_for_browser_variant(0) != NULL,
                            "known browser variant must resolve to catalog entry");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_for_browser_variant(999) == NULL,
                            "unknown browser variant must return NULL");
    BANANA_TEST_ASSERT_INT_EQ(banana_poc_demo_scene_catalog_clamp_index(-5), 0,
                              "negative requested index must clamp to zero");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_first_enabled_index() >= 0,
                            "first enabled index must be non-negative");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_is_launchable_index(0),
                            "first enabled catalog entry should be launchable");
}

static void test_catalog_validation_and_profile_paths(void **state)
{
    (void)state;
    BananaPocCoherentWorldProfile profile = {0};
    BANANA_TEST_ASSERT_INT_EQ(banana_poc_demo_scene_catalog_validate_index(-1), BANANA_POC_DEMO_SCENE_VALIDATION_UNKNOWN_SCENE,
                              "invalid catalog index must return unknown-scene validation");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_validation_message(BANANA_POC_DEMO_SCENE_VALIDATION_OK) != NULL,
                            "validation message for OK must not be null");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_display_name_for_variant(0) != NULL,
                            "display name for known variant must not be null");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_gameplay_theme_for_variant(0) != NULL,
                            "gameplay theme for known variant must not be null");
    float radius = 0.f; int reinforcements = 0; int cap = 0;
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_war_policy_for_variant(0, &radius, &reinforcements, &cap),
                            "war policy lookup must succeed for known variant");
    BANANA_TEST_ASSERT_TRUE(radius > 0.f && reinforcements > 0 && cap > 0,
                            "war policy values must be positive");
    BANANA_TEST_ASSERT_TRUE(!banana_poc_demo_scene_catalog_war_policy_for_variant(999, &radius, &reinforcements, &cap),
                            "war policy lookup must fail for unknown variant");

    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_build_coherent_profile(0, &profile),
                            "coherent profile build must succeed for known variant");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_coherent_profile_consistent(&profile),
                            "built coherent profile must validate as consistent");
    BANANA_TEST_ASSERT_TRUE(!banana_poc_demo_scene_catalog_coherent_profile_consistent(NULL),
                            "null profile must be inconsistent");
    BANANA_TEST_ASSERT_TRUE(!banana_poc_demo_scene_catalog_build_coherent_profile(999, &profile),
                            "coherent profile build must fail for unknown variant");
}

static void test_catalog_transition_and_gameplay_helpers(void **state)
{
    (void)state;
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_coherent_transition_connected(2, 3),
                            "known adjacent corridor variants must be connected");
    BANANA_TEST_ASSERT_TRUE(!banana_poc_demo_scene_catalog_coherent_transition_connected(0, 999),
                            "unknown variant must break coherent transition connectivity");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_coherent_transition_signature(2, 3) != 0u,
                            "connected variants must produce non-zero transition signature");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_coherent_transition_signature(0, 999) == 0u,
                            "disconnected variants must produce zero transition signature");

    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_gameplay_team_label(BANANA_POC_GAMEPLAY_TEAM_BANANA) != NULL,
                            "banana gameplay team label must not be null");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_gameplay_teams_are_enemies(BANANA_POC_GAMEPLAY_TEAM_BANANA, BANANA_POC_GAMEPLAY_TEAM_BEAN),
                            "banana and bean teams must be enemies");
    BANANA_TEST_ASSERT_TRUE(!banana_poc_demo_scene_gameplay_teams_are_enemies(BANANA_POC_GAMEPLAY_TEAM_BANANA, BANANA_POC_GAMEPLAY_TEAM_BANANA),
                            "same teams must not be enemies");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_gameplay_model_id_for_key("reference-banana") != NULL,
                            "known gameplay model key must resolve to model id");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_gameplay_model_override("reference-banana", "override/model") ,
                            "model override should succeed for known key");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_gameplay_model_id_for_key("reference-banana") != NULL,
                            "overridden model key must still resolve to model id");
    banana_poc_demo_scene_gameplay_model_clear_overrides();
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_vector_profile_for_model_id("gameplay/reference/banana-basic-v1", NULL, NULL, NULL, NULL, NULL) == 0,
                            "vector profile helper must reject null output pointers");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(0) != 0u,
                            "known variant must produce non-zero bootstrap signature");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_gameplay_placement_count_for_variant(0) > 0,
                            "known variant must expose gameplay placements");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_gameplay_placement_at(0, 0) != NULL,
                            "placement lookup for known variant/index must succeed");
    BANANA_TEST_ASSERT_TRUE(banana_poc_demo_scene_gameplay_placement_at(999, 0) == NULL,
                            "placement lookup for unknown variant must fail");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_catalog_index_and_lookup_paths),
    BANANA_TEST_CASE(test_catalog_validation_and_profile_paths),
    BANANA_TEST_CASE(test_catalog_transition_and_gameplay_helpers)
)
