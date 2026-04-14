#ifndef BANANA_NATIVE_TEST_HOOKS_H
#define BANANA_NATIVE_TEST_HOOKS_H

int banana_test_hook_force_summary_failure(void);
int banana_test_hook_try_forced_db_result(int* out_forced_result);
int banana_test_hook_force_payload_alloc_failure(void);
int banana_test_hook_force_payload_malloc_null(void);
int banana_test_hook_force_db_bad_result(void);
int banana_test_hook_force_db_status_mismatch(void);
int banana_test_hook_force_message_alloc_failure(void);

#endif
