#ifndef BANANA_WRAPPER_INTERNAL_H
#define BANANA_WRAPPER_INTERNAL_H

#include "banana_wrapper.h"

#include "../core/domain/banana_domain_status.h"
#include "../core/domain/banana_lifecycle.h"

static inline int banana_wrapper_map_status(BananaStatus status) {
    if (status == BANANA_OK) {
        return BANANA_STATUS_OK;
    }

    if (status == BANANA_ERROR_INVALID_INPUT) {
        return BANANA_STATUS_INVALID_ARGUMENT;
    }

    if (status == BANANA_ERROR_OVERFLOW) {
        return BANANA_STATUS_OVERFLOW;
    }

    if (status == BANANA_ERROR_NOT_FOUND) {
        return BANANA_STATUS_NOT_FOUND;
    }

    return BANANA_STATUS_INTERNAL_ERROR;
}

static inline void banana_wrapper_copy_ripeness_prediction(
    const BananaRipenessPrediction* prediction,
    CInteropBananaRipenessPrediction* out_prediction
) {
    out_prediction->predicted_stage = (CInteropBananaRipenessStage)prediction->predicted_stage;
    out_prediction->shelf_life_hours = prediction->shelf_life_hours;
    out_prediction->ripening_index = prediction->ripening_index;
    out_prediction->spoilage_probability = prediction->spoilage_probability;
    out_prediction->cold_chain_risk = prediction->cold_chain_risk;
}

#endif