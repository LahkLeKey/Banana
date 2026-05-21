#include "banana_wrapper_ml_regression.h"
#include "banana_wrapper.h"
#include "../../../../ml/domain/regression/regression_model.h"

int banana_wrapper_ml_predict_regression_score(const char *input_json, double *out_score)
{
    if (!input_json || !out_score)
        return BANANA_INVALID_ARGUMENT;
    if (ml_regression_predict(input_json, out_score) != 0)
        return BANANA_INTERNAL_ERROR;
    return BANANA_OK;
}
