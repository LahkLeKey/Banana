#ifndef BANANA_ML_REGRESSION_H
#define BANANA_ML_REGRESSION_H

#include "domain/banana_ml_models.h"

#ifdef __cplusplus
extern "C" {
#endif

int banana_ml_regression_predict(const BananaMlFeatureVector* features, double* out_score);

#ifdef __cplusplus
}
#endif

#endif