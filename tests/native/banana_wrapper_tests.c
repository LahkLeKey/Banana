#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "banana_wrapper.h"
#include "banana_db.h"

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
    int status = banana_calculate_banana(10, 2, &banana);
    require_true(status == BANANA_STATUS_OK, "expected status OK for calculate_banana");
    require_true(banana == 150, "expected banana=150 for calculate_banana");
}

static void test_calculate_banana_invalid(void) {
    int banana = 0;
    int status = banana_calculate_banana(-1, 2, &banana);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for negative purchases");
}

static void test_calculate_banana_overflow(void) {
    int banana = 0;
    int status = banana_calculate_banana(214748365, 1, &banana);
    require_true(status == BANANA_STATUS_OVERFLOW, "expected overflow for large purchases");
}

static void test_calculate_banana_overflow_bonus_term(void) {
    int banana = 0;
    int status = banana_calculate_banana(10, 85899346, &banana);
    require_true(status == BANANA_STATUS_OVERFLOW, "expected overflow when bonus term exceeds int range");
}

static void test_calculate_banana_overflow_total_sum(void) {
    int banana = 0;
    int status = banana_calculate_banana(214748364, 85899345, &banana);
    require_true(status == BANANA_STATUS_OVERFLOW, "expected overflow when base and bonus sum exceeds int range");
}

static void test_calculate_banana_null_pointer(void) {
    int status = banana_calculate_banana(10, 2, 0);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null out_banana");
}

static void test_create_banana_message_ok(void) {
    char* message = 0;
    int status = banana_create_banana_message(10, 2, &message);
    require_true(status == BANANA_STATUS_OK, "expected status OK for create_banana_message");
    require_true(message != 0, "expected message pointer");
    require_true(strstr(message, "banana_profile") != 0, "expected banana profile prefix in message");
    require_true(strstr(message, "cultivar=Cavendish") != 0, "expected cultivar token in message");
    require_true(strstr(message, "stage=YELLOW") != 0, "expected ripeness stage token in message");
    require_true(strstr(message, "banana=150") != 0, "expected banana token in message");
    banana_free(message);
}

static void test_create_banana_message_null_pointer(void) {
    int status = banana_create_banana_message(10, 2, 0);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null out_message");
}

static void test_create_banana_message_invalid_input(void) {
    char* message = 0;
    int status = banana_create_banana_message(-1, 2, &message);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for invalid create_banana_message input");
    require_true(message == 0, "expected null message when call fails");
}

static void test_breakdown_ok(void) {
    CInteropBananaBreakdown breakdown;
    int status = banana_calculate_banana_with_breakdown(10, 2, &breakdown);
    require_true(status == BANANA_STATUS_OK, "expected OK for breakdown call");
    require_true(breakdown.purchases == 10, "expected purchases=10 in breakdown");
    require_true(breakdown.multiplier == 2, "expected multiplier=2 in breakdown");
    require_true(breakdown.banana == 150, "expected banana=150 in breakdown");
}

static void test_breakdown_uses_shared_core_validation_for_large_values(void) {
    CInteropBananaBreakdown breakdown;
    int status = banana_calculate_banana_with_breakdown(214748365, 1, &breakdown);
    require_true(status == BANANA_STATUS_OVERFLOW, "expected overflow for breakdown when core validation rejects large values");
}

static void test_breakdown_invalid_input(void) {
    CInteropBananaBreakdown breakdown;
    int status = banana_calculate_banana_with_breakdown(-1, 2, &breakdown);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for negative breakdown input");
}

static void test_breakdown_internal_error(void) {
    CInteropBananaBreakdown breakdown;
    int status;

    require_true(SET_ENV("BANANA_FORCE_SUMMARY_FAIL", "1") == 0, "failed to set BANANA_FORCE_SUMMARY_FAIL");
    status = banana_calculate_banana_with_breakdown(10, 2, &breakdown);
    require_true(UNSET_ENV("BANANA_FORCE_SUMMARY_FAIL") == 0, "failed to unset BANANA_FORCE_SUMMARY_FAIL");
    require_true(status == BANANA_STATUS_INTERNAL_ERROR, "expected internal error for forced summary failure");
}

