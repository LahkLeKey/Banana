#ifndef BANANA_ML_INTERNAL_H
#define BANANA_ML_INTERNAL_H

#include "../../banana_ml_models.h"

int banana_ml_internal_value_is_valid(double value);

double banana_ml_internal_absolute(double value);

double banana_ml_internal_clamp(double value, double min_value, double max_value);

double banana_ml_internal_dot(const double* left, const double* right, int count);

BananaStatus banana_ml_internal_validate_feature_vector(const BananaMlFeatureVector* features);

double banana_ml_internal_linear_score(
    const double* weights,
    const BananaMlFeatureVector* features,
    double bias
);

double banana_ml_internal_pseudo_sigmoid(double value);

#endif