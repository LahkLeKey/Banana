// Native deterministic ML tests for regression, binary metrics, and transformer.
#include "../../src/native/wrapper/banana_wrapper.h"

#include <stdio.h>
#include <string.h>

#define CHECK(cond, msg)                              \
    do                                                \
    {                                                 \
        if (!(cond))                                  \
        {                                             \
            fprintf(stderr, "[native-unit] %s\n", msg); \
            return 1;                                 \
        }                                             \
    } while (0)

static int assert_contains(const char* text, const char* needle) {
    if (text == NULL || needle == NULL) {
        return 0;
    }
    return strstr(text, needle) != NULL;
}

int main(void) {
    const char* transformer_payload = "{\"text\":\"ripe banana bunch banana bread\"}";
    double score = 0.0;
    char* binary_json = NULL;
    char* junk_json = NULL;
    char* transformer_json = NULL;
    char* transformer_json_second = NULL;
    int rc;

    rc = banana_predict_banana_regression_score("{\"text\":\"ripe banana peel smoothie\"}", &score);
    CHECK(rc == BANANA_OK, "banana_predict_banana_regression_score failed");
    CHECK(score >= 0.0 && score <= 1.0, "regression score out of range");
    CHECK(score > 0.5, "regression score expected to indicate banana-like input");

    rc = banana_classify_banana_binary("{\"text\":\"fresh banana peel smoothie\"}", &binary_json);
    CHECK(rc == BANANA_OK, "banana_classify_banana_binary failed");
    CHECK(binary_json != NULL, "banana_classify_banana_binary returned NULL payload");
        CHECK(assert_contains(binary_json, "\"model\":\"binary\""),
            "binary payload missing model field");
        CHECK(assert_contains(binary_json, "\"label\":\"banana\""),
            "binary payload missing label field");
        CHECK(assert_contains(binary_json, "\"jaccard\":"),
            "binary payload missing jaccard field");
        CHECK(assert_contains(binary_json, "\"confusion_matrix\":"),
            "binary payload missing confusion_matrix field");
    banana_free(binary_json);

    rc = banana_classify_not_banana_junk("{\"text\":\"plastic engine oil junk waste\"}", &junk_json);
    CHECK(rc == BANANA_OK, "banana_classify_not_banana_junk failed");
    CHECK(junk_json != NULL, "banana_classify_not_banana_junk returned NULL payload");
        CHECK(assert_contains(junk_json, "\"label\":\"not_banana\""),
            "junk payload missing not_banana label");
    banana_free(junk_json);

    rc = banana_classify_banana_transformer(transformer_payload, &transformer_json);
    CHECK(rc == BANANA_OK, "banana_classify_banana_transformer failed");
    CHECK(transformer_json != NULL, "banana_classify_banana_transformer returned NULL payload");
        CHECK(assert_contains(transformer_json, "\"model\":\"transformer\""),
            "transformer payload missing model field");
        CHECK(assert_contains(transformer_json, "\"label\":\"banana\""),
            "transformer payload missing label field");

    rc = banana_classify_banana_transformer(transformer_payload, &transformer_json_second);
        CHECK(rc == BANANA_OK, "second banana_classify_banana_transformer call failed");
        CHECK(transformer_json_second != NULL,
            "second banana_classify_banana_transformer call returned NULL payload");
        CHECK(strcmp(transformer_json, transformer_json_second) == 0,
            "transformer results were not deterministic");

    banana_free(transformer_json);
    banana_free(transformer_json_second);

    rc = banana_predict_banana_regression_score(NULL, &score);
        CHECK(rc == BANANA_INVALID_ARGUMENT,
            "banana_predict_banana_regression_score should reject NULL payload");

    rc = banana_classify_banana_binary(NULL, &binary_json);
        CHECK(rc == BANANA_INVALID_ARGUMENT,
            "banana_classify_banana_binary should reject NULL payload");

    printf("[native-unit] deterministic ML runtime models ok\n");
    return 0;
}
