#include "../banana_wrapper_internal.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../core/domain/banana_profile.h"
#include "../../testing/native_test_hooks.h"

static int try_calculate_profile_context(
    int purchases,
    int multiplier,
    BananaExecutionContext* context
) {
    BananaInput input;
    BananaRules rules = banana_default_rules();
    BananaStatus status = BANANA_OK;

    if (context == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    input.purchases = purchases;
    input.multiplier = multiplier;

    status = banana_prepare_execution_context(&input, &rules, context);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    status = banana_calculate_context(context);
    return banana_wrapper_map_status(status);
}

int banana_calculate_banana(int purchases, int multiplier, int* out_banana) {
    BananaExecutionContext context;
    int status = BANANA_STATUS_OK;

    if (out_banana == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    status = try_calculate_profile_context(purchases, multiplier, &context);
    if (status != BANANA_STATUS_OK) {
        return status;
    }

    *out_banana = context.result.banana;
    return BANANA_STATUS_OK;
}

int banana_calculate_banana_with_breakdown(
    int purchases,
    int multiplier,
    CInteropBananaBreakdown* out_breakdown
) {
    BananaExecutionContext context;
    int status = BANANA_STATUS_OK;

    if (out_breakdown == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    status = try_calculate_profile_context(purchases, multiplier, &context);
    if (status != BANANA_STATUS_OK) {
        return status;
    }

    if (banana_test_hook_force_summary_failure()) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    out_breakdown->purchases = context.input.purchases;
    out_breakdown->multiplier = context.input.multiplier;
    out_breakdown->banana = context.result.banana;
    return BANANA_STATUS_OK;
}

int banana_create_banana_message(int purchases, int multiplier, char** out_message) {
    int banana = 0;
    int status = BANANA_STATUS_OK;
    char* message = 0;
    int required_bytes = 0;
    BananaInput input;
    BananaRipenessPrediction prediction;
    const char* cultivar = banana_species_name(BANANA_SPECIES_CAVENDISH);
    const char* stage_name = "UNKNOWN";

    if (out_message == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    *out_message = 0;

    status = banana_calculate_banana(purchases, multiplier, &banana);
    if (status != BANANA_STATUS_OK) {
        return status;
    }

    input.purchases = purchases;
    input.multiplier = multiplier;
    if (banana_predict_ripeness_for_profile_input(&input, &prediction) != BANANA_OK) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    stage_name = banana_ripeness_stage_name(prediction.predicted_stage);
    required_bytes = snprintf(
        0,
        0,
        "banana_profile cultivar=%s harvest_day=%d ethylene_index=%d stage=%s shelf_life_hours=%d spoilage_probability=%.2f banana=%d",
        cultivar,
        purchases,
        multiplier,
        stage_name,
        prediction.shelf_life_hours,
        prediction.spoilage_probability,
        banana);
    if (required_bytes < 0) {
        return BANANA_STATUS_INTERNAL_ERROR; /* GCOVR_EXCL_LINE */
    }

    if (banana_test_hook_force_message_alloc_failure()) {
        message = 0;
    } else {
        message = (char*)malloc((size_t)required_bytes + 1U);
    }

    if (message == 0) {
        return BANANA_STATUS_INTERNAL_ERROR;
    }

    if (snprintf(
        message,
        (size_t)required_bytes + 1U,
        "banana_profile cultivar=%s harvest_day=%d ethylene_index=%d stage=%s shelf_life_hours=%d spoilage_probability=%.2f banana=%d",
        cultivar,
        purchases,
        multiplier,
        stage_name,
        prediction.shelf_life_hours,
        prediction.spoilage_probability,
        banana) < 0) {
        free(message); /* GCOVR_EXCL_LINE */
        return BANANA_STATUS_INTERNAL_ERROR; /* GCOVR_EXCL_LINE */
    }

    *out_message = message;
    return BANANA_STATUS_OK;
}