static void test_breakdown_null_pointer(void) {
    int status = banana_calculate_banana_with_breakdown(10, 2, 0);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null breakdown");
}

static void test_db_query_banana_ok(void) {
    char* payload = 0;
    int row_count = 0;
    int status = banana_db_query_banana(10, 2, &payload, &row_count);

    require_true(status == BANANA_STATUS_OK, "expected status OK for db_query_banana");
    require_true(payload != 0, "expected payload pointer");
    require_true(row_count == 1, "expected row_count=1");
    require_true(strstr(payload, "\"cultivar\":\"Cavendish\"") != 0, "expected cultivar in payload");
    require_true(strstr(payload, "\"predicted_stage\":\"YELLOW\"") != 0, "expected ripeness stage in payload");
    require_true(strstr(payload, "\"shelf_life_hours\":") != 0, "expected shelf life in payload");
    require_true(strstr(payload, "\"banana\":150") != 0, "expected banana token in payload");

    banana_free(payload);
}

static void test_predict_banana_ripeness_ok(void) {
    double history[3] = { 12.5, 13.0, 14.2 };
    CInteropBananaRipenessPrediction prediction;
    int status = banana_predict_banana_ripeness(history, 3, 5, 2.5, 0.1, 13.2, &prediction);

    require_true(status == BANANA_STATUS_OK, "expected OK for ripeness prediction");
    require_true(prediction.predicted_stage >= 0, "expected a valid predicted stage");
    require_true(prediction.shelf_life_hours > 0, "expected positive shelf life hours");
    require_true(prediction.ripening_index > 0.0, "expected positive ripening index");
}

static void test_predict_banana_ripeness_invalid_args(void) {
    double history[1] = { 13.0 };
    int status = banana_predict_banana_ripeness(history, 1, -1, 0.0, 0.0, 13.0, 0);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null prediction");

    status = banana_predict_banana_ripeness(history, 1, -1, 0.0, 0.0, 13.0, (CInteropBananaRipenessPrediction*)&history[0]);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for invalid ripeness input");
}

static void test_batch_create_and_status_ok(void) {
    char* payload = 0;
    char* status_payload = 0;
    int status = banana_create_batch("batch-1", "farm-1", 13.2, 2.5, 3, &payload);

    require_true(status == BANANA_STATUS_OK, "expected OK for create_batch");
    require_true(payload != 0, "expected payload for create_batch");
    require_true(strstr(payload, "\"batchId\":\"batch-1\"") != 0, "expected batchId in batch payload");
    banana_free(payload);

    status = banana_get_batch_status("batch-1", &status_payload);
    require_true(status == BANANA_STATUS_OK, "expected OK for get_batch_status");
    require_true(status_payload != 0, "expected payload for get_batch_status");
    require_true(strstr(status_payload, "\"originFarm\":\"farm-1\"") != 0, "expected originFarm in status payload");
    banana_free(status_payload);
}

static void test_predict_batch_ripeness_ok(void) {
    double history[3] = { 12.5, 13.0, 14.2 };
    CInteropBananaRipenessPrediction prediction;
    int status = banana_predict_batch_ripeness("batch-1", history, 3, 5, 0.1, &prediction);

    require_true(status == BANANA_STATUS_OK, "expected OK for batch ripeness prediction");
    require_true(prediction.shelf_life_hours > 0, "expected positive shelf life for batch ripeness prediction");
}

