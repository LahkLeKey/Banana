#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>

#ifdef main
#undef main
#endif

extern int banana_legacy_main_void(void);

static void test_legacy_main_returns_zero(void **state)
{
    (void)state;
    assert_int_equal(banana_legacy_main_void(), 0);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_legacy_main_returns_zero),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
