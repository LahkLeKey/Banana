#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cinterop_wrapper.h"
#include "legacy_db.h"
#include "legacy_banana.h"

#if defined(_WIN32)
#include <stdlib.h>
#define SET_ENV(k, v) _putenv_s((k), (v))
#define UNSET_ENV(k) _putenv_s((k), "")
#else
#include <unistd.h>
#define SET_ENV(k, v) setenv((k), (v), 1)
#define UNSET_ENV(k) unsetenv((k))
#endif

static void require_true(int condition, const char* message) {
    if (!condition) {
        fprintf(stderr, "native_wrapper_tests failure: %s\n", message);
        exit(1);
    }
}

static void test_calculate_banana_ok(void) {
    int banana = 0;
    int status = cinterop_calculate_banana(10, 2, &banana);
    require_true(status == CINTEROP_STATUS_OK, "expected status OK for calculate_banana");
    require_true(banana == 150, "expected banana=150 for calculate_banana");
}

static void test_calculate_banana_invalid(void) {
    int banana = 0;
    int status = cinterop_calculate_banana(-1, 2, &banana);
    require_true(status == CINTEROP_STATUS_INVALID_ARGUMENT, "expected invalid argument for negative purchases");
}

static void test_calculate_banana_overflow(void) {
    int banana = 0;
    int status = cinterop_calculate_banana(214748365, 1, &banana);
    require_true(status == CINTEROP_STATUS_OVERFLOW, "expected overflow for large purchases");
}

static void test_calculate_banana_overflow_bonus_term(void) {
    int banana = 0;
    int status = cinterop_calculate_banana(10, 85899346, &banana);
    require_true(status == CINTEROP_STATUS_OVERFLOW, "expected overflow when bonus term exceeds int range");
}

static void test_calculate_banana_overflow_total_sum(void) {
    int banana = 0;
    int status = cinterop_calculate_banana(214748364, 85899345, &banana);
    require_true(status == CINTEROP_STATUS_OVERFLOW, "expected overflow when base and bonus sum exceeds int range");
}

static void test_calculate_banana_null_pointer(void) {
    int status = cinterop_calculate_banana(10, 2, 0);
    require_true(status == CINTEROP_STATUS_INVALID_ARGUMENT, "expected invalid argument for null out_banana");
}

static void test_create_banana_message_ok(void) {
    char* message = 0;
    int status = cinterop_create_banana_message(10, 2, &message);
    require_true(status == CINTEROP_STATUS_OK, "expected status OK for create_banana_message");
    require_true(message != 0, "expected message pointer");
    require_true(strstr(message, "banana=150") != 0, "expected banana token in message");
    cinterop_free(message);
}

static void test_create_banana_message_null_pointer(void) {
    int status = cinterop_create_banana_message(10, 2, 0);
    require_true(status == CINTEROP_STATUS_INVALID_ARGUMENT, "expected invalid argument for null out_message");
}

static void test_create_banana_message_invalid_input(void) {
    char* message = 0;
    int status = cinterop_create_banana_message(-1, 2, &message);
    require_true(status == CINTEROP_STATUS_INVALID_ARGUMENT, "expected invalid argument for invalid create_banana_message input");
    require_true(message == 0, "expected null message when call fails");
}

static void test_breakdown_ok(void) {
    CInteropBananaBreakdown breakdown;
    int status = cinterop_calculate_banana_with_breakdown(10, 2, &breakdown);
    require_true(status == CINTEROP_STATUS_OK, "expected OK for breakdown call");
    require_true(breakdown.purchases == 10, "expected purchases=10 in breakdown");
    require_true(breakdown.multiplier == 2, "expected multiplier=2 in breakdown");
    require_true(breakdown.banana == 150, "expected banana=150 in breakdown");
}

static void test_breakdown_invalid_input(void) {
    CInteropBananaBreakdown breakdown;
    int status = cinterop_calculate_banana_with_breakdown(-1, 2, &breakdown);
    require_true(status == CINTEROP_STATUS_INVALID_ARGUMENT, "expected invalid argument for negative breakdown input");
}

static void test_breakdown_internal_error(void) {
    CInteropBananaBreakdown breakdown;
    int status;

    require_true(SET_ENV("CINTEROP_FORCE_SUMMARY_FAIL", "1") == 0, "failed to set CINTEROP_FORCE_SUMMARY_FAIL");
    status = cinterop_calculate_banana_with_breakdown(10, 2, &breakdown);
    require_true(UNSET_ENV("CINTEROP_FORCE_SUMMARY_FAIL") == 0, "failed to unset CINTEROP_FORCE_SUMMARY_FAIL");
    require_true(status == CINTEROP_STATUS_INTERNAL_ERROR, "expected internal error for forced summary failure");
}

static void test_breakdown_null_pointer(void) {
    int status = cinterop_calculate_banana_with_breakdown(10, 2, 0);
    require_true(status == CINTEROP_STATUS_INVALID_ARGUMENT, "expected invalid argument for null breakdown");
}

