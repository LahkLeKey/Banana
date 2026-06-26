#include "runtime/player/gateway/player_gateway_abi.h"

#include "runtime/support/test_support.h"

#include <string.h>

static NativePlayerBinding g_bindings[3];
static int g_binding_count = 0;

int runtime_player_registry_count(void)
{
    return g_binding_count;
}

NativePlayerBinding *runtime_player_registry_get(int index)
{
    if (index < 0 || index >= g_binding_count)
        return NULL;
    return &g_bindings[index];
}

static void reset_bindings(void)
{
    memset(g_bindings, 0, sizeof(g_bindings));
    g_binding_count = 0;
}

static void test_active_guid_prefers_exact_entity_match(void **state)
{
    const char *guid = NULL;

    (void)state;
    reset_bindings();

    g_binding_count = 3;
    strncpy(g_bindings[0].guid, "fallback", sizeof(g_bindings[0].guid) - 1);
    g_bindings[0].entity_id = 11;

    strncpy(g_bindings[1].guid, "", sizeof(g_bindings[1].guid) - 1);
    g_bindings[1].entity_id = 77;

    strncpy(g_bindings[2].guid, "exact", sizeof(g_bindings[2].guid) - 1);
    g_bindings[2].entity_id = 99;

    guid = runtime_player_gateway_active_guid(99);

    BANANA_TEST_ASSERT_STR_EQ(guid,
                              "exact",
                              "active guid should return exact entity match when present");
}

static void test_active_guid_falls_back_to_first_non_empty_binding(void **state)
{
    const char *guid = NULL;

    (void)state;
    reset_bindings();

    g_binding_count = 2;
    strncpy(g_bindings[0].guid, "first", sizeof(g_bindings[0].guid) - 1);
    g_bindings[0].entity_id = 3;
    strncpy(g_bindings[1].guid, "second", sizeof(g_bindings[1].guid) - 1);
    g_bindings[1].entity_id = 4;

    guid = runtime_player_gateway_active_guid(404);

    BANANA_TEST_ASSERT_STR_EQ(guid,
                              "first",
                              "active guid should fall back to first non-empty binding");
}

static void test_resource_gateway_preserves_callbacks_and_context(void **state)
{
    RuntimeMerchantResourceGateway gateway;
    void *context = (void *)0x1234;

    (void)state;

    gateway = runtime_player_gateway_resource_gateway((RuntimeMerchantGetResourceByKeyFn)0x1,
                                                      (RuntimeMerchantSetResourceTotalByKeyFn)0x2,
                                                      (RuntimeMerchantAddResourceByKeyFn)0x3,
                                                      context);

    BANANA_TEST_ASSERT_TRUE(gateway.get == (RuntimeMerchantGetResourceByKeyFn)0x1,
                            "resource gateway should preserve get callback");
    BANANA_TEST_ASSERT_TRUE(gateway.set_total == (RuntimeMerchantSetResourceTotalByKeyFn)0x2,
                            "resource gateway should preserve set_total callback");
    BANANA_TEST_ASSERT_TRUE(gateway.add == (RuntimeMerchantAddResourceByKeyFn)0x3,
                            "resource gateway should preserve add callback");
    BANANA_TEST_ASSERT_TRUE(gateway.context == context,
                            "resource gateway should preserve context pointer");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_active_guid_prefers_exact_entity_match),
    BANANA_TEST_CASE(test_active_guid_falls_back_to_first_non_empty_binding),
    BANANA_TEST_CASE(test_resource_gateway_preserves_callbacks_and_context)
)
