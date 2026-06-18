#ifndef BANANA_NATIVE_RUNTIME_TEST_SUPPORT_H
#define BANANA_NATIVE_RUNTIME_TEST_SUPPORT_H

#include <stdio.h>
#include <string.h>

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#else
#include <stddef.h>
#endif

static inline int banana_test_fail_if(int condition, const char *message)
{
    if (condition)
    {
        fprintf(stderr, "%s\n", message);
        return 1;
    }
    return 0;
}

typedef void (*banana_native_test_fn)(void **state);
typedef int (*banana_native_test_setup_fn)(void **state);
typedef int (*banana_native_test_teardown_fn)(void **state);

/* Usage pattern for new native tests:
 * 1) Write test functions as void test_name(void **state).
 * 2) Use BANANA_TEST_ASSERT_* helpers instead of framework-specific asserts.
 * 3) Register tests with BANANA_TEST_MAIN(...) and BANANA_TEST_CASE(...).
 * 4) For fixture-style tests, use BANANA_TEST_CASE_SETUP_TEARDOWN(...).
 * 5) For group setup/teardown hooks, use BANANA_TEST_MAIN_WITH_GROUP(...).
 */

#if defined(BANANA_USE_CMOCKA)

#define BANANA_TEST_CASE(test_fn) cmocka_unit_test(test_fn)
#define BANANA_TEST_CASE_SETUP_TEARDOWN(test_fn, setup_fn, teardown_fn)                            \
    cmocka_unit_test_setup_teardown(test_fn, setup_fn, teardown_fn)

#define BANANA_TEST_MAIN(...)                                                                       \
    int main(void)                                                                                  \
    {                                                                                               \
        const struct CMUnitTest tests[] = { __VA_ARGS__ };                                         \
        return cmocka_run_group_tests(tests, NULL, NULL);                                          \
    }

#define BANANA_TEST_MAIN_WITH_GROUP(group_setup_fn, group_teardown_fn, ...)                        \
    int main(void)                                                                                  \
    {                                                                                               \
        const struct CMUnitTest tests[] = { __VA_ARGS__ };                                         \
        return cmocka_run_group_tests(tests, group_setup_fn, group_teardown_fn);                  \
    }

#define BANANA_TEST_FAIL(message) fail_msg("%s", (message))
#define BANANA_TEST_ASSERT_TRUE(condition, message)                                                 \
    do                                                                                              \
    {                                                                                               \
        if (!(condition))                                                                           \
            fail_msg("%s", (message));                                                             \
    } while (0)

#define BANANA_TEST_ASSERT_INT_EQ(actual, expected, message)                                        \
    do                                                                                              \
    {                                                                                               \
        if ((actual) != (expected))                                                                 \
            fail_msg("%s", (message));                                                             \
    } while (0)

#define BANANA_TEST_ASSERT_STR_EQ(actual, expected, message)                                        \
    do                                                                                              \
    {                                                                                               \
        if (strcmp((actual), (expected)) != 0)                                                      \
            fail_msg("%s", (message));                                                             \
    } while (0)

#define BANANA_TEST_ASSERT_FLOAT_EQ(actual, expected, epsilon, message)                             \
    do                                                                                              \
    {                                                                                               \
        if (((actual) < ((expected) - (epsilon))) || ((actual) > ((expected) + (epsilon))))       \
            fail_msg("%s", (message));                                                             \
    } while (0)

#else

typedef struct BananaNativeTestCase
{
    const char *name;
    banana_native_test_fn fn;
    banana_native_test_setup_fn setup;
    banana_native_test_teardown_fn teardown;
} BananaNativeTestCase;

static int s_banana_native_failures = 0;

static inline void banana_native_record_failure(const char *message, const char *file, int line)
{
    fprintf(stderr, "[native-test] %s (%s:%d)\n", message, file, line);
    s_banana_native_failures += 1;
}

static inline int banana_native_run_tests(const BananaNativeTestCase *tests,
                                          size_t count,
                                          banana_native_test_setup_fn group_setup,
                                          banana_native_test_teardown_fn group_teardown)
{
    size_t index = 0;
    void *group_state = NULL;

    s_banana_native_failures = 0;

    if (group_setup && group_setup(&group_state) != 0)
    {
        banana_native_record_failure("group setup failed", __FILE__, __LINE__);
        return 1;
    }

    for (index = 0; index < count; ++index)
    {
        int failures_before = s_banana_native_failures;
        void *test_state = NULL;

        if (tests[index].setup && tests[index].setup(&test_state) != 0)
        {
            banana_native_record_failure("test setup failed", __FILE__, __LINE__);
            fprintf(stderr, "[native-test] FAIL %s\n", tests[index].name);
            continue;
        }

        tests[index].fn(&test_state);

        if (tests[index].teardown && tests[index].teardown(&test_state) != 0)
            banana_native_record_failure("test teardown failed", __FILE__, __LINE__);

        if (s_banana_native_failures == failures_before)
            fprintf(stderr, "[native-test] PASS %s\n", tests[index].name);
        else
            fprintf(stderr, "[native-test] FAIL %s\n", tests[index].name);
    }

    if (group_teardown && group_teardown(&group_state) != 0)
        banana_native_record_failure("group teardown failed", __FILE__, __LINE__);

    return (s_banana_native_failures == 0) ? 0 : 1;
}

#define BANANA_TEST_CASE(test_fn) { #test_fn, test_fn, NULL, NULL }
#define BANANA_TEST_CASE_SETUP_TEARDOWN(test_fn, setup_fn, teardown_fn)                            \
    { #test_fn, test_fn, setup_fn, teardown_fn }

#define BANANA_TEST_MAIN(...)                                                                       \
    int main(void)                                                                                  \
    {                                                                                               \
        const BananaNativeTestCase tests[] = { __VA_ARGS__ };                                      \
        return banana_native_run_tests(tests, sizeof(tests) / sizeof(tests[0]), NULL, NULL);      \
    }

#define BANANA_TEST_MAIN_WITH_GROUP(group_setup_fn, group_teardown_fn, ...)                        \
    int main(void)                                                                                  \
    {                                                                                               \
        const BananaNativeTestCase tests[] = { __VA_ARGS__ };                                      \
        return banana_native_run_tests(tests,                                                      \
                                      sizeof(tests) / sizeof(tests[0]),                            \
                                      group_setup_fn,                                               \
                                      group_teardown_fn);                                           \
    }

#define BANANA_TEST_FAIL(message)                                                                    \
    do                                                                                               \
    {                                                                                                \
        banana_native_record_failure((message), __FILE__, __LINE__);                                \
        return;                                                                                      \
    } while (0)

#define BANANA_TEST_ASSERT_TRUE(condition, message)                                                  \
    do                                                                                               \
    {                                                                                                \
        if (!(condition))                                                                            \
        {                                                                                            \
            banana_native_record_failure((message), __FILE__, __LINE__);                            \
            return;                                                                                  \
        }                                                                                            \
    } while (0)

#define BANANA_TEST_ASSERT_INT_EQ(actual, expected, message)                                         \
    BANANA_TEST_ASSERT_TRUE((actual) == (expected), (message))

#define BANANA_TEST_ASSERT_STR_EQ(actual, expected, message)                                         \
    BANANA_TEST_ASSERT_TRUE(strcmp((actual), (expected)) == 0, (message))

#define BANANA_TEST_ASSERT_FLOAT_EQ(actual, expected, epsilon, message)                              \
    BANANA_TEST_ASSERT_TRUE(((actual) >= ((expected) - (epsilon))) && ((actual) <= ((expected) + (epsilon))), \
                            (message))

#endif

#endif
