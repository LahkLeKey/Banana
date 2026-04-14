#include <stdio.h>
#include <stdlib.h>

#include "native_test_hooks.h"

static void require_true(int condition, const char* message) {
    if (!condition) {
        fprintf(stderr, "native_test_hooks_default_tests failure: %s\n", message);
        exit(1);
    }
}

int main(void) {
    int forced_result = 123;

    require_true(banana_test_hook_force_summary_failure() == 0, "summary hook should be disabled by default");
    require_true(banana_test_hook_try_forced_db_result(&forced_result) == 0, "forced DB result hook should be disabled by default");
    require_true(forced_result == 123, "forced DB result output should remain untouched");
    require_true(banana_test_hook_force_payload_alloc_failure() == 0, "payload alloc failure hook should be disabled by default");
    require_true(banana_test_hook_force_payload_malloc_null() == 0, "payload malloc null hook should be disabled by default");
    require_true(banana_test_hook_force_db_bad_result() == 0, "db bad result hook should be disabled by default");
    require_true(banana_test_hook_force_db_status_mismatch() == 0, "db status mismatch hook should be disabled by default");
    require_true(banana_test_hook_force_message_alloc_failure() == 0, "message alloc failure hook should be disabled by default");

    puts("native_test_hooks_default_tests: all tests passed");
    return 0;
}