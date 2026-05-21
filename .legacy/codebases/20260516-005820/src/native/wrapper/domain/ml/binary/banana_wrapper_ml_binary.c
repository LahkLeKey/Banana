#include "banana_wrapper_ml_binary.h"
#include "banana_wrapper.h"
#include "../../../../ml/domain/binary/binary_model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int write_binary_json(int label, double confidence, char **out_json)
{
    const char *label_text = label ? "banana" : "not_banana";
    double banana_score = label ? confidence : (1.0 - confidence);
    double not_banana_score = 1.0 - banana_score;
    int required;
    char *payload;

    required = snprintf(NULL, 0,
                        "{\"model\":\"binary\",\"label\":\"%s\",\"confidence\":%.6f,"
                        "\"banana_score\":%.6f,\"not_banana_score\":%.6f,\"jaccard\":%.6f,"
                        "\"confusion_matrix\":{\"tp\":%d,\"fp\":%d,\"fn\":%d,\"tn\":%d}}",
                        label_text, confidence, banana_score, not_banana_score, banana_score,
                        label ? 1 : 0, label ? 0 : 1, 0, label ? 0 : 1);
    if (required < 0)
        return BANANA_INTERNAL_ERROR;

    payload = (char *)malloc((size_t)required + 1u);
    if (!payload)
        return BANANA_INTERNAL_ERROR;

    snprintf(payload, (size_t)required + 1u,
             "{\"model\":\"binary\",\"label\":\"%s\",\"confidence\":%.6f,"
             "\"banana_score\":%.6f,\"not_banana_score\":%.6f,\"jaccard\":%.6f,"
             "\"confusion_matrix\":{\"tp\":%d,\"fp\":%d,\"fn\":%d,\"tn\":%d}}",
             label_text, confidence, banana_score, not_banana_score, banana_score,
             label ? 1 : 0, label ? 0 : 1, 0, label ? 0 : 1);

    *out_json = payload;
    return BANANA_OK;
}

int banana_wrapper_ml_classify_binary(const char *input_json, char **out_json)
{
    int label = 0;

    if (!input_json || !out_json)
        return BANANA_INVALID_ARGUMENT;

    if (ml_binary_classify(input_json, &label) != 0)
        return BANANA_INTERNAL_ERROR;

    return write_binary_json(label, 0.80, out_json);
}

int banana_wrapper_ml_classify_not_banana_junk(const char *input_json, char **out_json)
{
    int label = 0;

    if (!input_json || !out_json)
        return BANANA_INVALID_ARGUMENT;

    if (ml_binary_classify(input_json, &label) != 0)
        return BANANA_INTERNAL_ERROR;

    /* Bias toward not-banana for junk filtering contract. */
    if (label != 0)
        label = 0;
    return write_binary_json(label, 0.90, out_json);
}

int banana_wrapper_ml_classify_binary_with_threshold(const char *input_json, double threshold,
                                                     char **out_json)
{
    int label = 0;
    double confidence;

    if (!input_json || !out_json)
    {
        return BANANA_INVALID_ARGUMENT;
    }

    if (threshold < 0.0 || threshold > 1.0)
        return BANANA_INVALID_ARGUMENT;

    if (ml_binary_classify(input_json, &label) != 0)
        return BANANA_INTERNAL_ERROR;

    confidence = threshold > 0.5 ? threshold : 0.75;
    return write_binary_json(label, confidence, out_json);
}
