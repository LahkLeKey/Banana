#include "runtime/player_gateway_abi.h"

#include <stdio.h>
#include <string.h>

#include "runtime/player_registry.h"
#include "world/world.h"

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s failed: got %d expected %d\n", label, actual, expected);
    return 0;
}

static int expect_str(const char *label, const char *actual, const char *expected)
{
    const char *a = actual ? actual : "";
    const char *e = expected ? expected : "";

    if (0 == strcmp(a, e))
        return 1;

    fprintf(stderr, "%s failed: got '%s' expected '%s'\n", label, a, e);
    return 0;
}

static int stub_get(void *context, RuntimeResourceKey key)
{
    (void)context;
    return key == RUNTIME_RESOURCE_GOLD ? 9 : 0;
}

static int stub_set(void *context, RuntimeResourceKey key, int amount)
{
    (void)context;
    return (key == RUNTIME_RESOURCE_WOOD && amount == 4) ? 1 : 0;
}

static int stub_add(void *context, RuntimeResourceKey key, int amount)
{
    (void)context;
    return (key == RUNTIME_RESOURCE_ORE && amount == 2) ? 1 : 0;
}

int main(void)
{
    const char *guid;
    RuntimeMerchantResourceGateway gateway;

    runtime_player_registry_reset();
    runtime_player_registry_add_default(11, "guid-a", "PlayerA", "human", 1);
    runtime_player_registry_add_default(22, "guid-b", "PlayerB", "human", 1);

    guid = runtime_player_gateway_active_guid(22);
    if (!expect_str("active guid by entity", guid, "guid-b"))
        return 1;

    guid = runtime_player_gateway_active_guid(999);
    if (!expect_str("fallback first guid", guid, "guid-a"))
        return 1;

    runtime_player_registry_reset();
    guid = runtime_player_gateway_active_guid(1);
    if (!expect_int("empty registry returns null", guid == NULL ? 1 : 0, 1))
        return 1;

    gateway = runtime_player_gateway_resource_gateway(stub_get, stub_set, stub_add, NULL);
    if (!expect_int("gateway get", gateway.get(gateway.context, RUNTIME_RESOURCE_GOLD), 9))
        return 1;
    if (!expect_int("gateway set", gateway.set_total(gateway.context, RUNTIME_RESOURCE_WOOD, 4), 1))
        return 1;
    if (!expect_int("gateway add", gateway.add(gateway.context, RUNTIME_RESOURCE_ORE, 2), 1))
        return 1;

    return 0;
}
