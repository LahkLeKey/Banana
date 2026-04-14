#include "native_test_hooks.h"

int banana_test_hook_force_summary_failure(void) {
    return 0;
}

int banana_test_hook_try_forced_db_result(int* out_forced_result) {
    (void)out_forced_result;
    return 0;
}

int banana_test_hook_force_payload_alloc_failure(void) {
    return 0;
}

int banana_test_hook_force_payload_malloc_null(void) {
    return 0;
}

int banana_test_hook_force_db_bad_result(void) {
    return 0;
}

int banana_test_hook_force_db_status_mismatch(void) {
    return 0;
}

int banana_test_hook_force_message_alloc_failure(void) {
    return 0;
}