static void test_batch_not_found_paths(void) {
    char* payload = 0;
    CInteropBananaRipenessPrediction prediction;
    int status = banana_get_batch_status("missing-batch", &payload);
    require_true(status == BANANA_STATUS_NOT_FOUND, "expected NOT_FOUND for missing batch status");

    status = banana_predict_batch_ripeness("missing-batch", 0, 0, 1, 0.0, &prediction);
    require_true(status == BANANA_STATUS_NOT_FOUND, "expected NOT_FOUND for missing batch ripeness");
}

static void test_db_query_banana_invalid_args(void) {
    int row_count = 0;
    char* payload = 0;
    int status = banana_db_query_banana(10, 2, 0, &row_count);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null payload output");

    status = banana_db_query_banana(10, 2, &payload, 0);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null row_count output");
}

static void test_db_query_banana_invalid_input(void) {
    char* payload = 0;
    int row_count = 0;
    int status = banana_db_query_banana(-1, 2, &payload, &row_count);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for negative db_query_banana input");
}

static void test_db_query_banana_forced_not_configured(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_DB_RESULT", "2") == 0, "failed to set BANANA_FORCE_DB_RESULT=2");
    status = banana_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("BANANA_FORCE_DB_RESULT") == 0, "failed to unset BANANA_FORCE_DB_RESULT");
    require_true(status == BANANA_STATUS_DB_NOT_CONFIGURED, "expected DB_NOT_CONFIGURED for forced db result 2");
}

static void test_db_query_banana_forced_db_error(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_DB_RESULT", "3") == 0, "failed to set BANANA_FORCE_DB_RESULT=3");
    status = banana_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("BANANA_FORCE_DB_RESULT") == 0, "failed to unset BANANA_FORCE_DB_RESULT");
    require_true(status == BANANA_STATUS_DB_ERROR, "expected DB_ERROR for forced db result 3");
}

static void test_db_query_banana_missing_connection(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(UNSET_ENV("BANANA_PG_CONNECTION") == 0, "failed to unset BANANA_PG_CONNECTION");
    status = banana_db_query_banana(10, 2, &payload, &row_count);
#if defined(BANANA_ENABLE_POSTGRES)
    require_true(status == BANANA_STATUS_DB_NOT_CONFIGURED, "expected DB_NOT_CONFIGURED when connection string is missing");
#else
    require_true(status == BANANA_STATUS_OK, "expected OK when PostgreSQL support is disabled");
#endif
    require_true(SET_ENV("BANANA_PG_CONNECTION", "host=postgres port=5432 user=cinterop password=cinterop dbname=cinterop") == 0, "failed to restore BANANA_PG_CONNECTION");
}

static void test_db_query_banana_bad_connection(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_PG_CONNECTION", "host=invalid-host-name port=5432 user=cinterop password=cinterop dbname=cinterop connect_timeout=1") == 0, "failed to set bad BANANA_PG_CONNECTION");
    status = banana_db_query_banana(10, 2, &payload, &row_count);
#if defined(BANANA_ENABLE_POSTGRES)
    require_true(status == BANANA_STATUS_DB_ERROR, "expected DB_ERROR when connection fails");
#else
    require_true(status == BANANA_STATUS_OK, "expected OK when PostgreSQL support is disabled");
#endif
    require_true(SET_ENV("BANANA_PG_CONNECTION", "host=postgres port=5432 user=cinterop password=cinterop dbname=cinterop") == 0, "failed to restore BANANA_PG_CONNECTION");
}

static void test_db_query_banana_forced_bad_result_branch(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_DB_BAD_RESULT", "1") == 0, "failed to set BANANA_FORCE_DB_BAD_RESULT");
    status = banana_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("BANANA_FORCE_DB_BAD_RESULT") == 0, "failed to unset BANANA_FORCE_DB_BAD_RESULT");
#if defined(BANANA_ENABLE_POSTGRES)
    require_true(status == BANANA_STATUS_DB_ERROR, "expected DB_ERROR for forced bad query result branch");
#else
    require_true(status == BANANA_STATUS_OK, "expected OK for forced bad query result branch when PostgreSQL support is disabled");
