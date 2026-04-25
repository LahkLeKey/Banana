#include "banana_wrapper_ml_json.h"
#include "banana_wrapper.h"

#include <stdio.h>
#include <stdlib.h>

int banana_wrapper_ml_result_to_json(const BananaMlClassificationResult* result,
                                     const char* model_name,
                                     char** out_json) {
    int required;
    char* payload;

    if (!result || !model_name || !out_json || !result->label) {
        return BANANA_INVALID_ARGUMENT;
    }

    required = snprintf(
        NULL,
        0,
        "{\"model\":\"%s\",\"label\":\"%s\",\"confidence\":%.6f,"
        "\"banana_score\":%.6f,\"not_banana_score\":%.6f,\"jaccard\":%.6f,"
        "\"confusion_matrix\":{\"tp\":%d,\"fp\":%d,\"fn\":%d,\"tn\":%d}}",
        model_name,
        result->label,
        result->confidence,
        result->banana_score,
        result->not_banana_score,
        result->jaccard,
        result->confusion.tp,
        result->confusion.fp,
        result->confusion.fn,
        result->confusion.tn);

    if (required < 0) {
        return BANANA_INTERNAL_ERROR;
    }

    payload = (char*)malloc((size_t)required + 1u);
    if (!payload) {
        return BANANA_INTERNAL_ERROR;
    }

    if (snprintf(
            payload,
            (size_t)required + 1u,
            "{\"model\":\"%s\",\"label\":\"%s\",\"confidence\":%.6f,"
            "\"banana_score\":%.6f,\"not_banana_score\":%.6f,\"jaccard\":%.6f,"
            "\"confusion_matrix\":{\"tp\":%d,\"fp\":%d,\"fn\":%d,\"tn\":%d}}",
            model_name,
            result->label,
            result->confidence,
            result->banana_score,
            result->not_banana_score,
            result->jaccard,
            result->confusion.tp,
            result->confusion.fp,
            result->confusion.fn,
            result->confusion.tn) < 0) {
        free(payload);
        return BANANA_INTERNAL_ERROR;
    }

    *out_json = payload;
    return BANANA_OK;
}