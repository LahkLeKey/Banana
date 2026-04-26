#ifndef BANANA_WRAPPER_ML_REGRESSION_H
#define BANANA_WRAPPER_ML_REGRESSION_H

#ifdef __cplusplus
extern "C" {
#endif

int banana_wrapper_ml_predict_regression_score(const char* input_json, double* out_score);

#ifdef __cplusplus
}
#endif

#endif