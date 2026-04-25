#ifndef BANANA_ML_TRANSFORMER_H
#define BANANA_ML_TRANSFORMER_H

#include "domain/banana_ml_models.h"

#ifdef __cplusplus
extern "C" {
#endif

int banana_ml_transformer_classify(const BananaMlFeatureVector* features,
                                   BananaMlClassificationResult* out_result);

#ifdef __cplusplus
}
#endif

#endif