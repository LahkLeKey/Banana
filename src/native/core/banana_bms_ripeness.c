#include "banana_bms.h"

static double clamp_double(double value, double min, double max) {
    if (value < min) {
        return min;
    }

    if (value > max) {
        return max;
    }

    return value;
}

static double average_temperature(const double* history, int count, double fallback) {
    double total = 0.0;
    int index = 0;

    if (history == 0 || count <= 0) {
        return fallback;
    }

    for (index = 0; index < count; index++) {
        total += history[index];
    }

    return total / (double)count;
}

static double stage_upper_bound(BananaRipenessStage stage) {
    switch (stage) {
        case BANANA_STAGE_GREEN:
            return 80.0;
        case BANANA_STAGE_BREAKING:
            return 140.0;
        case BANANA_STAGE_YELLOW:
            return 220.0;
        case BANANA_STAGE_SPOTTED:
            return 300.0;
        case BANANA_STAGE_OVERRIPE:
            return 360.0;
        case BANANA_STAGE_BIODEGRADATION:
        default:
            return 360.0;
    }
}

static BananaRipenessStage stage_from_index(double ripening_index) {
    if (ripening_index < 80.0) {
        return BANANA_STAGE_GREEN;
    }

    if (ripening_index < 140.0) {
        return BANANA_STAGE_BREAKING;
    }

    if (ripening_index < 220.0) {
        return BANANA_STAGE_YELLOW;
    }

    if (ripening_index < 300.0) {
        return BANANA_STAGE_SPOTTED;
    }

    if (ripening_index < 360.0) {
        return BANANA_STAGE_OVERRIPE;
    }

    return BANANA_STAGE_BIODEGRADATION;
}

static double temperature_multiplier(double average_temp) {
    if (average_temp < 13.0) {
        return 0.55;
    }

    if (average_temp <= 16.0) {
        return 0.85;
    }

    if (average_temp <= 20.0) {
        return 1.0;
    }

    if (average_temp <= 24.0) {
        return 1.2;
    }

    return 1.45;
}

BananaStatus banana_predict_ripeness(
    const BananaRipenessInput* input,
    BananaRipenessPrediction* prediction
) {
    double average_temp = 0.0;
    double ripening_index = 0.0;
    double remaining_index = 0.0;

    if (input == 0 || prediction == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    if (input->days_since_harvest < 0
        || input->ethylene_exposure < 0.0
        || input->mechanical_damage < 0.0
        || input->storage_temp_c < 0.0
        || input->storage_temp_c > 60.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    average_temp = average_temperature(
        input->temperature_history_c,
        input->temperature_history_count,
        input->storage_temp_c);

    if (average_temp < 0.0 || average_temp > 60.0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    ripening_index = ((double)input->days_since_harvest * 18.0) * temperature_multiplier(average_temp);
    ripening_index += input->ethylene_exposure * 4.0;
    ripening_index += input->mechanical_damage * 30.0;

    prediction->predicted_stage = stage_from_index(ripening_index);
    prediction->ripening_index = ripening_index;
    prediction->cold_chain_risk = clamp_double(
        ((average_temp > 18.0) ? 0.25 : 0.05)
            + ((average_temp < 11.0) ? 0.35 : 0.0),
        0.0,
        1.0);
    prediction->spoilage_probability = clamp_double(
        (ripening_index / 360.0)
            + (input->mechanical_damage * 0.35)
            + ((average_temp > 24.0) ? 0.12 : 0.0),
        0.0,
        1.0);

    remaining_index = stage_upper_bound(BANANA_STAGE_BIODEGRADATION) - ripening_index;
    if (remaining_index < 0.0) {
        remaining_index = 0.0;
    }

    prediction->shelf_life_hours = (int)remaining_index;
    return BANANA_OK;
}