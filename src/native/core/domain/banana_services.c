#include "banana_services.h"

double banana_quality_control_average_fruit_weight_kg(const BananaBunchRecord* bunch) {
    if (bunch == 0 || bunch->banana_count <= 0) {
        return 0.0;
    }

    return bunch->total_weight_kg / (double)bunch->banana_count;
}

int banana_quality_control_is_underweight(
    const BananaBunchRecord* bunch,
    double minimum_average_weight_kg
) {
    if (minimum_average_weight_kg <= 0.0) {
        return 0;
    }

    return banana_quality_control_average_fruit_weight_kg(bunch) < minimum_average_weight_kg;
}

BananaStatus banana_ripening_service_evaluate_bunch(
    const BananaBunchRecord* bunch,
    const BananaRipenessInput* input,
    BananaRipenessPrediction* prediction
) {
    BananaStatus status = BANANA_OK;

    if (bunch == 0 || input == 0 || prediction == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    status = banana_bunch_record_validate(bunch);
    if (status != BANANA_OK) {
        return status;
    }

    return banana_predict_ripeness(input, prediction);
}

BananaStatus banana_ripening_service_apply(
    BananaBunchRecord* bunch,
    const BananaRipenessInput* input,
    int event_day_ordinal,
    BananaRipenessPrediction* prediction,
    BananaDomainEvent* event
) {
    BananaStatus status = BANANA_OK;
    BananaRipenessPrediction local_prediction;
    BananaRipenessPrediction* resolved_prediction = prediction;

    if (resolved_prediction == 0) {
        resolved_prediction = &local_prediction;
    }

    status = banana_ripening_service_evaluate_bunch(bunch, input, resolved_prediction);
    if (status != BANANA_OK) {
        return status;
    }

    return banana_bunch_record_update_ripeness(
        bunch,
        resolved_prediction->predicted_stage,
        event_day_ordinal,
        event);
}