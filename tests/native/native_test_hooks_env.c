#include "native_test_hooks.h"

#include <stdlib.h>

int cinterop_test_hook_force_summary_failure(void) {
    const char* flag = getenv("CINTEROP_FORCE_SUMMARY_FAIL");
    return flag != 0 && flag[0] == '1';
}

int cinterop_test_hook_try_forced_db_result(int* out_forced_result) {
    const char* forced = getenv("CINTEROP_FORCE_DB_RESULT");
    if (forced == 0 || forced[0] == '\0') {
        return 0;
    }

    if (out_forced_result != 0) {
        *out_forced_result = atoi(forced);
    }

    return 1;
}

int cinterop_test_hook_force_payload_alloc_failure(void) {
    const char* flag = getenv("CINTEROP_FORCE_PAYLOAD_ALLOC_FAIL");
    return flag != 0 && flag[0] == '1';
}

int cinterop_test_hook_force_payload_malloc_null(void) {
    const char* flag = getenv("CINTEROP_FORCE_PAYLOAD_MALLOC_NULL");
    return flag != 0 && flag[0] == '1';
}

int cinterop_test_hook_force_db_bad_result(void) {
    const char* flag = getenv("CINTEROP_FORCE_DB_BAD_RESULT");
    return flag != 0 && flag[0] == '1';
}

int cinterop_test_hook_force_db_status_mismatch(void) {
    const char* flag = getenv("CINTEROP_FORCE_DB_STATUS_MISMATCH");
    return flag != 0 && flag[0] == '1';
}

int cinterop_test_hook_force_message_alloc_failure(void) {
    const char* flag = getenv("CINTEROP_FORCE_MESSAGE_ALLOC_FAIL");
    return flag != 0 && flag[0] == '1';
}
