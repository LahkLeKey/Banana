#ifndef BANANA_ML_BINARY_H
#define BANANA_ML_BINARY_H

#include "domain/banana_ml_models.h"

#ifdef __cplusplus
extern "C" {
#endif

int banana_ml_binary_classify(const BananaMlFeatureVector* features,
                              BananaMlClassificationResult* out_result);
int banana_ml_binary_classify_with_not_banana_bias(const BananaMlFeatureVector* features,
                                                    BananaMlClassificationResult* out_result);

#ifdef __cplusplus
}
#endif

#endif