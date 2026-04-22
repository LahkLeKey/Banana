#include "banana_ml_internal.h"

int banana_ml_internal_value_is_valid(double value) {
    if (value != value) {
        return 0;
    }

    if (value > 1000000.0 || value < -1000000.0) {
        return 0;
    }

    return 1;
}

double banana_ml_internal_absolute(double value) {
    return value < 0.0 ? -value : value;
}

double banana_ml_internal_clamp(double value, double min_value, double max_value) {
    if (value < min_value) {
        return min_value;
    }

    if (value > max_value) {
        return max_value;
    }

    return value;
}

double banana_ml_internal_dot(const double* left, const double* right, int count) {
    double result = 0.0;

    for (int index = 0; index < count; index++) {
        result += left[index] * right[index];
    }

    return result;
}

BananaStatus banana_ml_internal_validate_feature_vector(const BananaMlFeatureVector* features) {
    if (features == 0) {
        return BANANA_ERROR_INVALID_INPUT;
    }

    for (int index = 0; index < BANANA_ML_FEATURE_COUNT; index++) {
        if (!banana_ml_internal_value_is_valid(features->values[index])) {
            return BANANA_ERROR_INVALID_INPUT;
        }
    }

    return BANANA_OK;
}

double banana_ml_internal_linear_score(
    const double* weights,
    const BananaMlFeatureVector* features,
    double bias
) {
    return bias + banana_ml_internal_dot(weights, features->values, BANANA_ML_FEATURE_COUNT);
}

double banana_ml_internal_pseudo_sigmoid(double value) {
    double magnitude = banana_ml_internal_absolute(value);
    return 0.5 + (value / (2.0 * (1.0 + magnitude)));
}