static void test_db_query_banana_ok(void) {
    char* payload = 0;
    int row_count = 0;
    int status = cinterop_db_query_banana(10, 2, &payload, &row_count);

    require_true(status == CINTEROP_STATUS_OK, "expected status OK for db_query_banana");
    require_true(payload != 0, "expected payload pointer");
    require_true(row_count == 1, "expected row_count=1");
    require_true(strstr(payload, "\"banana\":150") != 0, "expected banana token in payload");

    cinterop_free(payload);
}

static void test_db_query_banana_invalid_args(void) {
    int row_count = 0;
    char* payload = 0;
    int status = cinterop_db_query_banana(10, 2, 0, &row_count);
    require_true(status == CINTEROP_STATUS_INVALID_ARGUMENT, "expected invalid argument for null payload output");

    status = cinterop_db_query_banana(10, 2, &payload, 0);
    require_true(status == CINTEROP_STATUS_INVALID_ARGUMENT, "expected invalid argument for null row_count output");
}

static void test_db_query_banana_invalid_input(void) {
    char* payload = 0;
    int row_count = 0;
    int status = cinterop_db_query_banana(-1, 2, &payload, &row_count);
    require_true(status == CINTEROP_STATUS_INVALID_ARGUMENT, "expected invalid argument for negative db_query_banana input");
}

static void test_db_query_banana_forced_not_configured(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("CINTEROP_FORCE_DB_RESULT", "2") == 0, "failed to set CINTEROP_FORCE_DB_RESULT=2");
    status = cinterop_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("CINTEROP_FORCE_DB_RESULT") == 0, "failed to unset CINTEROP_FORCE_DB_RESULT");
    require_true(status == CINTEROP_STATUS_DB_NOT_CONFIGURED, "expected DB_NOT_CONFIGURED for forced db result 2");
}

static void test_db_query_banana_forced_db_error(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("CINTEROP_FORCE_DB_RESULT", "3") == 0, "failed to set CINTEROP_FORCE_DB_RESULT=3");
    status = cinterop_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("CINTEROP_FORCE_DB_RESULT") == 0, "failed to unset CINTEROP_FORCE_DB_RESULT");
    require_true(status == CINTEROP_STATUS_DB_ERROR, "expected DB_ERROR for forced db result 3");
}

static void test_db_query_banana_missing_connection(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(UNSET_ENV("CINTEROP_PG_CONNECTION") == 0, "failed to unset CINTEROP_PG_CONNECTION");
    status = cinterop_db_query_banana(10, 2, &payload, &row_count);
#if defined(CINTEROP_ENABLE_POSTGRES)
    require_true(status == CINTEROP_STATUS_DB_NOT_CONFIGURED, "expected DB_NOT_CONFIGURED when connection string is missing");
#else
    require_true(status == CINTEROP_STATUS_OK, "expected OK when PostgreSQL support is disabled");
#endif
    require_true(SET_ENV("CINTEROP_PG_CONNECTION", "host=postgres port=5432 user=cinterop password=cinterop dbname=cinterop") == 0, "failed to restore CINTEROP_PG_CONNECTION");
}

static void test_db_query_banana_bad_connection(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("CINTEROP_PG_CONNECTION", "host=invalid-host-name port=5432 user=cinterop password=cinterop dbname=cinterop connect_timeout=1") == 0, "failed to set bad CINTEROP_PG_CONNECTION");
    status = cinterop_db_query_banana(10, 2, &payload, &row_count);
#if defined(CINTEROP_ENABLE_POSTGRES)
    require_true(status == CINTEROP_STATUS_DB_ERROR, "expected DB_ERROR when connection fails");
#else
    require_true(status == CINTEROP_STATUS_OK, "expected OK when PostgreSQL support is disabled");
#endif
    require_true(SET_ENV("CINTEROP_PG_CONNECTION", "host=postgres port=5432 user=cinterop password=cinterop dbname=cinterop") == 0, "failed to restore CINTEROP_PG_CONNECTION");
}

static void test_db_query_banana_forced_bad_result_branch(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("CINTEROP_FORCE_DB_BAD_RESULT", "1") == 0, "failed to set CINTEROP_FORCE_DB_BAD_RESULT");
    status = cinterop_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("CINTEROP_FORCE_DB_BAD_RESULT") == 0, "failed to unset CINTEROP_FORCE_DB_BAD_RESULT");
#if defined(CINTEROP_ENABLE_POSTGRES)
    require_true(status == CINTEROP_STATUS_DB_ERROR, "expected DB_ERROR for forced bad query result branch");
#else
    require_true(status == CINTEROP_STATUS_OK, "expected OK for forced bad query result branch when PostgreSQL support is disabled");
#endif
}

static void test_db_query_banana_forced_status_mismatch_branch(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("CINTEROP_FORCE_DB_STATUS_MISMATCH", "1") == 0, "failed to set CINTEROP_FORCE_DB_STATUS_MISMATCH");
    status = cinterop_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("CINTEROP_FORCE_DB_STATUS_MISMATCH") == 0, "failed to unset CINTEROP_FORCE_DB_STATUS_MISMATCH");
