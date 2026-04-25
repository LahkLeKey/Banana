#ifndef BANANA_WRAPPER_ML_JSON_H
#define BANANA_WRAPPER_ML_JSON_H

#include "domain/banana_ml_models.h"

#ifdef __cplusplus
extern "C" {
#endif

int banana_wrapper_ml_result_to_json(const BananaMlClassificationResult* result,
                                     const char* model_name,
                                     char** out_json);

#ifdef __cplusplus
}
#endif

#endif