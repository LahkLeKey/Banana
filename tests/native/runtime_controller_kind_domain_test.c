#include "runtime/controller/controller_kind_domain.h"

#include <stdio.h>
#include <string.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

static int expect_str(const char *label, const char *actual, const char *expected)
{
    if (strcmp(actual, expected) == 0)
        return 1;

    fprintf(stderr, "%s expected='%s' actual='%s'\n", label, expected, actual);
    return 0;
}

int main(void)
{
    RuntimeControllerKind kind = RUNTIME_CONTROLLER_KIND_UNKNOWN;

    if (!expect_int("parse human", runtime_controller_kind_parse("human", &kind), 0))
        return 1;
    if (!expect_int("human enum", (int)kind, (int)RUNTIME_CONTROLLER_KIND_HUMAN))
        return 1;

    if (!expect_int("parse wildlife", runtime_controller_kind_parse("wildlife", &kind), 0))
        return 1;
    if (!expect_int("wildlife enum", (int)kind, (int)RUNTIME_CONTROLLER_KIND_WILDLIFE))
        return 1;

    if (!expect_int("parse invalid", runtime_controller_kind_parse("banana", &kind), -1))
        return 1;

    if (!expect_int("parse-or-unknown invalid",
                    (int)runtime_controller_kind_parse_or_unknown("banana"),
                    (int)RUNTIME_CONTROLLER_KIND_UNKNOWN))
        return 1;

    if (!expect_str("name ai",
                    runtime_controller_kind_name(RUNTIME_CONTROLLER_KIND_AI),
                    "ai"))
        return 1;

    if (!expect_str("name invalid",
                    runtime_controller_kind_name(RUNTIME_CONTROLLER_KIND_UNKNOWN),
                    ""))
        return 1;

    return 0;
}
