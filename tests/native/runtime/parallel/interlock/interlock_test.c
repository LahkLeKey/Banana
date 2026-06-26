#include "runtime/parallel/interlock/interlock.h"

#include "runtime/support/test_support.h"

static void test_interlock_init_destroy_null_guards(void **state)
{
    (void)state;
    RuntimeInterlockContext ctx = {0};

    BANANA_TEST_ASSERT_INT_EQ(runtime_interlock_init(NULL),
                              0,
                              "null context must return failure");
    BANANA_TEST_ASSERT_INT_EQ(runtime_interlock_contention_hits(NULL),
                              0,
                              "null context must return zero contention hits");

    runtime_interlock_destroy(NULL);    /* must not crash */

    ctx.initialized = 0;
    runtime_interlock_destroy(&ctx);    /* uninitialized must not crash */
}

static void test_interlock_lifecycle_and_contention(void **state)
{
    (void)state;
    RuntimeInterlockContext ctx = {0};

    BANANA_TEST_ASSERT_INT_EQ(runtime_interlock_init(&ctx),
                              1,
                              "valid context must initialize successfully");
    BANANA_TEST_ASSERT_INT_EQ(ctx.initialized,
                              1,
                              "initialized flag must be set after init");
    BANANA_TEST_ASSERT_INT_EQ(runtime_interlock_contention_hits(&ctx),
                              0,
                              "fresh interlock must report zero contention hits");

    /* Enter/leave from the same thread — no deadlock with OMP test_lock path */
    runtime_interlock_enter(&ctx);
    runtime_interlock_leave(&ctx);

    /* NULL-guard paths for enter/leave */
    runtime_interlock_enter(NULL);
    runtime_interlock_leave(NULL);

    ctx.initialized = 0;
    runtime_interlock_enter(&ctx);     /* uninitialized must not crash */
    runtime_interlock_leave(&ctx);

    ctx.initialized = 1;
    runtime_interlock_destroy(&ctx);

    BANANA_TEST_ASSERT_INT_EQ(ctx.initialized,
                              0,
                              "initialized flag must be cleared after destroy");
    BANANA_TEST_ASSERT_TRUE(ctx.native_lock == NULL,
                            "native lock pointer must be NULL after destroy");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_interlock_init_destroy_null_guards),
    BANANA_TEST_CASE(test_interlock_lifecycle_and_contention)
)
