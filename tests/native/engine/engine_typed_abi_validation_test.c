#include "../../../src/native/engine/engine.h"

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
    const char *guid = "typed-boundary-player";
    uint32_t player_id = 0;

    engine_shutdown();

    if (!expect_int("pre-init terrain write rejected", engine_terrain_set_height(1, 1, 2), -1))
        return 1;
    if (!expect_int("pre-init merchant null item", engine_npc_merchant_trade_buy(0, NULL, 1), -1))
        return 1;

    if (!expect_int("engine init", engine_init(640, 360), 0))
        return 1;

    player_id = engine_player_upsert(guid, "typed-boundary", "not-a-kind", 1);
    if (!expect_int("upsert accepts unknown controller kind", player_id != 0 ? 1 : 0, 1))
        return 1;

    if (!expect_int("unknown kind defaults to vanguard stat",
                    engine_player_build_get_stat(guid, "attack"),
                    18))
        return 1;

    if (!expect_int("invalid class rejected",
                    engine_player_build_set_class(guid, 99),
                    -1))
        return 1;

    if (!expect_int("invalid gear slot rejected",
                    engine_player_build_equip(guid, 99, 1, 1, 1, 1),
                    -1))
        return 1;

    if (!expect_int("invalid stat rejected",
                    engine_player_build_get_stat(guid, "banana"),
                    -1))
        return 1;

    if (!expect_int("null stat rejected",
                    engine_player_build_get_stat(guid, NULL),
                    -1))
        return 1;

    if (!expect_int("null resource add rejected",
                    engine_player_add_resource(NULL, 5),
                    0))
        return 1;

    if (!expect_int("invalid resource add rejected",
                    engine_player_add_resource("banana", 5),
                    0))
        return 1;

    if (!expect_int("invalid controller attach rejected",
                    (int)engine_controller_attach(0, "wildlife"),
                    0))
        return 1;

    if (!expect_int("out-of-bounds terrain write rejected",
                    engine_terrain_set_height(-1, 0, 1),
                    0))
        return 1;

    if (!expect_int("out-of-bounds terrain write rejected positive",
                    engine_terrain_set_height(999, 0, 1),
                    0))
        return 1;

    if (!expect_int("null merchant price request",
                    engine_npc_merchant_get_price(0, NULL),
                    0))
        return 1;

    engine_shutdown();
    return 0;
}
