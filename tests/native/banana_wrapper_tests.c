#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "banana_wrapper.h"
#include "banana_db.h"
#include "internal/banana_postgres.h"

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

static double absolute_difference(double left, double right) {
    return left > right ? left - right : right - left;
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

static void test_predict_banana_regression_score_ok(void) {
    double features[CINTEROP_BANANA_ML_FEATURE_COUNT] = {
        0.82,
        0.14,
        0.76,
        0.43,
        0.05,
        0.61,
        0.72,
        0.18
    };
    double score = 0.0;
    int status = banana_predict_banana_regression_score(
        features,
        CINTEROP_BANANA_ML_FEATURE_COUNT,
        &score);

    require_true(status == BANANA_STATUS_OK, "expected OK for regression score prediction");
    require_true(score > 0.7905 && score < 0.7907, "expected deterministic regression score");
}

static void test_predict_banana_regression_score_invalid_args(void) {
    double features[CINTEROP_BANANA_ML_FEATURE_COUNT] = { 0.0 };
    double score = 0.0;
    int status = banana_predict_banana_regression_score(0, CINTEROP_BANANA_ML_FEATURE_COUNT, &score);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null regression features");

    status = banana_predict_banana_regression_score(features, CINTEROP_BANANA_ML_FEATURE_COUNT - 1, &score);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for wrong regression feature count");

    status = banana_predict_banana_regression_score(features, CINTEROP_BANANA_ML_FEATURE_COUNT, 0);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null regression score output");
}

static void test_classify_banana_binary_ok(void) {
    double banana_features[CINTEROP_BANANA_ML_FEATURE_COUNT] = {
        0.82,
        0.14,
        0.76,
        0.43,
        0.05,
        0.61,
        0.72,
        0.18
    };
    double not_banana_features[CINTEROP_BANANA_ML_FEATURE_COUNT] = {
        0.12,
        0.81,
        0.17,
        0.09,
        0.74,
        0.18,
        0.22,
        0.66
    };
    CInteropBananaMlBinaryClassification classification;
    int status = banana_classify_banana_binary(
        banana_features,
        CINTEROP_BANANA_ML_FEATURE_COUNT,
        &classification);

    require_true(status == BANANA_STATUS_OK, "expected OK for banana-like binary classification sample");
    require_true(classification.predicted_label == CINTEROP_BANANA_ML_LABEL_BANANA, "expected banana label for banana-like binary sample");
    require_true(classification.banana_probability > 0.80, "expected strong banana probability for banana-like binary sample");

    status = banana_classify_banana_binary(
        not_banana_features,
        CINTEROP_BANANA_ML_FEATURE_COUNT,
        &classification);
    require_true(status == BANANA_STATUS_OK, "expected OK for non-banana binary classification sample");
    require_true(classification.predicted_label == CINTEROP_BANANA_ML_LABEL_NOT_BANANA, "expected not-banana label for non-banana binary sample");
    require_true(classification.banana_probability < 0.20, "expected low banana probability for non-banana binary sample");
    require_true(absolute_difference(
            classification.banana_probability + classification.not_banana_probability,
            1.0) < 0.000001,
        "expected binary probabilities to sum to one");
}

static void test_classify_banana_binary_invalid_args(void) {
    double features[CINTEROP_BANANA_ML_FEATURE_COUNT] = { 0.0 };
    CInteropBananaMlBinaryClassification classification;
    int status = banana_classify_banana_binary(0, CINTEROP_BANANA_ML_FEATURE_COUNT, &classification);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null binary features");

    status = banana_classify_banana_binary(features, CINTEROP_BANANA_ML_FEATURE_COUNT - 1, &classification);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for wrong binary feature count");

    status = banana_classify_banana_binary(features, CINTEROP_BANANA_ML_FEATURE_COUNT, 0);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null binary classification output");
}

static void test_classify_banana_transformer_ok(void) {
    double banana_tokens[] = {
        0.82, 0.15, 0.76, 0.18,
        0.79, 0.12, 0.74, 0.16,
        0.84, 0.11, 0.78, 0.14
    };
    double not_banana_tokens[] = {
        0.18, 0.82, 0.22, 0.79,
        0.16, 0.85, 0.20, 0.81,
        0.20, 0.78, 0.24, 0.76
    };
    CInteropBananaMlTransformerClassification classification;
    int status = banana_classify_banana_transformer(
        banana_tokens,
        (int)(sizeof(banana_tokens) / sizeof(double)),
        &classification);

    require_true(status == BANANA_STATUS_OK, "expected OK for transformer banana-like sequence");
    require_true(classification.predicted_label == CINTEROP_BANANA_ML_LABEL_BANANA, "expected banana label for transformer banana-like sequence");
    require_true(classification.banana_probability > 0.95, "expected high banana probability for transformer banana-like sequence");
    require_true(classification.attention_focus > 0.0 && classification.attention_focus <= 1.0, "expected normalized attention focus for transformer sequence");

    status = banana_classify_banana_transformer(
        not_banana_tokens,
        (int)(sizeof(not_banana_tokens) / sizeof(double)),
        &classification);
    require_true(status == BANANA_STATUS_OK, "expected OK for transformer non-banana sequence");
    require_true(classification.predicted_label == CINTEROP_BANANA_ML_LABEL_NOT_BANANA, "expected not-banana label for transformer non-banana sequence");
    require_true(classification.banana_probability < 0.05, "expected low banana probability for transformer non-banana sequence");
}

static void test_classify_banana_transformer_invalid_args(void) {
    double token_values[CINTEROP_BANANA_ML_TOKEN_FEATURE_COUNT * 2] = {
        0.2, 0.1, 0.3, 0.4,
        0.4, 0.3, 0.2, 0.1
    };
    double over_limit_values[CINTEROP_BANANA_ML_TOKEN_FEATURE_COUNT * (CINTEROP_BANANA_ML_MAX_SEQUENCE_LENGTH + 1)] = { 0.0 };
    CInteropBananaMlTransformerClassification classification;
    int status = banana_classify_banana_transformer(0, 8, &classification);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null transformer token values");

    status = banana_classify_banana_transformer(token_values, 0, &classification);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for empty transformer token sequence");

    status = banana_classify_banana_transformer(token_values, 7, &classification);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for non-aligned transformer token value count");

    status = banana_classify_banana_transformer(
        over_limit_values,
        (int)(sizeof(over_limit_values) / sizeof(double)),
        &classification);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for transformer sequence exceeding max length");

    status = banana_classify_banana_transformer(token_values, 8, 0);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null transformer output");
}

static void test_db_query_banana_ok(void) {
    char* payload = 0;
    int row_count = 0;
    int status = banana_db_query_banana_profile(10, 2, &payload, &row_count);

    require_true(status == BANANA_STATUS_OK, "expected status OK for db_query_banana_profile");
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

static void test_harvest_batch_create_add_and_status_ok(void) {
    char* payload = 0;
    char* updated_payload = 0;
    char* status_payload = 0;
    int status = banana_create_harvest_batch("harvest-1", "field-7", 42, &payload);

    require_true(status == BANANA_STATUS_OK, "expected OK for create_harvest_batch");
    require_true(payload != 0, "expected payload for create_harvest_batch");
    require_true(strstr(payload, "\"harvestBatchId\":\"harvest-1\"") != 0, "expected harvestBatchId in harvest batch payload");
    banana_free(payload);

    status = banana_add_bunch_to_harvest_batch("harvest-1", "bunch-1", 42, 18.5, &updated_payload);
    require_true(status == BANANA_STATUS_OK, "expected OK for add_bunch_to_harvest_batch");
    require_true(updated_payload != 0, "expected payload for add_bunch_to_harvest_batch");
    require_true(strstr(updated_payload, "\"bunchCount\":1") != 0, "expected bunchCount=1 after add_bunch_to_harvest_batch");
    require_true(strstr(updated_payload, "\"totalWeightKg\":18.50") != 0, "expected totalWeightKg after add_bunch_to_harvest_batch");
    banana_free(updated_payload);

    status = banana_get_harvest_batch_status("harvest-1", &status_payload);
    require_true(status == BANANA_STATUS_OK, "expected OK for get_harvest_batch_status");
    require_true(status_payload != 0, "expected payload for get_harvest_batch_status");
    require_true(strstr(status_payload, "\"fieldId\":\"field-7\"") != 0, "expected fieldId in harvest batch status payload");
    banana_free(status_payload);
}

static void test_harvest_batch_not_found_paths(void) {
    char* payload = 0;
    int status = banana_get_harvest_batch_status("missing-harvest", &payload);
    require_true(status == BANANA_STATUS_NOT_FOUND, "expected NOT_FOUND for missing harvest batch status");

    status = banana_add_bunch_to_harvest_batch("missing-harvest", "bunch-9", 2, 3.5, &payload);
    require_true(status == BANANA_STATUS_NOT_FOUND, "expected NOT_FOUND for missing harvest batch add");
}

static void test_truck_register_load_relocate_unload_and_status_ok(void) {
    char* payload = 0;
    char* loaded_payload = 0;
    char* relocated_payload = 0;
    char* unloaded_payload = 0;
    char* status_payload = 0;
    int status = banana_register_truck(
        "truck-1",
        "warehouse-1",
        CINTEROP_DISTRIBUTION_NODE_WAREHOUSE,
        9.90,
        -79.60,
        60.0,
        &payload);

    require_true(status == BANANA_STATUS_OK, "expected OK for register_truck");
    require_true(payload != 0, "expected payload for register_truck");
    require_true(strstr(payload, "\"truckId\":\"truck-1\"") != 0, "expected truckId in truck payload");
    require_true(strstr(payload, "\"nodeType\":\"WAREHOUSE\"") != 0, "expected WAREHOUSE nodeType in truck payload");
    banana_free(payload);

    status = banana_load_truck_container("truck-1", "container-1", 18.5, &loaded_payload);
    require_true(status == BANANA_STATUS_OK, "expected OK for load_truck_container");
    require_true(loaded_payload != 0, "expected payload for load_truck_container");
    require_true(strstr(loaded_payload, "\"containerCount\":1") != 0, "expected containerCount=1 after load_truck_container");
    require_true(strstr(loaded_payload, "\"currentLoadKg\":18.50") != 0, "expected currentLoadKg after load_truck_container");
    banana_free(loaded_payload);

    status = banana_relocate_truck(
        "truck-1",
        "port-3",
        CINTEROP_DISTRIBUTION_NODE_PORT,
        8.95,
        -79.55,
        &relocated_payload);
    require_true(status == BANANA_STATUS_OK, "expected OK for relocate_truck");
    require_true(relocated_payload != 0, "expected payload for relocate_truck");
    require_true(strstr(relocated_payload, "\"currentNodeId\":\"port-3\"") != 0, "expected currentNodeId after relocate_truck");
    require_true(strstr(relocated_payload, "\"nodeType\":\"PORT\"") != 0, "expected PORT nodeType after relocate_truck");
    banana_free(relocated_payload);

    status = banana_unload_truck_container("truck-1", "container-1", 18.5, &unloaded_payload);
    require_true(status == BANANA_STATUS_OK, "expected OK for unload_truck_container");
    require_true(unloaded_payload != 0, "expected payload for unload_truck_container");
    require_true(strstr(unloaded_payload, "\"containerCount\":0") != 0, "expected containerCount=0 after unload_truck_container");
    require_true(strstr(unloaded_payload, "\"currentLoadKg\":0.00") != 0, "expected currentLoadKg=0.00 after unload_truck_container");
    banana_free(unloaded_payload);

    status = banana_get_truck_status("truck-1", &status_payload);
    require_true(status == BANANA_STATUS_OK, "expected OK for get_truck_status");
    require_true(status_payload != 0, "expected payload for get_truck_status");
    require_true(strstr(status_payload, "\"truckId\":\"truck-1\"") != 0, "expected truckId in truck status payload");
    banana_free(status_payload);
}

static void test_truck_not_found_paths(void) {
    char* payload = 0;
    int status = banana_get_truck_status("missing-truck", &payload);
    require_true(status == BANANA_STATUS_NOT_FOUND, "expected NOT_FOUND for missing truck status");

    status = banana_load_truck_container("missing-truck", "container-1", 10.0, &payload);
    require_true(status == BANANA_STATUS_NOT_FOUND, "expected NOT_FOUND for missing truck load");
}

static void test_db_query_banana_invalid_args(void) {
    int row_count = 0;
    char* payload = 0;
    int status = banana_db_query_banana_profile(10, 2, 0, &row_count);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null payload output");

    status = banana_db_query_banana_profile(10, 2, &payload, 0);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for null row_count output");
}

static void test_db_query_banana_invalid_input(void) {
    char* payload = 0;
    int row_count = 0;
    int status = banana_db_query_banana_profile(-1, 2, &payload, &row_count);
    require_true(status == BANANA_STATUS_INVALID_ARGUMENT, "expected invalid argument for negative db_query_banana_profile input");
}

static void test_db_query_banana_forced_not_configured(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_DB_RESULT", "2") == 0, "failed to set BANANA_FORCE_DB_RESULT=2");
    status = banana_db_query_banana_profile(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("BANANA_FORCE_DB_RESULT") == 0, "failed to unset BANANA_FORCE_DB_RESULT");
    require_true(status == BANANA_STATUS_DB_NOT_CONFIGURED, "expected DB_NOT_CONFIGURED for forced db result 2");
}

static void test_db_query_banana_forced_db_error(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_DB_RESULT", "3") == 0, "failed to set BANANA_FORCE_DB_RESULT=3");
    status = banana_db_query_banana_profile(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("BANANA_FORCE_DB_RESULT") == 0, "failed to unset BANANA_FORCE_DB_RESULT");
    require_true(status == BANANA_STATUS_DB_ERROR, "expected DB_ERROR for forced db result 3");
}

static void test_db_query_banana_missing_connection(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(UNSET_ENV("BANANA_PG_CONNECTION") == 0, "failed to unset BANANA_PG_CONNECTION");
    status = banana_db_query_banana_profile(10, 2, &payload, &row_count);
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
    status = banana_db_query_banana_profile(10, 2, &payload, &row_count);
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
    status = banana_db_query_banana_profile(10, 2, &payload, &row_count);
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
    status = banana_db_query_banana_profile(10, 2, &payload, &row_count);
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
    status = banana_db_query_banana_profile(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("BANANA_FORCE_DB_RESULT") == 0, "failed to unset BANANA_FORCE_DB_RESULT");
    require_true(status == BANANA_STATUS_INTERNAL_ERROR, "expected INTERNAL_ERROR for forced db result 9");
}

static void test_db_query_banana_forced_payload_alloc_fail(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_PAYLOAD_ALLOC_FAIL", "1") == 0, "failed to set BANANA_FORCE_PAYLOAD_ALLOC_FAIL");
    status = banana_db_query_banana_profile(10, 2, &payload, &row_count);
    require_true(UNSET_ENV("BANANA_FORCE_PAYLOAD_ALLOC_FAIL") == 0, "failed to unset BANANA_FORCE_PAYLOAD_ALLOC_FAIL");
    require_true(status == BANANA_STATUS_DB_ERROR, "expected DB_ERROR when payload allocation is forced to fail");
}

static void test_db_query_banana_forced_payload_malloc_null(void) {
    char* payload = 0;
    int row_count = 0;
    int status;

    require_true(SET_ENV("BANANA_FORCE_PAYLOAD_MALLOC_NULL", "1") == 0, "failed to set BANANA_FORCE_PAYLOAD_MALLOC_NULL");
    status = banana_db_query_banana_profile(10, 2, &payload, &row_count);
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
    int status = banana_db_query_profile_projection(1, 1, 0, &row_count);
    require_true(status == 1, "expected direct banana_db_query_profile_projection null payload status");

    status = banana_db_query_profile_projection(1, 1, &payload, 0);
    require_true(status == 1, "expected direct banana_db_query_profile_projection null row_count status");
}

static void test_postgres_query_direct_validation_paths(void) {
    int banana = 0;
    int status = banana_postgres_query_banana_profile(1, 1, 0);

    require_true(status == BANANA_DB_INVALID_ARGUMENT, "expected invalid argument for null direct postgres out_banana");

    require_true(UNSET_ENV("BANANA_PG_CONNECTION") == 0, "failed to unset BANANA_PG_CONNECTION for direct postgres query");
    status = banana_postgres_query_banana_profile(1, 1, &banana);
    require_true(status == BANANA_DB_NOT_CONFIGURED, "expected not configured when direct postgres query has no connection string");
    require_true(SET_ENV("BANANA_PG_CONNECTION", "host=postgres port=5432 user=cinterop password=cinterop dbname=cinterop") == 0, "failed to restore BANANA_PG_CONNECTION for direct postgres query");
}

static void test_postgres_snapshot_validation_paths(void) {
    char snapshot[4] = { 't', 'e', 's', 't' };
    char loaded_snapshot[4] = { 0, 0, 0, 0 };
    int status = banana_postgres_upsert_snapshot(0, "aggregate-1", snapshot, sizeof(snapshot));

    require_true(status == BANANA_DB_INVALID_ARGUMENT, "expected invalid argument for null aggregate type in direct postgres upsert");

    status = banana_postgres_upsert_snapshot("aggregate-type", 0, snapshot, sizeof(snapshot));
    require_true(status == BANANA_DB_INVALID_ARGUMENT, "expected invalid argument for null aggregate id in direct postgres upsert");

    status = banana_postgres_upsert_snapshot("aggregate-type", "aggregate-1", 0, sizeof(snapshot));
    require_true(status == BANANA_DB_INVALID_ARGUMENT, "expected invalid argument for null snapshot in direct postgres upsert");

    status = banana_postgres_get_snapshot(0, "aggregate-1", loaded_snapshot, sizeof(loaded_snapshot));
    require_true(status == BANANA_DB_INVALID_ARGUMENT, "expected invalid argument for null aggregate type in direct postgres get");

    status = banana_postgres_get_snapshot("aggregate-type", 0, loaded_snapshot, sizeof(loaded_snapshot));
    require_true(status == BANANA_DB_INVALID_ARGUMENT, "expected invalid argument for null aggregate id in direct postgres get");

    status = banana_postgres_get_snapshot("aggregate-type", "aggregate-1", 0, sizeof(loaded_snapshot));
    require_true(status == BANANA_DB_INVALID_ARGUMENT, "expected invalid argument for null output snapshot in direct postgres get");

    status = banana_postgres_clear_snapshots(0);
    require_true(status == BANANA_DB_INVALID_ARGUMENT, "expected invalid argument for null aggregate type in direct postgres clear");
}

static void test_postgres_snapshot_forced_results(void) {
    char snapshot[4] = { 'd', 'a', 't', 'a' };
    char loaded_snapshot[4] = { 0, 0, 0, 0 };
    int status;

    require_true(SET_ENV("BANANA_FORCE_DB_RESULT", "3") == 0, "failed to set BANANA_FORCE_DB_RESULT=3 for direct postgres snapshot tests");

    status = banana_postgres_upsert_snapshot("aggregate-type", "aggregate-1", snapshot, sizeof(snapshot));
    require_true(status == BANANA_DB_ERROR, "expected forced DB_ERROR for direct postgres upsert snapshot");

    status = banana_postgres_get_snapshot("aggregate-type", "aggregate-1", loaded_snapshot, sizeof(loaded_snapshot));
    require_true(status == BANANA_DB_ERROR, "expected forced DB_ERROR for direct postgres get snapshot");

    status = banana_postgres_clear_snapshots("aggregate-type");
    require_true(status == BANANA_DB_ERROR, "expected forced DB_ERROR for direct postgres clear snapshots");

    require_true(UNSET_ENV("BANANA_FORCE_DB_RESULT") == 0, "failed to unset BANANA_FORCE_DB_RESULT=3 for direct postgres snapshot tests");

    require_true(SET_ENV("BANANA_FORCE_DB_RESULT", "4") == 0, "failed to set BANANA_FORCE_DB_RESULT=4 for direct postgres snapshot tests");
    status = banana_postgres_get_snapshot("aggregate-type", "aggregate-1", loaded_snapshot, sizeof(loaded_snapshot));
    require_true(status == BANANA_DB_NOT_FOUND, "expected forced DB_NOT_FOUND for direct postgres get snapshot");
    require_true(UNSET_ENV("BANANA_FORCE_DB_RESULT") == 0, "failed to unset BANANA_FORCE_DB_RESULT=4 for direct postgres snapshot tests");
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
    test_predict_banana_regression_score_ok();
    test_predict_banana_regression_score_invalid_args();
    test_classify_banana_binary_ok();
    test_classify_banana_binary_invalid_args();
    test_classify_banana_transformer_ok();
    test_classify_banana_transformer_invalid_args();
    test_db_query_banana_ok();
    test_predict_banana_ripeness_ok();
    test_predict_banana_ripeness_invalid_args();
    test_batch_create_and_status_ok();
    test_predict_batch_ripeness_ok();
    test_batch_not_found_paths();
    test_harvest_batch_create_add_and_status_ok();
    test_harvest_batch_not_found_paths();
    test_truck_register_load_relocate_unload_and_status_ok();
    test_truck_not_found_paths();
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
    test_postgres_query_direct_validation_paths();
    test_postgres_snapshot_validation_paths();
    test_postgres_snapshot_forced_results();
    test_free_null_is_safe();

    puts("native_wrapper_tests: all tests passed");
    return 0;
}
