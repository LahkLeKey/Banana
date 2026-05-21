#include "engine.h"

#include <stdio.h>
#include <string.h>

static int expect_contains(const char *haystack, const char *needle)
{
    if (strstr(haystack, needle) != NULL)
        return 1;

    fprintf(stderr, "expected snapshot to contain %s\nsnapshot: %s\n", needle, haystack);
    return 0;
}

int main(void)
{
    const char *snapshot = NULL;

    if (engine_init(640, 360) != 0)
    {
        fprintf(stderr, "engine_init failed\n");
        return 1;
    }

    snapshot = engine_tick_serialize(1.0f / 60.0f);
    if (!expect_contains(snapshot, "\"tick\":1"))
        return 1;
    if (!expect_contains(snapshot, "\"isAuthoritative\":true"))
        return 1;
    if (!expect_contains(snapshot, "\"entities\":{"))
        return 1;
    if (!expect_contains(snapshot, "\"entity-1\""))
        return 1;
    if (!expect_contains(snapshot, "\"type\":\"player\""))
        return 1;
    if (!expect_contains(snapshot, "\"playerGuid\":\"native-default-player\""))
        return 1;
    if (!expect_contains(snapshot, "\"controllerKind\":\"human\""))
        return 1;

    if (engine_player_upsert("00000000-0000-4000-8000-000000000111", "ai-bot", "ai", 1) == 0)
    {
        fprintf(stderr, "engine_player_upsert failed\n");
        return 1;
    }
    snapshot = engine_tick_serialize(1.0f / 60.0f);
    if (!expect_contains(snapshot, "\"playerGuid\":\"00000000-0000-4000-8000-000000000111\""))
        return 1;
    if (!expect_contains(snapshot, "\"controllerKind\":\"ai\""))
        return 1;

    engine_set_move_input(1.0f, 0.0f);
    engine_player_apply_input("native-default-player", 1.0f, 0.0f);
    snapshot = engine_tick_serialize(1.0f / 60.0f);
    if (!expect_contains(snapshot, "\"tick\":3"))
        return 1;
    if (!expect_contains(snapshot, "\"x\":"))
        return 1;
    if (!expect_contains(snapshot, "\"z\":"))
        return 1;

    engine_shutdown();
    return 0;
}
