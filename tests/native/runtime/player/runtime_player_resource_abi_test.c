#include "runtime/player/player_resource_abi.h"
#include "runtime/player/player_registry.h"

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
    const char *guid = "resource-test-player";

    runtime_player_registry_reset();
    runtime_player_registry_add_default(1,
                                        guid,
                                        "Resource Tester",
                                        "human",
                                        1);

    if (!expect_int("unknown resource get", runtime_player_resource_abi_get(guid, "banana"), 0))
        return 1;

    if (!expect_int("wood starts zero", runtime_player_resource_abi_get(guid, "wood"), 0))
        return 1;

    if (!expect_int("add wood via abi", runtime_player_resource_abi_add(guid, "wood", 7), 7))
        return 1;

    if (!expect_int("get wood via key", runtime_player_resource_abi_get_by_key(guid, RUNTIME_RESOURCE_WOOD), 7))
        return 1;

    if (!expect_int("set total by key", runtime_player_resource_abi_set_total_by_key(guid, RUNTIME_RESOURCE_WOOD, 3), 3))
        return 1;

    if (!expect_int("add by key clamps through registry", runtime_player_resource_abi_add_by_key(guid, RUNTIME_RESOURCE_WOOD, -10), 0))
        return 1;

    if (!expect_int("null guid get", runtime_player_resource_abi_get(NULL, "wood"), 0))
        return 1;

    return 0;
}
