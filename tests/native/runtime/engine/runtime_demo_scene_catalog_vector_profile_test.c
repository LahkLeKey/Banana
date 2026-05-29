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
