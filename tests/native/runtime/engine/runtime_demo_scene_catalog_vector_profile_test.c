#include "win32_dx12_poc/scene/demo_scene_catalog.h"

#include <stdio.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

static int expect_float_x100(const char *label, float actual, int expected_x100)
{
    int actual_x100 = (int)(actual * 100.0f + 0.5f);
    if (actual_x100 == expected_x100)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected_x100, actual_x100);
    return 0;
}

int main(void)
{
    float radius = 0.0f;
    float length = 0.0f;
    float curve = 0.0f;
    float tip = 0.0f;
    int quality = 0;

    if (!expect_true("reference banana profile resolves",
                     banana_poc_demo_scene_vector_profile_for_model_id(
                         "gameplay/reference/banana-basic-v1",
                         &radius,
                         &length,
                         &curve,
                         &tip,
                         &quality) == 1))
    {
        return 1;
    }
    if (!expect_float_x100("reference radius", radius, 105))
        return 1;
    if (!expect_true("reference quality", quality == 3))
        return 1;

    if (!expect_true("bean profile resolves",
                     banana_poc_demo_scene_vector_profile_for_model_id(
                         "gameplay/reference/banana-bean-green-v1",
                         &radius,
                         &length,
                         &curve,
                         &tip,
                         &quality) == 1))
    {
        return 1;
    }
    if (!expect_float_x100("bean radius", radius, 132))
        return 1;
    if (!expect_true("bean quality", quality == 3))
        return 1;

    if (!expect_true("landmark profile resolves",
                     banana_poc_demo_scene_vector_profile_for_model_id(
                         "gameplay/tropical-coastal/volcanic-arch-v1",
                         &radius,
                         &length,
                         &curve,
                         &tip,
                         &quality) == 1))
    {
        return 1;
    }
    if (!expect_float_x100("landmark length", length, 82))
        return 1;
    if (!expect_true("landmark quality", quality == 2))
        return 1;

    if (!expect_true("traversal profile resolves",
                     banana_poc_demo_scene_vector_profile_for_model_id(
                         "gameplay/urban-industrial/platform-barrier-v1",
                         &radius,
                         &length,
                         &curve,
                         &tip,
                         &quality) == 1))
    {
        return 1;
    }
    if (!expect_float_x100("traversal curve", curve, 46))
        return 1;

    if (!expect_true("banana apex profile resolves",
                     banana_poc_demo_scene_vector_profile_for_model_id(
                         "gameplay/war/banana-phalanx-urban-v1",
                         &radius,
                         &length,
                         &curve,
                         &tip,
                         &quality) == 1))
    {
        return 1;
    }
    if (!expect_float_x100("banana apex radius", radius, 178))
        return 1;
    if (!expect_true("banana apex quality", quality == 3))
        return 1;

    if (!expect_true("bean apex profile resolves",
                     banana_poc_demo_scene_vector_profile_for_model_id(
                         "gameplay/war/bean-colossus-glacier-v1",
                         &radius,
                         &length,
                         &curve,
                         &tip,
                         &quality) == 1))
    {
        return 1;
    }
    if (!expect_float_x100("bean apex curve glacier", curve, 25))
        return 1;

    if (!expect_true("banana mythic profile resolves",
                     banana_poc_demo_scene_vector_profile_for_model_id(
                         "gameplay/war/banana-archon-urban-v1",
                         &radius,
                         &length,
                         &curve,
                         &tip,
                         &quality) == 1))
    {
        return 1;
    }
    if (!expect_float_x100("banana mythic radius", radius, 206))
        return 1;
    if (!expect_true("banana mythic quality", quality == 3))
        return 1;

    if (!expect_true("bean mythic profile resolves",
                     banana_poc_demo_scene_vector_profile_for_model_id(
                         "gameplay/war/bean-leviathan-glacier-v1",
                         &radius,
                         &length,
                         &curve,
                         &tip,
                         &quality) == 1))
    {
        return 1;
    }
    if (!expect_float_x100("bean mythic curve glacier", curve, 18))
        return 1;

    if (!expect_true("banana flank skirmish profile resolves",
                     banana_poc_demo_scene_vector_profile_for_model_id(
                         "gameplay/war/banana-scout-flank-urban-v1",
                         &radius,
                         &length,
                         &curve,
                         &tip,
                         &quality) == 1))
    {
        return 1;
    }
    if (!expect_float_x100("banana flank urban length", length, 156))
        return 1;
    if (!expect_float_x100("banana flank urban curve", curve, 120))
        return 1;

    if (!expect_true("banana regroup skirmish profile resolves",
                     banana_poc_demo_scene_vector_profile_for_model_id(
                         "gameplay/war/banana-scout-regroup-glacier-v1",
                         &radius,
                         &length,
                         &curve,
                         &tip,
                         &quality) == 1))
    {
        return 1;
    }
    if (!expect_float_x100("banana regroup glacier radius", radius, 102))
        return 1;
    if (!expect_float_x100("banana regroup glacier curve", curve, 90))
        return 1;

    if (!expect_true("bean negotiate envoy profile resolves",
                     banana_poc_demo_scene_vector_profile_for_model_id(
                         "gameplay/war/bean-raider-envoy-tropical-v1",
                         &radius,
                         &length,
                         &curve,
                         &tip,
                         &quality) == 1))
    {
        return 1;
    }
    if (!expect_float_x100("bean envoy tropical curve", curve, 94))
        return 1;
    if (!expect_float_x100("bean envoy tropical tip", tip, 27))
        return 1;

    if (!expect_true("null model id rejected",
                     banana_poc_demo_scene_vector_profile_for_model_id(NULL,
                                                                        &radius,
                                                                        &length,
                                                                        &curve,
                                                                        &tip,
                                                                        &quality) == 0))
    {
        return 1;
    }

    printf("runtime_demo_scene_catalog_vector_profile_test: pass\n");
    return 0;
}
