#include "runtime/controller/kind/controller_kind_domain.h"
#include "runtime/support/test_support.h"

#include <string.h>

static void test_controller_kind_parse_known_and_unknown_values(void **state)
{
    (void)state;
    RuntimeControllerKind kind = RUNTIME_CONTROLLER_KIND_UNKNOWN;

    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_kind_parse("merchant", &kind), 0,
                              "parse must accept known controller kinds");
    BANANA_TEST_ASSERT_INT_EQ((int)kind, (int)RUNTIME_CONTROLLER_KIND_MERCHANT,
                              "parse must return expected enum value");

    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_kind_parse("not-a-kind", &kind), -1,
                              "parse must reject unknown names");
}

static void test_controller_kind_parse_guards_and_fallbacks(void **state)
{
    (void)state;
    RuntimeControllerKind kind = RUNTIME_CONTROLLER_KIND_UNKNOWN;

    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_kind_parse(NULL, &kind), -1,
                              "parse must reject null input strings");
    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_kind_parse("", &kind), -1,
                              "parse must reject empty input strings");
    BANANA_TEST_ASSERT_INT_EQ(runtime_controller_kind_parse("human", NULL), -1,
                              "parse must reject null output pointers");

    BANANA_TEST_ASSERT_INT_EQ((int)runtime_controller_kind_parse_or_unknown("unknown-value"),
                              (int)RUNTIME_CONTROLLER_KIND_UNKNOWN,
                              "parse_or_unknown must return unknown for invalid values");
}

static void test_controller_kind_name_known_and_unknown(void **state)
{
    (void)state;
    const char *known = runtime_controller_kind_name(RUNTIME_CONTROLLER_KIND_HUMAN);
    const char *unknown = runtime_controller_kind_name((RuntimeControllerKind)999);

    BANANA_TEST_ASSERT_TRUE(strcmp(known, "human") == 0,
                            "name must map known enum values");
    BANANA_TEST_ASSERT_TRUE(strcmp(unknown, "") == 0,
                            "name must return empty string for unknown enum values");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_controller_kind_parse_known_and_unknown_values),
    BANANA_TEST_CASE(test_controller_kind_parse_guards_and_fallbacks),
    BANANA_TEST_CASE(test_controller_kind_name_known_and_unknown)
)
