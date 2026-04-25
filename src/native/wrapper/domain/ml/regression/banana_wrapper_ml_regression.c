#include "banana_wrapper_ml_regression.h"
#include "domain/banana_ml_models.h"

int banana_wrapper_ml_predict_regression_score(const char* input_json, double* out_score) {
    return banana_ml_predict_regression_score(input_json, out_score);
}