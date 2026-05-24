#include "runtime/player/player_build_abi.h"
#include "runtime/player/player_builds.h"

#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    int damage_bonus = 0;
    int mitigation_bonus = 0;
    int synergy_bonus = 0;

    player_builds_init();

    if (!expect_int("set class", runtime_player_build_abi_set_class("p-build", BUILD_CLASS_RANGER), 0))
        return 1;

    if (!expect_int("set allocations",
                    runtime_player_build_abi_set_allocations("p-build", 3, 2, 1),
                    0))
        return 1;

    if (!expect_int("equip weapon",
                    runtime_player_build_abi_equip("p-build", GEAR_SLOT_WEAPON, 2, 4, 0, 0),
                    0))
        return 1;

    if (!expect_int("get attack stat",
                    runtime_player_build_abi_get_stat("p-build", "attack"),
                    30))
    {
        return 1;
    }

    if (!expect_int("combo evaluate",
                    runtime_player_build_abi_evaluate_combo("p-build",
                                                            "snare_shot",
                                                            "piercing_volley",
                                                            900,
                                                            3,
                                                            &damage_bonus,
                                                            &mitigation_bonus,
                                                            &synergy_bonus),
                    1))
    {
        return 1;
    }

    if (!expect_int("combo party damage", damage_bonus, 17))
        return 1;
    if (!expect_int("combo mitigation", mitigation_bonus, 0))
        return 1;
    if (!expect_int("combo synergy", synergy_bonus, 9))
        return 1;

    if (!expect_int("combo evaluate with null outputs",
                    runtime_player_build_abi_evaluate_combo("p-build",
                                                            "snare_shot",
                                                            "piercing_volley",
                                                            900,
                                                            3,
                                                            NULL,
                                                            NULL,
                                                            NULL),
                    1))
    {
        return 1;
    }

    if (!expect_int("reject invalid class",
                    runtime_player_build_abi_set_class("p-build", 99),
                    -1))
        return 1;

    if (!expect_int("reject invalid slot",
                    runtime_player_build_abi_equip("p-build", 99, 1, 1, 1, 1),
                    -1))
        return 1;

    if (!expect_int("reject invalid gear tier",
                    runtime_player_build_abi_equip("p-build", GEAR_SLOT_WEAPON, 99, 1, 1, 1),
                    -1))
        return 1;

    if (!expect_int("reject invalid stat",
                    runtime_player_build_abi_get_stat("p-build", "banana"),
                    -1))
        return 1;

    if (!expect_int("reject null guid class",
                    runtime_player_build_abi_set_class(NULL, BUILD_CLASS_VANGUARD),
                    -1))
        return 1;

    if (!expect_int("reject null guid stat",
                    runtime_player_build_abi_get_stat(NULL, "attack"),
                    -1))
        return 1;

    if (!expect_int("reject invalid combo args",
                    runtime_player_build_abi_evaluate_combo("p-build",
                                                            "",
                                                            "piercing_volley",
                                                            100,
                                                            2,
                                                            NULL,
                                                            NULL,
                                                            NULL),
                    -1))
    {
        return 1;
    }

    player_builds_cleanup();
    return 0;
}
