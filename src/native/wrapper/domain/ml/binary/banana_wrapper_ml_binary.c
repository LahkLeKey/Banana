#include "banana_wrapper_ml_binary.h"
#include "domain/banana_ml_models.h"
#include "../shared/banana_wrapper_ml_json.h"

int banana_wrapper_ml_classify_binary(const char* input_json, char** out_json) {
    BananaMlClassificationResult result;
    int rc;

    rc = banana_ml_classify_binary(input_json, &result);
    if (rc != 0) {
        return rc;
    }

    return banana_wrapper_ml_result_to_json(&result, "binary", out_json);
}

int banana_wrapper_ml_classify_not_banana_junk(const char* input_json, char** out_json) {
    BananaMlClassificationResult result;
    int rc;

    rc = banana_ml_classify_not_banana_junk(input_json, &result);
    if (rc != 0) {
        return rc;
    }

    return banana_wrapper_ml_result_to_json(&result, "binary", out_json);
}