#include "runtime/merchant/trade/merchant_trade_domain.h"
#include "runtime/resource/resource_domain.h"

#include <stdio.h>
#include <string.h>

static int tests_run = 0;
static int tests_passed = 0;

#define ASSERT_EQ(actual, expected, label)                                                     \
    do                                                                                         \
    {                                                                                          \
        tests_run++;                                                                           \
        if ((actual) == (expected))                                                            \
        {                                                                                      \
            tests_passed++;                                                                    \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            printf("FAIL: %s (expected %d got %d)\n", label, (expected), (actual));         \
        }                                                                                      \
    } while (0)

#define ASSERT_STREQ(actual, expected, label)                                                  \
    do                                                                                         \
    {                                                                                          \
        tests_run++;                                                                           \
        if (strcmp((actual), (expected)) == 0)                                                 \
        {                                                                                      \
            tests_passed++;                                                                    \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            printf("FAIL: %s (expected '%s' got '%s')\n", label, (expected), (actual));     \
        }                                                                                      \
    } while (0)

static void test_resource_parsing(void)
{
    RuntimeResourceKey key;

    ASSERT_EQ(runtime_resource_parse_key("wood", &key), 0, "parse wood");
    ASSERT_EQ((int)key, RUNTIME_RESOURCE_WOOD, "wood key");
    ASSERT_EQ(runtime_resource_parse_key("ore", &key), 0, "parse ore");
    ASSERT_EQ((int)key, RUNTIME_RESOURCE_ORE, "ore key");
    ASSERT_EQ(runtime_resource_parse_key("gold", &key), 0, "parse gold");
    ASSERT_EQ((int)key, RUNTIME_RESOURCE_GOLD, "gold key");

    ASSERT_EQ(runtime_resource_parse_key("banana", &key), -1, "reject unknown resource");
    ASSERT_EQ(runtime_resource_parse_key(NULL, &key), -1, "reject null resource");
    ASSERT_EQ(runtime_resource_parse_key("wood", NULL), -1, "reject null output pointer");
}

static void test_resource_key_names(void)
{
    ASSERT_STREQ(runtime_resource_key_name(RUNTIME_RESOURCE_WOOD), "wood", "wood key name");
    ASSERT_STREQ(runtime_resource_key_name(RUNTIME_RESOURCE_ORE), "ore", "ore key name");
    ASSERT_STREQ(runtime_resource_key_name(RUNTIME_RESOURCE_GOLD), "gold", "gold key name");
    ASSERT_STREQ(runtime_resource_key_name(RUNTIME_RESOURCE_COUNT), "", "invalid key name");
}

static void test_merchant_status_mapping(void)
{
    ASSERT_EQ(runtime_merchant_map_buy_status(MERCHANT_TRADE_OK), 0, "buy ok");
    ASSERT_EQ(runtime_merchant_map_buy_status(MERCHANT_TRADE_INSUFFICIENT_GOLD), -2, "buy insufficient gold");
    ASSERT_EQ(runtime_merchant_map_buy_status(MERCHANT_TRADE_INSUFFICIENT_STOCK), -3, "buy insufficient stock");
    ASSERT_EQ(runtime_merchant_map_buy_status(MERCHANT_TRADE_INVALID_ITEM), -1, "buy invalid item");

    ASSERT_EQ(runtime_merchant_map_sell_status(MERCHANT_TRADE_OK), 0, "sell ok");
    ASSERT_EQ(runtime_merchant_map_sell_status(MERCHANT_TRADE_INSUFFICIENT_STOCK), -3, "sell insufficient stock");
    ASSERT_EQ(runtime_merchant_map_sell_status(MERCHANT_TRADE_INSUFFICIENT_GOLD), -4, "sell insufficient gold");
    ASSERT_EQ(runtime_merchant_map_sell_status(MERCHANT_TRADE_INVALID_MERCHANT), -1, "sell invalid merchant");
}

int main(void)
{
    test_resource_parsing();
    test_resource_key_names();
    test_merchant_status_mapping();

    printf("Passed: %d / %d\n", tests_passed, tests_run);
    return (tests_run == tests_passed) ? 0 : 1;
}