#if defined(CINTEROP_ENABLE_POSTGRES)
    require_true(status == CINTEROP_STATUS_DB_ERROR, "expected DB_ERROR for forced status mismatch branch");
#else
    require_true(status == CINTEROP_STATUS_OK, "expected OK for forced status mismatch branch when PostgreSQL support is disabled");
#endif
}

static void test_db_query_banana_forced_internal_error(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("CINTEROP_FORCE_DB_RESULT", "9") == 0, "failed to set CINTEROP_FORCE_DB_RESULT=9");
    status = cinterop_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("CINTEROP_FORCE_DB_RESULT") == 0, "failed to unset CINTEROP_FORCE_DB_RESULT");
    require_true(status == CINTEROP_STATUS_INTERNAL_ERROR, "expected INTERNAL_ERROR for forced db result 9");
}

static void test_db_query_banana_forced_payload_alloc_fail(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("CINTEROP_FORCE_PAYLOAD_ALLOC_FAIL", "1") == 0, "failed to set CINTEROP_FORCE_PAYLOAD_ALLOC_FAIL");
    status = cinterop_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("CINTEROP_FORCE_PAYLOAD_ALLOC_FAIL") == 0, "failed to unset CINTEROP_FORCE_PAYLOAD_ALLOC_FAIL");
    require_true(status == CINTEROP_STATUS_DB_ERROR, "expected DB_ERROR when payload allocation is forced to fail");
}

static void test_db_query_banana_forced_payload_malloc_null(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("CINTEROP_FORCE_PAYLOAD_MALLOC_NULL", "1") == 0, "failed to set CINTEROP_FORCE_PAYLOAD_MALLOC_NULL");
    status = cinterop_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("CINTEROP_FORCE_PAYLOAD_MALLOC_NULL") == 0, "failed to unset CINTEROP_FORCE_PAYLOAD_MALLOC_NULL");
    require_true(status == CINTEROP_STATUS_DB_ERROR, "expected DB_ERROR when payload malloc null is forced");
}

static void test_create_banana_message_forced_alloc_failure(void) {
    char* message = 0;
    int status;

    require_true(SET_ENV("CINTEROP_FORCE_MESSAGE_ALLOC_FAIL", "1") == 0, "failed to set CINTEROP_FORCE_MESSAGE_ALLOC_FAIL");
    status = cinterop_create_banana_message(10, 2, &message);
    require_true(UNSET_ENV("CINTEROP_FORCE_MESSAGE_ALLOC_FAIL") == 0, "failed to unset CINTEROP_FORCE_MESSAGE_ALLOC_FAIL");
    require_true(status == CINTEROP_STATUS_INTERNAL_ERROR, "expected INTERNAL_ERROR when message alloc is forced to fail");
}

static void test_legacy_summary_direct_null_pointer(void) {
    int status = legacy_calculate_summary(10, 2, 0);
    require_true(status == -1, "expected legacy_calculate_summary null pointer failure");
}

static void test_legacy_db_direct_null_output_validation(void) {
    int row_count = 0;
    char* payload = 0;
    int status = legacy_db_query_banana(1, 1, 0, &row_count);
    require_true(status == 1, "expected direct legacy_db_query_banana null payload status");

    status = legacy_db_query_banana(1, 1, &payload, 0);
    require_true(status == 1, "expected direct legacy_db_query_banana null row_count status");
}

static void test_free_null_is_safe(void) {
    cinterop_free(0);
}

int main(void) {
    test_calculate_banana_ok();
    test_calculate_banana_invalid();
    test_calculate_banana_overflow();
    test_calculate_banana_overflow_bonus_term();
    test_calculate_banana_overflow_total_sum();
    test_calculate_banana_null_pointer();
    test_create_banana_message_ok();
    test_create_banana_message_null_pointer();
    test_create_banana_message_invalid_input();
    test_create_banana_message_forced_alloc_failure();
    test_breakdown_ok();
    test_breakdown_invalid_input();
    test_breakdown_null_pointer();
    test_breakdown_internal_error();
    test_db_query_banana_ok();
    test_db_query_banana_invalid_args();
    test_db_query_banana_invalid_input();
    test_db_query_banana_missing_connection();
    test_db_query_banana_bad_connection();
    test_db_query_banana_forced_not_configured();
    test_db_query_banana_forced_db_error();
    test_db_query_banana_forced_bad_result_branch();
    test_db_query_banana_forced_status_mismatch_branch();
    test_db_query_banana_forced_internal_error();
    test_db_query_banana_forced_payload_alloc_fail();
    test_db_query_banana_forced_payload_malloc_null();
    test_legacy_db_direct_null_output_validation();
    test_legacy_summary_direct_null_pointer();
    test_free_null_is_safe();

    puts("native_wrapper_tests: all tests passed");
    return 0;
}
