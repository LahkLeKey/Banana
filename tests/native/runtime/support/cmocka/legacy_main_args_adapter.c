#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef main
#undef main
#endif

static int g_argc;
static char **g_argv;

extern int banana_legacy_main_args(int argc, char **argv);

static void test_legacy_main_returns_zero(void **state)
{
    (void)state;
    assert_int_equal(banana_legacy_main_args(g_argc, g_argv), 0);
}

int main(int argc, char **argv)
{
    g_argc = argc;
    g_argv = argv;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_legacy_main_returns_zero),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
