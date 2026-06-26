#include "runtime/engine/gameplay/model/gameplay_model_profile.h"

#include "runtime/support/test_support.h"

static int profile_for(const char *model_id,
                       float *radius,
                       float *length,
                       float *curve,
                       float *taper,
                       int *quality)
{
    return runtime_gameplay_model_vector_profile_for_model_id(model_id,
                                                              radius,
                                                              length,
                                                              curve,
                                                              taper,
                                                              quality);
}

static void test_profile_rejects_invalid_arguments(void **state)
{
    (void)state;
    float radius = 0.0f;
    float length = 0.0f;
    float curve = 0.0f;
    float taper = 0.0f;
    int quality = 0;

    BANANA_TEST_ASSERT_TRUE(!profile_for(NULL, &radius, &length, &curve, &taper, &quality),
                            "null model id must be rejected");
    BANANA_TEST_ASSERT_TRUE(!profile_for("", &radius, &length, &curve, &taper, &quality),
                            "empty model id must be rejected");
    BANANA_TEST_ASSERT_TRUE(!profile_for("archon", NULL, &length, &curve, &taper, &quality),
                            "null radius output must be rejected");
    BANANA_TEST_ASSERT_TRUE(!profile_for("archon", &radius, NULL, &curve, &taper, &quality),
                            "null length output must be rejected");
    BANANA_TEST_ASSERT_TRUE(!profile_for("archon", &radius, &length, NULL, &taper, &quality),
                            "null curve output must be rejected");
    BANANA_TEST_ASSERT_TRUE(!profile_for("archon", &radius, &length, &curve, NULL, &quality),
                            "null taper output must be rejected");
    BANANA_TEST_ASSERT_TRUE(!profile_for("archon", &radius, &length, &curve, &taper, NULL),
                            "null quality output must be rejected");
}

static void test_profile_covers_war_lanes_and_environment_modifiers(void **state)
{
    (void)state;
    float radius = 0.0f;
    float length = 0.0f;
    float curve = 0.0f;
    float taper = 0.0f;
    int quality = 0;

    BANANA_TEST_ASSERT_TRUE(profile_for("archon-volcanic-flank-banana-01", &radius, &length, &curve, &taper, &quality),
                            "archon volcanic flank banana variant must resolve");
    BANANA_TEST_ASSERT_INT_EQ(quality, 3, "archon family must produce quality tier 3");

    BANANA_TEST_ASSERT_TRUE(profile_for("leviathan-glacier-regroup-bean-01", &radius, &length, &curve, &taper, &quality),
                            "leviathan glacier regroup bean variant must resolve");
    BANANA_TEST_ASSERT_INT_EQ(quality, 3, "leviathan family must produce quality tier 3");

    BANANA_TEST_ASSERT_TRUE(profile_for("archon-urban-envoy", &radius, &length, &curve, &taper, &quality),
                            "archon urban envoy variant must resolve");
    BANANA_TEST_ASSERT_TRUE(profile_for("archon-tropical-truce", &radius, &length, &curve, &taper, &quality),
                            "archon tropical truce variant must resolve");

    BANANA_TEST_ASSERT_TRUE(profile_for("phalanx-volcanic-flank-banana-02", &radius, &length, &curve, &taper, &quality),
                            "phalanx volcanic flank banana variant must resolve");
    BANANA_TEST_ASSERT_TRUE(profile_for("colossus-glacier-regroup-bean-02", &radius, &length, &curve, &taper, &quality),
                            "colossus glacier regroup bean variant must resolve");
    BANANA_TEST_ASSERT_TRUE(profile_for("phalanx-urban-envoy", &radius, &length, &curve, &taper, &quality),
                            "phalanx urban envoy variant must resolve");
    BANANA_TEST_ASSERT_TRUE(profile_for("phalanx-tropical-truce", &radius, &length, &curve, &taper, &quality),
                            "phalanx tropical truce variant must resolve");

    BANANA_TEST_ASSERT_TRUE(profile_for("siege-volcanic-flank-banana-03", &radius, &length, &curve, &taper, &quality),
                            "siege volcanic flank banana variant must resolve");
    BANANA_TEST_ASSERT_TRUE(profile_for("warbrute-glacier-regroup-bean-03", &radius, &length, &curve, &taper, &quality),
                            "warbrute glacier regroup bean variant must resolve");
    BANANA_TEST_ASSERT_TRUE(profile_for("commander-urban-envoy", &radius, &length, &curve, &taper, &quality),
                            "commander urban envoy variant must resolve");
    BANANA_TEST_ASSERT_TRUE(profile_for("siege-tropical-truce", &radius, &length, &curve, &taper, &quality),
                            "siege tropical truce variant must resolve");

    BANANA_TEST_ASSERT_TRUE(profile_for("scout-urban-flank-banana-01", &radius, &length, &curve, &taper, &quality),
                            "scout urban flank banana variant must resolve");
    BANANA_TEST_ASSERT_TRUE(profile_for("raider-tropical-regroup-bean-01", &radius, &length, &curve, &taper, &quality),
                            "raider tropical regroup bean variant must resolve");
    BANANA_TEST_ASSERT_TRUE(profile_for("scout-envoy", &radius, &length, &curve, &taper, &quality),
                            "scout envoy variant must resolve");
    BANANA_TEST_ASSERT_TRUE(profile_for("raider-truce", &radius, &length, &curve, &taper, &quality),
                            "raider truce variant must resolve");
}

static void test_profile_covers_reference_and_default_lanes(void **state)
{
    (void)state;
    float radius = 0.0f;
    float length = 0.0f;
    float curve = 0.0f;
    float taper = 0.0f;
    int quality = 0;

    BANANA_TEST_ASSERT_TRUE(profile_for("banana-bean-green", &radius, &length, &curve, &taper, &quality),
                            "banana-bean-green id must resolve");
    BANANA_TEST_ASSERT_INT_EQ(quality, 3, "banana-bean-green profile must use quality tier 3");

    BANANA_TEST_ASSERT_TRUE(profile_for("bean-market", &radius, &length, &curve, &taper, &quality),
                            "bean lane id must resolve");
    BANANA_TEST_ASSERT_INT_EQ(quality, 3, "bean profile must use quality tier 3");

    BANANA_TEST_ASSERT_TRUE(profile_for("reference/banana-basic", &radius, &length, &curve, &taper, &quality),
                            "reference banana basic id must resolve");
    BANANA_TEST_ASSERT_INT_EQ(quality, 3, "reference banana profile must use quality tier 3");

    BANANA_TEST_ASSERT_TRUE(profile_for("landmark-hub-arch-gate", &radius, &length, &curve, &taper, &quality),
                            "landmark lane id must resolve");
    BANANA_TEST_ASSERT_INT_EQ(quality, 2, "landmark profile must use quality tier 2");

    BANANA_TEST_ASSERT_TRUE(profile_for("traversal-cluster-barrier-stall", &radius, &length, &curve, &taper, &quality),
                            "traversal lane id must resolve");
    BANANA_TEST_ASSERT_INT_EQ(quality, 2, "traversal profile must use quality tier 2");

    BANANA_TEST_ASSERT_TRUE(profile_for("misc-custom-model", &radius, &length, &curve, &taper, &quality),
                            "fallback model id must resolve");
    BANANA_TEST_ASSERT_INT_EQ(quality, 2, "fallback profile must use quality tier 2");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_profile_rejects_invalid_arguments),
    BANANA_TEST_CASE(test_profile_covers_war_lanes_and_environment_modifiers),
    BANANA_TEST_CASE(test_profile_covers_reference_and_default_lanes)
)
