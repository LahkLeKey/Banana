#ifndef BANANA_WRAPPER_ML_JSON_H
#define BANANA_WRAPPER_ML_JSON_H

typedef struct BananaMlConfusionMatrix
{
    int tp;
    int fp;
    int fn;
    int tn;
} BananaMlConfusionMatrix;

typedef struct BananaMlClassificationResult
{
    const char *label;
    double confidence;
    double banana_score;
    double not_banana_score;
    double jaccard;
    BananaMlConfusionMatrix confusion;
} BananaMlClassificationResult;

#ifdef __cplusplus
extern "C"
{
#endif

    int banana_wrapper_ml_result_to_json(const BananaMlClassificationResult *result,
                                         const char *model_name, char **out_json);

#ifdef __cplusplus
}
#endif

#endif
