#include "runtime/resource/resource_domain.h"
#include "runtime/support/test_support.h"

static void test_resource_domain_parse_and_name(void **state)
{
    (void)state;
    RuntimeResourceKey key = RUNTIME_RESOURCE_COUNT;

    BANANA_TEST_ASSERT_INT_EQ(runtime_resource_parse_key("wood", &key), 0,
                              "parse_key must accept wood");
    BANANA_TEST_ASSERT_INT_EQ((int)key, (int)RUNTIME_RESOURCE_WOOD,
                              "parse_key must map wood to the wood enum");
    BANANA_TEST_ASSERT_STR_EQ(runtime_resource_key_name(RUNTIME_RESOURCE_WOOD), "wood",
                              "key_name must return wood");

    BANANA_TEST_ASSERT_INT_EQ(runtime_resource_parse_key("ore", &key), 0,
                              "parse_key must accept ore");
    BANANA_TEST_ASSERT_INT_EQ((int)key, (int)RUNTIME_RESOURCE_ORE,
                              "parse_key must map ore to the ore enum");
    BANANA_TEST_ASSERT_STR_EQ(runtime_resource_key_name(RUNTIME_RESOURCE_ORE), "ore",
                              "key_name must return ore");

    BANANA_TEST_ASSERT_INT_EQ(runtime_resource_parse_key("gold", &key), 0,
                              "parse_key must accept gold");
    BANANA_TEST_ASSERT_INT_EQ((int)key, (int)RUNTIME_RESOURCE_GOLD,
                              "parse_key must map gold to the gold enum");
    BANANA_TEST_ASSERT_STR_EQ(runtime_resource_key_name(RUNTIME_RESOURCE_GOLD), "gold",
                              "key_name must return gold");

    BANANA_TEST_ASSERT_INT_EQ(runtime_resource_parse_key("silver", &key), -1,
                              "parse_key must reject unknown resources");
    BANANA_TEST_ASSERT_INT_EQ(runtime_resource_parse_key(NULL, &key), -1,
                              "parse_key must reject null strings");
    BANANA_TEST_ASSERT_INT_EQ(runtime_resource_parse_key("wood", NULL), -1,
                              "parse_key must reject null output pointers");
    BANANA_TEST_ASSERT_STR_EQ(runtime_resource_key_name((RuntimeResourceKey)99), "",
                              "key_name must return empty string for unknown keys");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_resource_domain_parse_and_name)
)