#endif
}

static void test_db_query_banana_forced_status_mismatch_branch(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_DB_STATUS_MISMATCH", "1") == 0, "failed to set BANANA_FORCE_DB_STATUS_MISMATCH");
    status = banana_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("BANANA_FORCE_DB_STATUS_MISMATCH") == 0, "failed to unset BANANA_FORCE_DB_STATUS_MISMATCH");
#if defined(BANANA_ENABLE_POSTGRES)
    require_true(status == BANANA_STATUS_DB_ERROR, "expected DB_ERROR for forced status mismatch branch");
#else
    require_true(status == BANANA_STATUS_OK, "expected OK for forced status mismatch branch when PostgreSQL support is disabled");
#endif
}

static void test_db_query_banana_forced_internal_error(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_DB_RESULT", "9") == 0, "failed to set BANANA_FORCE_DB_RESULT=9");
    status = banana_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("BANANA_FORCE_DB_RESULT") == 0, "failed to unset BANANA_FORCE_DB_RESULT");
    require_true(status == BANANA_STATUS_INTERNAL_ERROR, "expected INTERNAL_ERROR for forced db result 9");
}

static void test_db_query_banana_forced_payload_alloc_fail(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_PAYLOAD_ALLOC_FAIL", "1") == 0, "failed to set BANANA_FORCE_PAYLOAD_ALLOC_FAIL");
    status = banana_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("BANANA_FORCE_PAYLOAD_ALLOC_FAIL") == 0, "failed to unset BANANA_FORCE_PAYLOAD_ALLOC_FAIL");
    require_true(status == BANANA_STATUS_DB_ERROR, "expected DB_ERROR when payload allocation is forced to fail");
}

static void test_db_query_banana_forced_payload_malloc_null(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_PAYLOAD_MALLOC_NULL", "1") == 0, "failed to set BANANA_FORCE_PAYLOAD_MALLOC_NULL");
    status = banana_db_query_banana(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("BANANA_FORCE_PAYLOAD_MALLOC_NULL") == 0, "failed to unset BANANA_FORCE_PAYLOAD_MALLOC_NULL");
    require_true(status == BANANA_STATUS_DB_ERROR, "expected DB_ERROR when payload malloc null is forced");
}

static void test_create_banana_message_forced_alloc_failure(void) {
    char* message = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_MESSAGE_ALLOC_FAIL", "1") == 0, "failed to set BANANA_FORCE_MESSAGE_ALLOC_FAIL");
    status = banana_create_banana_message(10, 2, &message);
    require_true(UNSET_ENV("BANANA_FORCE_MESSAGE_ALLOC_FAIL") == 0, "failed to unset BANANA_FORCE_MESSAGE_ALLOC_FAIL");
    require_true(status == BANANA_STATUS_INTERNAL_ERROR, "expected INTERNAL_ERROR when message alloc is forced to fail");
}

static void test_dal_db_direct_null_output_validation(void) {
    int row_count = 0;
    char* payload = 0;
    int status = banana_db_query(1, 1, 0, &row_count);
    require_true(status == 1, "expected direct banana_db_query null payload status");

    status = banana_db_query(1, 1, &payload, 0);
    require_true(status == 1, "expected direct banana_db_query null row_count status");
}

static void test_free_null_is_safe(void) {
    banana_free(0);
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
    test_breakdown_uses_shared_core_validation_for_large_values();
    test_breakdown_invalid_input();
    test_breakdown_null_pointer();
    test_breakdown_internal_error();
    test_db_query_banana_ok();
    test_predict_banana_ripeness_ok();
    test_predict_banana_ripeness_invalid_args();
    test_batch_create_and_status_ok();
    test_predict_batch_ripeness_ok();
    test_batch_not_found_paths();
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
    test_dal_db_direct_null_output_validation();
    test_free_null_is_safe();

    puts("native_wrapper_tests: all tests passed");
    return 0;
}
