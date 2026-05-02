#ifndef BANANA_ML_MODELS_H
#define BANANA_ML_MODELS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct BananaMlConfusionMatrix
    {
        int tp;
        int fp;
        int fn;
        int tn;
    } BananaMlConfusionMatrix;

    typedef struct BananaMlFeatureVector
    {
        size_t token_count;
        size_t unique_token_count;
        size_t banana_hits;
        size_t not_banana_hits;
        size_t positive_bigram_hits;
        size_t negative_bigram_hits;
        double banana_signal_ratio;
        double not_banana_signal_ratio;
        double unique_token_ratio;
        double length_ratio;
        double positive_bigram_ratio;
        double negative_bigram_ratio;
        double banana_attention_ratio;
        double not_banana_attention_ratio;
    } BananaMlFeatureVector;

    typedef struct BananaMlClassificationResult
    {
        const char *label;
        double confidence;
        double banana_score;
        double not_banana_score;
        double jaccard;
        BananaMlConfusionMatrix confusion;
    } BananaMlClassificationResult;

    /* Returns BananaStatusCode values declared in banana_wrapper.h. */
    int banana_ml_predict_regression_score(const char *input_json, double *out_score);
    int banana_ml_classify_binary(const char *input_json, BananaMlClassificationResult *out_result);
    int banana_ml_classify_transformer(const char *input_json,
                                       BananaMlClassificationResult *out_result);
    int banana_ml_classify_not_banana_junk(const char *input_json,
                                           BananaMlClassificationResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
