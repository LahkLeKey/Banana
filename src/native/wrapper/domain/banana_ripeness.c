#include "../banana_wrapper_internal.h"

#include "../../core/domain/banana_lifecycle.h"

int banana_predict_banana_ripeness(
    const double* temperature_history_c,
    int temperature_history_count,
    int days_since_harvest,
    double ethylene_exposure,
    double mechanical_damage,
    double storage_temp_c,
    CInteropBananaRipenessPrediction* out_prediction
) {
    BananaRipenessInput input;
    BananaRipenessPrediction prediction;
    BananaStatus status = BANANA_OK;

    if (out_prediction == 0) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    input.temperature_history_c = temperature_history_c;
    input.temperature_history_count = temperature_history_count;
    input.days_since_harvest = days_since_harvest;
    input.ethylene_exposure = ethylene_exposure;
    input.mechanical_damage = mechanical_damage;
    input.storage_temp_c = storage_temp_c;

    status = banana_predict_ripeness(&input, &prediction);
    if (status != BANANA_OK) {
        return banana_wrapper_map_status(status);
    }

    banana_wrapper_copy_ripeness_prediction(&prediction, out_prediction);
    return BANANA_STATUS_OK;
}