#include "win32_dx12_poc/scene/demo_scene_catalog.h"

#include <stdio.h>

static int expect_true(const char *label, int condition)
{
    if (condition)
        return 1;

    fprintf(stderr, "%s failed\n", label);
    return 0;
}

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_float_x10(const char *label, float actual, int expected_x10)
{
    int actual_x10 = (int)(actual * 10.0f + 0.5f);
    if (actual_x10 == expected_x10)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected_x10, actual_x10);
    return 0;
}

int main(void)
{
    float radius = 0.0f;
    int reinforcements = 0;
    int cap = 0;

    if (!expect_int("invalid variant rejects policy",
                    banana_poc_demo_scene_catalog_war_policy_for_variant(999,
                                                                         &radius,
                                                                         &reinforcements,
                                                                         &cap),
                    0))
    {
        return 1;
    }

    if (!expect_int("continent policy available",
                    banana_poc_demo_scene_catalog_war_policy_for_variant(0,
                                                                         &radius,
                                                                         &reinforcements,
                                                                         &cap),
                    1))
    {
        return 1;
    }
    if (!expect_float_x10("continent radius", radius, 120))
        return 1;
    if (!expect_int("continent reinforcements", reinforcements, 2))
        return 1;
    if (!expect_int("continent cap", cap, 128))
        return 1;

    if (!expect_int("station policy available",
                    banana_poc_demo_scene_catalog_war_policy_for_variant(2,
                                                                         &radius,
                                                                         &reinforcements,
                                                                         &cap),
                    1))
    {
        return 1;
    }
    if (!expect_float_x10("station radius", radius, 80))
        return 1;
    if (!expect_int("station reinforcements", reinforcements, 3))
        return 1;
    if (!expect_int("station cap", cap, 144))
        return 1;

    if (!expect_int("corridor policy available",
                    banana_poc_demo_scene_catalog_war_policy_for_variant(3,
                                                                         &radius,
                                                                         &reinforcements,
                                                                         &cap),
                    1))
    {
        return 1;
    }
    if (!expect_float_x10("corridor radius", radius, 60))
        return 1;
    if (!expect_int("corridor reinforcements", reinforcements, 4))
        return 1;
    if (!expect_int("corridor cap", cap, 176))
        return 1;

    if (!expect_int("sentience showcase policy available",
                    banana_poc_demo_scene_catalog_war_policy_for_variant(6,
                                                                         &radius,
                                                                         &reinforcements,
                                                                         &cap),
                    1))
    {
        return 1;
    }
    if (!expect_float_x10("sentience showcase radius", radius, 50))
        return 1;
    if (!expect_int("sentience showcase reinforcements", reinforcements, 6))
        return 1;
    if (!expect_int("sentience showcase cap", cap, 192))
        return 1;

    if (!expect_true("null output pointer rejected",
                     banana_poc_demo_scene_catalog_war_policy_for_variant(0,
                                                                          NULL,
                                                                          &reinforcements,
                                                                          &cap) == 0))
    {
        return 1;
    }

    printf("runtime_demo_scene_catalog_war_policy_test: pass\n");
    return 0;
}
