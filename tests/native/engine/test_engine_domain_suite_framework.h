#ifndef BANANA_TEST_ENGINE_DOMAIN_SUITE_FRAMEWORK_H
#define BANANA_TEST_ENGINE_DOMAIN_SUITE_FRAMEWORK_H

#include <math.h>
#include <stdio.h>

typedef struct DomainSuiteStats
{
    int pass;
    int fail;
} DomainSuiteStats;

#define SUITE_TEST(name) \
    do { printf("  %-60s", name); } while (0)

#define SUITE_PASS(stats) \
    do { printf("PASS\n"); (stats)->pass++; } while (0)

#define SUITE_FAIL(stats, msg) \
    do { printf("FAIL (%s)\n", msg); (stats)->fail++; } while (0)

#define SUITE_ASSERT_TRUE(stats, expr) \
    do { \
        if (!(expr)) { \
            SUITE_FAIL(stats, #expr " is false"); \
            goto done; \
        } \
    } while (0)

#define SUITE_ASSERT_INT_EQ(stats, a, b) \
    do { \
        if ((a) != (b)) { \
            char _m[96]; \
            snprintf(_m, sizeof(_m), "%d != %d", (int)(a), (int)(b)); \
            SUITE_FAIL(stats, _m); \
            goto done; \
        } \
    } while (0)

#define SUITE_ASSERT_NEAR(stats, a, b, tol) \
    do { \
        if (fabsf((a) - (b)) > (tol)) { \
            SUITE_FAIL(stats, #a " !~= " #b); \
            goto done; \
        } \
    } while (0)

#endif
