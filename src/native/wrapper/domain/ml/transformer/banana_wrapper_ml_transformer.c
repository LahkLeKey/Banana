#include "banana_wrapper_ml_transformer.h"
#include "domain/banana_ml_models.h"
#include "../shared/banana_wrapper_ml_json.h"

int banana_wrapper_ml_classify_transformer(const char* input_json, char** out_json) {
    BananaMlClassificationResult result;
    int rc;

    rc = banana_ml_classify_transformer(input_json, &result);
    if (rc != 0) {
        return rc;
    }

    return banana_wrapper_ml_result_to_json(&result, "transformer", out_json);
}