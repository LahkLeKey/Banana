#include "banana_wrapper_ml_binary.h"
#include "../shared/banana_wrapper_ml_json.h"
#include "banana_wrapper.h"
#include "domain/banana_ml_models.h"
#include "domain/ml/binary/banana_ml_binary.h"
#include "domain/ml/shared/banana_ml_shared.h"

#include <string.h>

int banana_wrapper_ml_classify_binary(const char *input_json, char **out_json)
{
    BananaMlClassificationResult result;
    int rc;

    rc = banana_ml_classify_binary(input_json, &result);
    if (rc != 0)
    {
        return rc;
    }

    return banana_wrapper_ml_result_to_json(&result, "binary", out_json);
}

int banana_wrapper_ml_classify_not_banana_junk(const char *input_json, char **out_json)
{
    BananaMlClassificationResult result;
    int rc;

    rc = banana_ml_classify_not_banana_junk(input_json, &result);
    if (rc != 0)
    {
        return rc;
    }

    return banana_wrapper_ml_result_to_json(&result, "binary", out_json);
}

int banana_wrapper_ml_classify_binary_with_threshold(const char *input_json, double threshold,
                                                     char **out_json)
{
    BananaMlFeatureVector features;
    BananaMlClassificationResult result;
    int rc;

    if (!input_json || !out_json)
    {
        return BANANA_INVALID_ARGUMENT;
    }

    memset(&result, 0, sizeof(result));

    rc = banana_ml_build_feature_vector(input_json, &features);
    if (rc != BANANA_OK)
    {
        return rc;
    }

    rc = banana_ml_binary_classify_with_threshold(&features, threshold, &result);
    if (rc != BANANA_OK)
    {
        return rc;
    }

    return banana_wrapper_ml_result_to_json(&result, "binary", out_json);
}
